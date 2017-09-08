#include "server_launcher.h"
#include "kv_request_handler.h"
#include "server.h"
#include "sharding_proxy_worker_factory.h"
#include "rdma/rdma_server.h"
#include "sqpkv/common.h"

#include "gflags/gflags.h"
#include "mpi.h"
#include "spdlog/spdlog.h"

#include <fstream>

#include <cerrno>
#include <cstdlib>
#include <cstring>

#include <ifaddrs.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

DEFINE_string(kv_path, "/tmp/sqpkv", "Path of the KV store.");
DEFINE_string(proxy_hostname, "/tmp/sqp_proxy", "File name to save the hostname of the proxy server to.");
DEFINE_int32(port, 4242, "Port to listen to.");

namespace sqpkv {

static rocksdb::SliceTransform *NewTablePrefixTransform() {
  return new sqpkv::TablePrefixTransform();
}

ServerLauncher::ServerLauncher() :
    shard_id_(-1), world_size_(0), world_rank_(-1),
    proxy_rank_(-1), db_(nullptr) {}

int ServerLauncher::Run() {
  InitShardingConfig();
  int ret_val;
  if (shard_id_ == 0) {
    ret_val = ProxyMain();
  } else {
    ret_val = ShardMain();
  }
  MPI_Finalize();
  return ret_val;
}

void ServerLauncher::InitShardingConfig() {
  MPI_Init(nullptr, nullptr);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank_);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size_);

  OpenDb();
  GetShardId();
  ShakeHands();
  if (proxy_rank_ == -1) {
    delete db_;
    exit(EXIT_FAILURE);
  }
  if (shard_id_ == 0) {
    spdlog::set_pattern("[Proxy] %v");
  } else {
    spdlog::set_pattern("[Shard " + std::to_string(shard_id_) + "] %v");
  }
  auto s = ResolveIpAddresses();
  if (!s.ok()) {
    spdlog::get("console")->error(s.message());
    exit(EXIT_FAILURE);
  }
}

void ServerLauncher::OpenDb() {
  rocksdb::Options options;
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  options.create_if_missing = true;
  options.prefix_extractor.reset(NewTablePrefixTransform());
  std::string kv_path = FLAGS_kv_path + std::to_string(world_rank_);
  spdlog::get("console")->debug("Opening db {}", kv_path);
  rocksdb::Status s = rocksdb::DB::Open(options, kv_path, &db_);
  if (!s.ok()) {
    spdlog::get("console")->error(s.ToString());
  }
}

void ServerLauncher::GetShardId() {
  if (db_ == nullptr) {
    shard_id_ = -1;
    return;
  }
  std::string shard_id_key = "shard_id";
  std::string shard_id_value;
  auto s = db_->Get(rocksdb::ReadOptions(), shard_id_key, &shard_id_value);
  if (!s.ok()) {
    if (s.IsNotFound()) {
      spdlog::get("console")->debug("No shard id found in the kv store");
      shard_id_ = world_rank_;
    } else {
      shard_id_ = -1;
    }
  } else {
    spdlog::get("console")->debug("Got shard id from kv store: {}", shard_id_value);
    shard_id_ = stoi(shard_id_value);
  }
}

void ServerLauncher::ShakeHands() {
  if (world_rank_ == 0) {
    proxy_rank_ = world_rank_;
    if (shard_id_ == -1) {
      proxy_rank_ = -1;
    }
    id_to_rank[shard_id_] = world_rank_;
    for (int rank = 1; rank < world_size_; rank++) {
      int comm_shard_id;
      MPI_Recv(&comm_shard_id, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      spdlog::get("console")->debug("Rank 0 receives shard id {} from rank {}", comm_shard_id, rank);
      if (comm_shard_id == -1) {
        proxy_rank_ = -1;
      } else if (proxy_rank_ != -1 && comm_shard_id == 0) {
        proxy_rank_ = rank;
      }
      id_to_rank[comm_shard_id] = rank;
    }
    spdlog::get("console")->debug("Broadcasting proxy rank {} to all others", proxy_rank_);
    MPI_Bcast(&proxy_rank_, 1, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Send(&shard_id_, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Bcast(&proxy_rank_, 1, MPI_INT, 0, MPI_COMM_WORLD);
  }
  if (proxy_rank_ == -1) {
    spdlog::get("console")->error("Handshake failed due to error in shard id creation.");
    return;
  }
  auto s = db_->Put(rocksdb::WriteOptions(), "shard_id", std::to_string(world_rank_));
  int save_success = s.ok();
  if (world_rank_ ==  proxy_rank_) {
    for (int shard_id = 1; shard_id < world_size_; shard_id++) {
      int rank = id_to_rank[shard_id];
      int comm_save_success;
      MPI_Recv(&comm_save_success, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      spdlog::get("console")->debug("Rank {} receives save result {} from rank {}", proxy_rank_, comm_save_success, rank);
      save_success =  comm_save_success && save_success;
    }
    spdlog::get("console")->debug("Rank {} broadcasting save result {} to all others", proxy_rank_, save_success);
    MPI_Bcast(&save_success, 1, MPI_INT, proxy_rank_, MPI_COMM_WORLD);
  } else {
    MPI_Send(&save_success, 1, MPI_INT, proxy_rank_, 0, MPI_COMM_WORLD);
    MPI_Bcast(&save_success, 1, MPI_INT, proxy_rank_, MPI_COMM_WORLD);
  }
  proxy_rank_ = save_success ? proxy_rank_ : -1;
  if (proxy_rank_ == -1) {
    spdlog::get("console")->error("Handshake failed due to error in shard id preservation.");
  }
}

int ServerLauncher::GetInfinibandIp(char *host) {
  struct ifaddrs *ifaddr, *ifa;
  int s, n;

  if (getifaddrs(&ifaddr) == -1) {
    spdlog::get("console")->debug("getifaddrs() error: {}}", strerror(errno));
    return -1;
  }

  /* Walk through linked list, maintaining head pointer so we
   *               can free list later */

  int ip_found = 0;
  for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
    if (ifa->ifa_addr == NULL)
      continue;

    if (strncmp(ifa->ifa_name, "ib0", 3) == 0 &&
        ifa->ifa_addr->sa_family == AF_INET) {

      s = getnameinfo(ifa->ifa_addr,
          sizeof(*ifa->ifa_addr),
          host, NI_MAXHOST,
          NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        spdlog::get("console")->debug("getifaddrs() error: {}}", gai_strerror(errno));
        return -1;
      }

      ip_found = 1;
    }
  }

  if (ip_found) {
    return 0;
  } else {
    spdlog::get("console")->debug("Infiniband ip address not found");
    return -1;
  }
}

Status ServerLauncher::ResolveIpAddresses() {
  char ip_address[NI_MAXHOST];
  int ip_address_size;
  int ip_address_resolution_success = true;
  if (world_rank_ == proxy_rank_) {
    GetInfinibandIp(ip_address);
    std::ofstream proxy_ip_address_file(FLAGS_proxy_hostname);
    proxy_ip_address_file << ip_address << std::endl;
    proxy_ip_address_file.close();
    for (int shard_id = 1; shard_id < world_size_; shard_id++) {
      int rank = id_to_rank[shard_id];
      MPI_Recv(&ip_address_size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      spdlog::get("console")->debug("Rank {} receives ip address size {} from rank {}", proxy_rank_, ip_address_size, rank);
      if (ip_address_size == -1) {
        ip_address_resolution_success = false;
        continue;
      }
      MPI_Recv(ip_address, ip_address_size, MPI_CHAR, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      spdlog::get("console")->debug("Rank {} receives ip address {} from rank {}", proxy_rank_, ip_address, rank);
      ip_addresses_.push_back(std::string(ip_address));
    }
    spdlog::get("console")->debug("Rank {} broadcasting ip address resolution result {} to all others", proxy_rank_, ip_address_resolution_success);
    MPI_Bcast(&ip_address_resolution_success, 1, MPI_INT, proxy_rank_, MPI_COMM_WORLD);
  } else {
    int rc = GetInfinibandIp(ip_address);
    if (rc == -1) {
      ip_address_size = -1;
      MPI_Send(&ip_address_size, 1, MPI_INT, proxy_rank_, 0, MPI_COMM_WORLD);
    } else {
      ip_address_size = strlen(ip_address) + 1;
      MPI_Send(&ip_address_size, 1, MPI_INT, proxy_rank_, 0, MPI_COMM_WORLD);
      MPI_Send(ip_address, ip_address_size, MPI_CHAR, proxy_rank_, 0, MPI_COMM_WORLD);
    }
    MPI_Bcast(&ip_address_resolution_success, 1, MPI_INT, proxy_rank_, MPI_COMM_WORLD);
    spdlog::get("console")->debug("Rank {} receives ip address resolution result {} from rank {}", world_rank_, ip_address_resolution_success, proxy_rank_);
  }
  return ip_address_resolution_success ? Status::Ok() : Status::Err();
}

void ServerLauncher::ExchangeRDMAPort(int port) {
  if (world_rank_ == proxy_rank_) {
    for (int shard_id = 1; shard_id < world_size_; shard_id++) {
      int rank = id_to_rank[shard_id];
      int port = -1;
      MPI_Recv(&port, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      spdlog::get("console")->debug("Rank {} receives port {} from rank {}", proxy_rank_, port, rank);
      ports_.push_back(port);
    }
  } else {
    MPI_Send(&port, 1, MPI_INT, proxy_rank_, 0, MPI_COMM_WORLD);
  }
}

int ServerLauncher::ProxyMain() {
  ExchangeRDMAPort(0);
  auto worker_factory = std::unique_ptr<WorkerFactory>(
    new sqpkv::ShardingProxyWorkerFactory(ip_addresses_, ports_, FLAGS_port));
  sqpkv::Server *server = sqpkv::Server::GetInstance(std::move(worker_factory), FLAGS_port);
  server->Start();

  return 0;
}

int ServerLauncher::ShardMain() {
  auto request_handler = make_unique<sqpkv::KvRequestHandler>(db_);
  sqpkv::RDMAServer server(request_handler.get());
  server.Initialize();
  int port = server.port();
  ExchangeRDMAPort(port);
  server.Run();
  return 0;
}

} // namespace sqpkv

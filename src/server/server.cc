#include "server.h"

#include "spdlog/spdlog.h"

#include <cstring>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

namespace sqpkv {

Server *Server::instance = nullptr;

Server *Server::GetInstance(std::unique_ptr<WorkerFactory> worker_factory, int port) {
  if (port > 0 && instance == nullptr) {
    instance = new Server(std::move(worker_factory), port);
  }
  return instance;
}

Server::Server(std::unique_ptr<WorkerFactory> worker_factory, int port) :
    worker_factory_(std::move(worker_factory)), port_(port), sock_fd_(-1) {}

Server::~Server() {
  Stop();
}

void Server::Start() {
  struct sockaddr_in serv_addr;
  sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd_ < 0) {
    spdlog::get("console")->error("Error creating socket: {}", strerror(errno));
    return;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port_);
  if (bind(sock_fd_, (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0) {
    spdlog::get("console")->error("Error binding socket: {}", strerror(errno));
    return;
  }
  listen(sock_fd_, 5);

  while (Accept()) {
    // Accept more.
  }
}

bool Server::Accept() {
  socklen_t clilen;
  struct sockaddr_in cli_addr;
  clilen = sizeof(cli_addr);
  int client_fd = accept(sock_fd_, (struct sockaddr *) &cli_addr, &clilen);
  if (client_fd < 0) {
    spdlog::get("console")->error("Error accpeting connection: {}", strerror(errno));
    return false;
  }
  auto worker = worker_factory_->CreateWorker(client_fd);
  if (!worker.ok()) {
    return false;
  } else {
    workers_.push_back(worker.Get());
  }
  return true;
}

void Server::Stop() {
  if (sock_fd_ != -1) {
    close(sock_fd_);
  }
  sock_fd_ = -1;
  for (auto &worker : workers_) {
    worker->Stop();
  }
}

} // namespace sqpkv

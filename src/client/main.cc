#include "sqpkv/connection_factory.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <cstdio>

DEFINE_string(server_addr_file, "/tmp/sqpkv_proxy", "File containing the address of the proxy server");
DEFINE_string(server_addr, "127.0.0.1", "Address of the server");
DEFINE_int32(port, 4242, "Port number of the server");
DEFINE_bool(rdma, false, "Use RDMA connection for client and server");

void Show(std::string message) {
  std::cout << message << std::endl;
  std::cout << "> ";
}

void ShowStatus(sqpkv::Status &status) {
  if (status.err()) {
    Show(status.message());
  } else if (status.eof()) {
    std::cout << "Server is lost, exiting..." << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "> ";
  }
}

std::string FillCell(const std::string &item, size_t max_len) {
  std::string left_pad;
  std::string right_pad;
  size_t lpad = (max_len - item.length()) / 2;
  size_t rpad = max_len - lpad - item.length();
  left_pad.assign(lpad, ' ');
  right_pad.assign(rpad, ' ');

  std::stringstream ss;
  // ss << max_len << " " << lpad << " " << item.size() << " " << rpad << std::endl;
  ss << "│" << left_pad << item << right_pad << "│" << std::endl;
  return ss.str();
}

void ShowList(std::vector<std::string> list, std::string filename="") {
  std::ostream out_stream(nullptr);
  std::ofstream out_file;
  if (filename.length() > 0) {
    out_file.open(filename);
    out_stream.rdbuf(out_file.rdbuf());
  } else {
    out_stream.rdbuf(std::cout.rdbuf());
  }
  size_t max_len = 0;
  for (auto &item : list) {
    if (item.length() > max_len) {
      max_len = item.length();
    }
  }
  max_len += 2;
  out_stream << list.size() << " records found" << std::endl;
  std::stringstream ss;
  for (size_t i = 0; i < max_len; i++) {
    ss << "─";
  }
  auto hline = ss.str();
  out_stream << "┌" << hline << "┐" << std::endl;
  if (list.size() > 0) {
    out_stream << FillCell(list[0], max_len);
  }
  for (size_t i = 1; i < list.size(); i++) {
    out_stream << "├" << hline << "┤" << std::endl;
    out_stream << FillCell(list[i], max_len);
  }
  out_stream << "└" << hline << "┘" << std::endl;
  std::cout << "> ";
}

std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the string into a stream.
  std::string tok;
  
  while(std::getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  
  return std::move(internal);
}

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  spdlog::set_pattern("[%H:%M:%S] %v");
  spdlog::set_level(spdlog::level::info);
  auto console = spdlog::stdout_color_mt("console");
  std::string server_addr;
  std::ifstream addr_file(FLAGS_server_addr_file);
  if (!addr_file.fail()) {
    addr_file >> server_addr;
    addr_file.close();
  } else {
    server_addr = FLAGS_server_addr;
  }

  sqpkv::ConnectionFactory factory;
  sqpkv::StatusOr<sqpkv::Connection> connection;
  if (FLAGS_rdma) {
    connection = std::move(factory.CreateRdmaConnection(server_addr, FLAGS_port));
  } else {
    connection = std::move(factory.CreateSocketConnection(server_addr, FLAGS_port));
  }

  if (connection.err()) {
    spdlog::get("console")->error(connection.status().ToString());
    return 1;
  }

  Show("Welcome to SQPKV");
  bool quit = false;
  std::string line;
  while (!quit) {
    std::getline(std::cin, line);
    if (line.find("get all ") == 0) {
      std::string out_file;
      size_t into_pos = line.find("into ");
      if (into_pos != std::string::npos) {
        out_file = line.substr(into_pos + 5);
      }
      auto parts = split(line, ' ');
      std::vector<std::string> keys;
      auto status = connection->GetAll(parts[2], keys);
      if (status.ok()) {
        ShowList(keys, out_file);
      } else {
        ShowStatus(status);
      }
    } else if (line.find("get ") == 0) {
      auto parts = split(line, ' ');
      std::string value;
      auto status = connection->Get(parts[1], value);
      if (status.ok()) {
        Show(value);
      } else {
        ShowStatus(status);
      }
    } else if (line.find("put ") == 0) {
      auto parts = split(line, ' ');
      auto status = connection->Put(parts[1], parts[2]);
      ShowStatus(status);
    } else if (line.find("delete ") == 0) {
      auto parts = split(line, ' ');
      auto status = connection->Delete(parts[1]);
      ShowStatus(status);
    } else if (line.find("quit") == 0 || line.find("exit") || line.find("\\q")) {
      quit = true;
    } else {
      Show("Unsupported syntax");
    }
  }

  return 0;
}

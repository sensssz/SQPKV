#include "sqpkv/connection.h"

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

void ShowList(std::vector<std::string> list) {
  size_t max_len = 0;
  for (auto &item : list) {
    if (item.length() > max_len) {
      max_len = item.length();
    }
  }
  max_len += 2;
  std::stringstream ss;
  for (size_t i = 0; i < max_len; i++) {
    ss << "─";
  }
  auto hline = ss.str();
  std::cout << "┌" << hline << "┐" << std::endl;
  if (list.size() > 0) {
    std::string &item = list[0];
    int lpad = static_cast<int>((max_len - item.length()) / 2);
    int rpad = static_cast<int>(max_len - lpad - item.length() - 1);
    printf("│ %*s%*s │\n", lpad, item.c_str(), rpad, "");
  }
  for (size_t i = 1; i < list.size(); i++) {
    auto &item = list[i];
    std::cout << "├" << hline << "┤" << std::endl;
    int lpad = static_cast<int>((max_len - item.length()) / 2);
    int rpad = static_cast<int>(max_len - lpad - item.length() - 1);
    printf("│ %*s%*s │\n", lpad, item.c_str(), rpad, "");
  }
  std::cout << "└" << hline << "┘" << std::endl;
  std::cout << "> ";
}

// You could also take an existing vector as a parameter.
std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the string into a stream.
  std::string tok;
  
  while(std::getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  
  return internal;
}

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  spdlog::set_pattern("[%H:%M:%S] %v");
  spdlog::set_level(spdlog::level::debug);
  auto console = spdlog::stdout_color_mt("console");
  std::string server_addr;
  std::ifstream addr_file(FLAGS_server_addr_file);
  if (!addr_file.fail()) {
    addr_file >> server_addr;
    addr_file.close();
  } else {
    server_addr = FLAGS_server_addr;
  }
  auto connection = sqpkv::Connection::ConnectTo(server_addr, FLAGS_port);

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
      auto parts = split(line, ' ');
      std::vector<std::string> keys;
      auto status = connection->GetAll(parts[2], keys);
      if (status.ok()) {
        ShowList(keys);
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
    } else if (line.find("quit") == 0) {
      quit = true;
    } else {
      Show("Unsupported syntax");
    }
  }

  return 0;
}

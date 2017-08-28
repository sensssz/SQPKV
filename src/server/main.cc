#include "server_launcher.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <cerrno>
#include <cstring>
#include <csignal>
#include <cstdlib>

#include <execinfo.h>
#include <ucontext.h>
#include <unistd.h>

static void CrashHandler(int sig) {
  const int kMaxStackDepth = 50;
  void *array[kMaxStackDepth];
  size_t size = backtrace(array, kMaxStackDepth);

  // print out all the frames to stderr
  spdlog::get("console")->error("Signal {}:", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(EXIT_FAILURE);
}

static void InstallSignalHandler() {
  if (signal(SIGSEGV, CrashHandler) == SIG_ERR) {
    spdlog::get("console")->error("Error installing signal handler: {}", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  auto console = spdlog::stdout_color_mt("console");
  spdlog::set_pattern("[%H:%M:%S] %v");
  spdlog::set_level(spdlog::level::debug);

  InstallSignalHandler();

  sqpkv::ServerLauncher launcher;
  return launcher.Run();
}

#include "server_launcher.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <sstream>

#include <cerrno>
#include <cstring>
#include <csignal>
#include <cstdlib>

#include <execinfo.h> // for backtrace
#include <dlfcn.h>    // for dladdr
#include <cxxabi.h>   // for __cxa_demangle
#include <ucontext.h>
#include <unistd.h>

DEFINE_string(log_file, "/tmp/sqpkv.log", "Path of the log file.");

static std::string Backtrace(int skip = 1) {
  void *callstack[128];
  const int nMaxFrames = sizeof(callstack) / sizeof(callstack[0]);
  char buf[1024];
  int nFrames = backtrace(callstack, nMaxFrames);
  char **symbols = backtrace_symbols(callstack, nFrames);

  std::ostringstream trace_buf;
  for (int i = skip; i < nFrames; i++) {
    Dl_info info;
    if (dladdr(callstack[i], &info) && info.dli_sname) {
      char *demangled = NULL;
      int status = -1;
      if (info.dli_sname[0] == '_') {
        demangled = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
      }
      snprintf(buf, sizeof(buf), "%-3d %*p %s + %zd\n",
               i, int(2 + sizeof(void*) * 2), callstack[i],
               status == 0 ? demangled :
               info.dli_sname == 0 ? symbols[i] : info.dli_sname,
               (char *)callstack[i] - (char *)info.dli_saddr);
      free(demangled);
    } else {
      snprintf(buf, sizeof(buf), "%-3d %*p %s\n",
               i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
    }
    trace_buf << buf;
  }
  free(symbols);
  if (nFrames == nMaxFrames) {
    trace_buf << "[truncated]\n";
  }
  return trace_buf.str();
}

static void CrashHandler(int sig) {
  spdlog::get("console")->error("Signal {}\n{}", sig, Backtrace(0));
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

  auto console = spdlog::basic_logger_mt("console", "sqpkv.log");;
  spdlog::set_pattern("[%H:%M:%S] %v");
  auto log_level = spdlog::level::err;
  spdlog::set_level(log_level);
  console->flush_on(log_level);

  InstallSignalHandler();

  sqpkv::ServerLauncher launcher;
  return launcher.Run();
}

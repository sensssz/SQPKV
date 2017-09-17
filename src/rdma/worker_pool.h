#ifndef RDMA_WORKER_POOL_H_
#define RDMA_WORKER_POOL_H_

#include "context.h"
#include "request_handler.h"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

namespace sqpkv {

enum class WorkType {
  kRecv = 0,
  kSend = 1,
};

struct WorkUnit {
  WorkType work_type;
  Context *context;
  bool successful;
  RequestHandler *request_handler;
};

class WorkerPool {
public:
  static WorkerPool &GetInstance() {
    static WorkerPool worker_pool;
    return worker_pool;
  }

  WorkerPool();
  ~WorkerPool();
  void SubmitWorkUnit(WorkUnit &&work_unit);
  void Start();
  void Stop();

private:
  void ProcessNextWorkUnit();

  std::atomic<bool> stop_;
  std::atomic<int> num_workers_;
  std::vector<std::thread> workers_;

  std::mutex queue_mutex_;
  std::condition_variable cond_var_;
  std::deque<WorkUnit> work_queue_;
};

} // namespace sqpkv

#endif // RDMA_WORKER_POOL_H_
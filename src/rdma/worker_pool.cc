#include "sqpkv/worker_pool.h"

#include <vector>

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

DEFINE_int32(num_workers, 20, "Number of worker threads in the worker pool.");

namespace sqpkv {

WorkerPool::WorkerPool() : stop_(true), num_workers_(FLAGS_num_workers) {}

WorkerPool::WorkerPool(int num_workers) : stop_(false), num_workers_(num_workers) {}

WorkerPool::~WorkerPool() {
  if (!stop_) {
    Stop();
  }
}

void WorkerPool::SubmitWorkUnit(WorkUnit &&work_unit) {
  std::unique_lock<std::mutex> l(queue_mutex_);
  work_queue_.push_back(work_unit);
  if (work_queue_.size() == 1) {
    cond_var_.notify_one();
  }
}

void WorkerPool::Start() {
  if (!stop_) {
    return;
  }
  stop_ = false;
  for (int i = 0; i < num_workers_; i++) {
    workers_.push_back(std::thread(&WorkerPool::ProcessNextWorkUnit, this));
  }
}

void WorkerPool::Stop() {
  stop_ = true;
  cond_var_.notify_all();
  for (auto &worker : workers_) {
    worker.join();
  }
}

void WorkerPool::ProcessNextWorkUnit() {
  size_t num_work_units = 5;
  std::vector<WorkUnit> units;
  units.resize(num_work_units);
  while (!stop_.load()) {
    size_t num_units_to_process = 0;
    {
      std::unique_lock<std::mutex> l(queue_mutex_);
      cond_var_.wait(l, [this]() { return stop_.load() || !work_queue_.empty(); });
      if (stop_.load()) {
        break;
      }
      num_units_to_process = std::min(num_work_units, work_queue_.size());
      for (size_t i = 0; i < num_units_to_process; i++) {
        units[i] = work_queue_.front();
        work_queue_.pop_front();
      }
    }
    for (size_t i = 0; i < num_units_to_process; i++) {
      auto &work_unit = units[i];
      if (work_unit.work_type == WorkType::kRecv) {
        work_unit.request_handler->HandleRecvCompletion(work_unit.context, work_unit.successful);
      } else {
        work_unit.request_handler->HandleSendCompletion(work_unit.context, work_unit.successful);
      }
    }
  }
}

} // namespace sqpkv

#ifndef ROUTER_WORKER_H_
#define ROUTER_WORKER_H_

namespace sqpkv {

class Worker {
public:
  virtual ~Worker() {}
  virtual void Stop() = 0;
};

} // namespace sqpkv

#endif // ROUTER_WORKER_H_

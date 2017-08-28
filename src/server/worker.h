#ifndef SERVER_WORKER_H_
#define SERVER_WORKER_H_

namespace sqpkv {

class Worker {
public:
  virtual ~Worker() {}
  virtual void Stop() = 0;
};

} // namespace sqpkv

#endif // SERVER_WORKER_H_

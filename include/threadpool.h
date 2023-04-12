#pragma once

#include <functional>
#include <queue>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>


using Task = std::function<void()>;

class ThreadPool {

public:
  explicit ThreadPool(size_t);
  
  ~ThreadPool();

  void add_task(Task);

  void wait();

private:
  Task _get_task(std::stop_token&);

  class _Worker {
  
  public:
    explicit _Worker(ThreadPool *);

    void stop();

  private:
    void _run();

    ThreadPool * _pool;
    std::jthread _thread;
  };

  std::mutex _task_queue_mtx;
  std::condition_variable_any _new_task;
  std::condition_variable _finished;
  std::queue<Task> _task_queue;
  std::vector<_Worker> _workers;
};

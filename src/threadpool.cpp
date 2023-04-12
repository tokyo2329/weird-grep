#include "threadpool.h"


ThreadPool::ThreadPool(size_t thread_number) {
  _workers.reserve(thread_number);

  for(size_t i{0}; i < thread_number; i++)
    _workers.emplace_back(this);
}


ThreadPool::~ThreadPool() {
  for(auto& w : _workers)
    w.stop();
}


void ThreadPool::add_task(Task task) {
  {
    std::unique_lock lock{_task_queue_mtx};
    _task_queue.push(task);
  }

  _new_task.notify_one();
}


void ThreadPool::wait() {
  std::unique_lock lock{_task_queue_mtx};
  _finished.wait(lock, [this]{ return _task_queue.empty(); });
}


Task ThreadPool::_get_task(std::stop_token& stop_token) {
  Task todo;
  {
    std::unique_lock lock{_task_queue_mtx};
    _new_task.wait(lock, stop_token, [this]{
      return not _task_queue.empty();
    });

    if(stop_token.stop_requested())
      return {};
    
    todo = std::move(_task_queue.front());
    _task_queue.pop();

    if(_task_queue.empty())
      _finished.notify_all();
  }
  
  return todo;
}


ThreadPool::_Worker::_Worker(ThreadPool * pool)
  : _pool{pool}, _thread{&ThreadPool::_Worker::_run, this} {}


void ThreadPool::_Worker::stop() { _thread.request_stop(); }


void ThreadPool::_Worker::_run() {
  auto stop_token = _thread.get_stop_token();

  while(auto todo = _pool->_get_task(stop_token))
    todo();
}

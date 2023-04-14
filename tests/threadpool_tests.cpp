#include <thread>
#include <atomic>
#include <iostream>
#include "gtest/gtest.h"
#include "threadpool.h"


TEST(ThreadPoolTest, ZeroInit) {
  ThreadPool thread_pool{0};
}

TEST(ThreadPoolTest, SimpleInit) {
  ThreadPool thread_pool{1};
}

TEST(ThreadPoolTest, MaxInit) {
  ThreadPool thread_pool{std::thread::hardware_concurrency()};
}

TEST(ThreadPoolTest, SimpleTask) {
  int counter{0};

  {
    ThreadPool thread_pool{1};
    thread_pool.add_task([&]{ for(int i{0}; i < 100; i++) counter++; });

    thread_pool.wait();
  }

  ASSERT_EQ(counter, 100);
}

TEST(ThreadPoolTest, ManyTasks) {
  std::atomic<int> counter{0};

  {
    ThreadPool thread_pool{5};

    for(int i{0}; i < 20; i++)
      thread_pool.add_task([&]{
        for(int j{0}; j < 1000; j++) counter++;
      });

    thread_pool.wait();
  }

  ASSERT_EQ(counter, 20*1000);
}

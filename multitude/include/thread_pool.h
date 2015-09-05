#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool {
 public:
  ThreadPool(int numThreads) : running(true) {
    for (int i=0; i<numThreads; i++) {
      threads.push_back(std::thread([&] {
            while (true) {
              std::function<void ()> task;
              {
                std::unique_lock<std::mutex> lock(mutex);
                if (!running) {
                  return;
                } else if (workQueue.empty()) {
                  cond.wait(lock);
                  continue;
                } else {
                  task = workQueue.front();
                  workQueue.pop();
                }
              }
              try {
                task();
              } catch(...) {}
            }
          }));
    }
  }

  ~ThreadPool() {
    running = false;
    cond.notify_all();
    for (auto &thread : threads) {
      thread.join();
    }
  }

  template<typename T>
  std::future<T> schedule(std::function<T ()> producer) {
    auto promise = std::make_shared<std::promise<T>>();

    std::function<void ()> task = [=]() {
      try {
        T value = producer();
        promise->set_value(value);
      } catch (...) {
        promise->set_exception(std::current_exception());
      }
    };

    {
      std::unique_lock<std::mutex> lock(mutex);
      workQueue.push(task);
      cond.notify_one();
    }

    return promise->get_future();
  };

 private:
  ThreadPool(ThreadPool const&) = delete;
  void operator=(ThreadPool const&) = delete;

  bool running;
  std::mutex mutex;
  std::condition_variable cond;
  std::queue<std::function<void ()>> workQueue;
  std::vector<std::thread> threads;
};

#endif

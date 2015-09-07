#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

namespace Multitude {

/**
 * Task-based thread pool.
 *
 * Tasks submitted to the pool are executed as soon as a free
 * worker thread is available (FIFO).
 */
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

  /**
   * Schedule a task to run on the thread pool (FIFO).
   *
   * @param task - Task to execute on the thread pool.
   * @return - Future of task's result.
   */
  template<typename T>
  std::future<T> schedule(std::function<T ()> task) {
    auto promise = std::make_shared<std::promise<T>>();

    std::function<void ()> workerFn = [=]() {
      try {
        T value = task();
        promise->set_value(value);
      } catch (...) {
        promise->set_exception(std::current_exception());
      }
    };

    {
      std::unique_lock<std::mutex> lock(mutex);
      workQueue.push(workerFn);
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

}

#endif

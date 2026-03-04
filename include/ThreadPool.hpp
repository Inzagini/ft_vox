#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
class ThreadPool {

  public:
    ThreadPool(const uint8_t n) {
        running = true;
        for (uint8_t i = 0; i < n; i++) {
            workers.emplace_back([this] { this->workerLoop(); });
        }
    }

    ~ThreadPool() {
        running = false;
        cv.notify_all();
        for (auto &t : workers)
            t.join();
    }

    void enqueue(std::function<void()> job) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            jobs.push(job);
        }
        cv.notify_one();
    }

  private:
    void workerLoop() {
        while (running) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [&] { return !jobs.empty() || !running; });
                if (!running && jobs.empty())
                    return;
                job = jobs.front();
                jobs.pop();
            }
            job();
        }
    }

  private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobs;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> running;
};

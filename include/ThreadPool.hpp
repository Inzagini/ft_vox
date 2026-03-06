#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {

  public:
    ThreadPool(const uint8_t n) {
        running = true;
        for (size_t i = 0; i < n; i++) {
            workers.emplace_back([this] { this->workerLoop(); });
        }
    }

    ~ThreadPool() {
        std::unique_lock<std::mutex> lock(mutex);
        running = false;
        cv.notify_all();
        for (auto &t : workers)
            t.join();
    }

    void addTask(std::function<void()> job) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks.push(job);
        }
        cv.notify_one();
    }

  private:
    void workerLoop() {
        while (true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [&] { return !tasks.empty() || !running; });
                if (!running && tasks.empty())
                    return;
                job = std::move(tasks.front());
                tasks.pop();
            }
            job();
        }
    }

  private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> running;
};

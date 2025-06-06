#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

namespace TAGE::Threading {
    class ThreadPool {
    public:
        ThreadPool(size_t threadCount = std::thread::hardware_concurrency())
            : stop(false)
        {
            for (size_t i = 0; i < threadCount; ++i) {
                workers.emplace_back([this]() {
                    while (true) {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(queueMutex);
                            condition.wait(lock, [this]() {
                                return stop || !tasks.empty();
                                });

                            if (stop && tasks.empty()) return;

                            task = std::move(tasks.front());
                            tasks.pop();
                        }

                        task();
                    }
                    });
            }
        }

        template<typename F, typename... Args>
        auto enqueue(F&& func, Args&&... args)
            -> std::future<std::invoke_result_t<F, Args...>>
        {
            using ReturnType = std::invoke_result_t<F, Args...>;

            auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                std::bind(std::forward<F>(func), std::forward<Args>(args)...)
            );

            std::future<ReturnType> result = task->get_future();
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (stop)
                    throw std::runtime_error("ThreadPool has been stopped");

                tasks.emplace([task]() { (*task)(); });
            }

            condition.notify_one();
            return result;
        }

        ~ThreadPool() {
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                stop = true;
            }

            condition.notify_all();
            for (std::thread& worker : workers) {
                if (worker.joinable())
                    worker.join();
            }
        }

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> stop;
    };
}

#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace TAGE {
    class ThreadPool {
    public:
        ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
        ~ThreadPool();

        void EnqueueJob(std::function<void()> job);

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> jobs;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> stop;

        void WorkerThread();
    };

    class GrapichDispatcher {
    public:
        void Enqueue(std::function<void()> glCommand);
        void ExecutePending();

    private:
        std::queue<std::function<void()>> commandQueue;
        std::mutex queueMutex;
    };
}
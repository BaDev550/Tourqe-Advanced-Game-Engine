#include "tagepch.h"
#include "MultiThread.h"

namespace TAGE {
    ThreadPool::ThreadPool(size_t threadCount) : stop(false) {
        for (size_t i = 0; i < threadCount; ++i) {
            workers.emplace_back([this]() { WorkerThread(); });
        }
    }

    ThreadPool::~ThreadPool() {
        stop.store(true);
        condition.notify_all();
        for (auto& worker : workers) {
            if (worker.joinable())
                worker.join();
        }
    }

    void ThreadPool::EnqueueJob(std::function<void()> job) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            jobs.push(std::move(job));
        }
        condition.notify_one();
    }

    void ThreadPool::WorkerThread() {
        while (!stop.load()) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this]() { return !jobs.empty() || stop.load(); });
                if (stop.load() && jobs.empty())
                    return;
                job = std::move(jobs.front());
                jobs.pop();
            }
            job();
        }
    }

    void GrapichDispatcher::Enqueue(std::function<void()> glCommand) {
        std::lock_guard<std::mutex> lock(queueMutex);
        commandQueue.push(std::move(glCommand));
    }

    void GrapichDispatcher::ExecutePending() {
        std::queue<std::function<void()>> localQueue;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            std::swap(localQueue, commandQueue);
        }
        while (!localQueue.empty()) {
            localQueue.front()();
            localQueue.pop();
        }
    }
}
#include <RaeptorCogs/Worker.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <filesystem>
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace RaeptorCogs {

namespace Singletons {

#pragma region MainWorker

void MainWorker::addJob(const std::function<void()>& job, int priority) {
    #ifndef __EMSCRIPTEN__
    std::lock_guard<std::mutex> lock(mtx);
    #endif
    jobs[priority].push_back(job);
}

void MainWorker::addJob(const std::function<void()>& job, JobPriority priority) {
    addJob(job, static_cast<int>(priority));
}

void MainWorker::executeJobs() {
    #ifndef __EMSCRIPTEN__
    std::lock_guard<std::mutex> lock(mtx);
    #endif
    for (auto& [priority, jobList] : jobs) {
        for (auto& job : jobList) {
            job();
        }
    }
    jobs.clear();
}

void MainWorker::clearJobs() {
    jobs.clear();
}

#pragma endregion

}



#pragma region Worker

Worker::~Worker() {
    stop();
}

void Worker::start() {
    running = true;
    #ifndef __EMSCRIPTEN__
    workerThread = std::thread(&Worker::run, this);
    #else
    emscripten_async_call([](void* arg){
        Worker* self = static_cast<Worker*>(arg);
        self->run();
    }, this, 0); // 0 ms delay → run next tick
    #endif
}

void Worker::stop() {
    running = false;
    #ifndef __EMSCRIPTEN__
    if (workerThread.joinable()) {
        workerThread.join();
    }
    #endif
}

bool Worker::isRunning() const {
    return running;
}

#ifndef __EMSCRIPTEN__
void Worker::run() {
    while (running) {
        std::function<void()> job;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!jobs.empty()) {
                job = std::move(jobs.begin()->second.front());
                jobs.begin()->second.erase(jobs.begin()->second.begin());
                if (jobs.begin()->second.empty()) {
                    jobs.erase(jobs.begin());
                }
            }
        }
        if (job) {
            job();
        }
    }
}
#else
void Worker::run() {
        if (!running) return;

        // Process one job per async tick
        std::function<void()> job;

        // Find highest priority job
        if (!jobs.empty()) {
            job = std::move(jobs.begin()->second.front());
            jobs.begin()->second.erase(jobs.begin()->second.begin());
            if (jobs.begin()->second.empty()) {
                jobs.erase(jobs.begin());
            }
        }

        if (job) {
            job();
        }

        // Schedule next iteration
        emscripten_async_call([](void* arg){
            Worker* self = static_cast<Worker*>(arg);
            self->run();
        }, this, 0); // 0 ms delay → run next tick
}
#endif

void Worker::addJob(const std::function<void()>& job, int priority) {
    #ifndef __EMSCRIPTEN__
    std::lock_guard<std::mutex> lock(mtx);
    #endif
    jobs[priority].push_back(job);
    if (!running) {
        start();
    }
}

void Worker::addJob(const std::function<void()>& job, JobPriority priority) {
    addJob(job, static_cast<int>(priority));
}

void Worker::clearJobs() {
    #ifndef __EMSCRIPTEN__
    std::lock_guard<std::mutex> lock(mtx);
    #endif
    jobs.clear();
    stop();
}

#pragma endregion
}
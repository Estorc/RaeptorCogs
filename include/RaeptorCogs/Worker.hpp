/** ********************************************************************************
 * @section Worker_Overview Overview
 * @file Worker.hpp
 * @brief Worker thread management and job scheduling.
 * @details
 * Typical use cases:
 * - Managing worker threads for background tasks.
 * - Scheduling jobs with different priorities.
 * *********************************************************************************
 * @section Worker_Header Header
 * <RaeptorCogs/Worker.hpp>
 ***********************************************************************************
 * @section Worker_Metadata Metadata
 * @author Estorc
 * @version v1.0
 * @copyright Copyright (c) 2025 Estorc MIT License.
 **********************************************************************************/
/*                             This file is part of
 *                                  RaeptorCogs
 *                     (https://github.com/Estorc/RaeptorCogs)
 ***********************************************************************************
 * Copyright (c) 2025 Estorc.
 * This file is licensed under the MIT License.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ***********************************************************************************/

#pragma once
#include <RaeptorCogs/Singleton.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <map>
#include <limits>

namespace RaeptorCogs {

/**
 * @brief Enum class for job priority levels.
 * 
 * Used to specify the priority of jobs added to the worker.
 */
enum class JobPriority : int {
    /** Lowest priority level for jobs. */
    LOWEST = std::numeric_limits<int>::min(),
    /** Normal priority level for jobs. */
    NORMAL = 0,
    /** Highest priority level for jobs. */
    HIGHEST = std::numeric_limits<int>::max()
};


namespace Singletons {

    /**
     * @brief MainWorker singleton class.
     * 
     * Manages jobs that need to be executed on the main thread.
     */
    class MainWorker {
        private:

            // ============================================================================
            //                               PRIVATE ATTRIBUTES
            // ============================================================================

            #ifndef __EMSCRIPTEN__
            /**
             * @brief Mutex for synchronizing access to the job queue.
             * 
             * Ensures thread-safe operations when adding or executing jobs.
             */
            std::mutex mtx;
            #endif
            
            /**
             * @brief Map of job queues categorized by priority.
             * 
             * Each priority level maps to a vector of job functions.
             */
            std::map<int, std::vector<std::function<void()>>, std::greater<int>> jobs;

            /**
             * @brief Private constructor for the MainWorker singleton.
             * 
             * Ensures that the MainWorker can only be instantiated through the SingletonAccessor.
             */
            MainWorker() {};

            friend SingletonAccessor<MainWorker>;
        public:

            // ============================================================================
            //                               PUBLIC METHODS
            // ============================================================================

            /**
             * @brief Destructor for the MainWorker singleton.
             */
            ~MainWorker() = default;

            /**
             * @brief Add a job to the main worker.
             * 
             * @param job The job function to add.
             * @param priority The priority level of the job (default is NORMAL).
             * 
             * @note Jobs with higher priority values are executed first.
             */
            void addJob(const std::function<void()>& job, int priority);

            /**
             * @brief Overload of addJob to accept JobPriority enum.
             * 
             * @param job The job function to add.
             * @param priority The priority level of the job as JobPriority enum (default is NORMAL).
             * 
             * @note Jobs with higher priority values are executed first.
             */
            void addJob(const std::function<void()>& job, JobPriority priority = JobPriority::NORMAL);

            /**
             * @brief Execute all pending jobs in the main worker.
             * 
             * @note Jobs are executed in order of priority.
             */
            void executeJobs();

            /**
             * @brief Clear all pending jobs in the main worker.
             * 
             * @note This removes all jobs without executing them.
             */
            void clearJobs();
    };
}


/**
 * @brief Worker class for managing a background thread and job scheduling.
 * 
 * Provides functionality to start/stop a worker thread, add jobs with priorities,
 * and manage job execution.
 */
class Worker {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        #ifndef __EMSCRIPTEN__
        /**
         * @brief Thread object for the worker.
         * 
         * Manages the execution of jobs in a separate thread.
         */
        std::thread workerThread;

        /**
         * @brief Mutex for synchronizing access to the job queue.
         * 
         * Ensures thread-safe operations when adding or executing jobs.
         */
        std::mutex mtx;
        #endif

        /**
         * @brief Map of job queues categorized by priority.
         * 
         * Each priority level maps to a vector of job functions.
         */
        std::map<int, std::vector<std::function<void()>>, std::greater<int>> jobs;

        /**
         * @brief Main loop function for the worker thread.
         * 
         * Continuously executes jobs while the worker is running.
         */
        void run();

        /**
         * @brief Flag indicating whether the worker is running.
         * 
         * Used to control the main loop of the worker thread.
         */
        bool running = false;
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for Worker.
         */
        ~Worker();

        /**
         * @brief Start the worker thread.
         * 
         * Initializes and begins execution of the worker thread.
         */
        void start();

        /**
         * @brief Stop the worker thread.
         * 
         * Signals the worker thread to terminate and waits for it to finish.
         */
        void stop();

        /**
         * @brief Check if the worker thread is running.
         * 
         * @return true if the worker is running, false otherwise.
         */
        bool isRunning() const;

        /**
         * @brief Add a job to the worker.
         * 
         * @param job The job function to add.
         * @param priority The priority level of the job (default is NORMAL).
         * 
         * @note Jobs with higher priority values are executed first.
         */
        void addJob(const std::function<void()>& job, int priority);

        /**
         * @brief Overload of addJob to accept JobPriority enum.
         * 
         * @param job The job function to add.
         * @param priority The priority level of the job as JobPriority enum.
         * 
         * @note Jobs with higher priority values are executed first.
         */
        void addJob(const std::function<void()>& job, JobPriority priority = JobPriority::NORMAL);

        /**
         * @brief Clear all pending jobs in the worker.
         * 
         * @note This removes all jobs without executing them.
         */
        void clearJobs();
};

}
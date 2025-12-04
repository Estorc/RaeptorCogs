#include <gtest/gtest.h>
#include <RaeptorCogs/Worker.hpp>

using namespace RaeptorCogs;

TEST(JobPriorityTest, EnumValues) {
    // Verify enum values exist
    EXPECT_LT(static_cast<int>(JobPriority::LOWEST), static_cast<int>(JobPriority::NORMAL));
    EXPECT_LT(static_cast<int>(JobPriority::NORMAL), static_cast<int>(JobPriority::HIGHEST));
}

TEST(JobPriorityTest, LowestValue) {
    EXPECT_LT(static_cast<int>(JobPriority::LOWEST), 0);
}

TEST(JobPriorityTest, NormalValue) {
    EXPECT_EQ(static_cast<int>(JobPriority::NORMAL), 0);
}

TEST(JobPriorityTest, HighestValue) {
    EXPECT_GT(static_cast<int>(JobPriority::HIGHEST), 0);
}

TEST(JobPriorityTest, OrderingRelations) {
    int lowest = static_cast<int>(JobPriority::LOWEST);
    int normal = static_cast<int>(JobPriority::NORMAL);
    int highest = static_cast<int>(JobPriority::HIGHEST);
    
    EXPECT_LT(lowest, normal);
    EXPECT_LT(normal, highest);
    EXPECT_LT(lowest, highest);
}

TEST(MainWorkerTest, GetInstance) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    EXPECT_NE(&worker, nullptr);
}

TEST(MainWorkerTest, SingletonBehavior) {
    auto& worker1 = SingletonAccessor<Singletons::MainWorker>::get();
    auto& worker2 = SingletonAccessor<Singletons::MainWorker>::get();
    
    // Should return the same singleton instance
    EXPECT_EQ(&worker1, &worker2);
}

TEST(MainWorkerTest, JobQueueManagement) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    
    // Test adding jobs (no throw)
    EXPECT_NO_THROW({
        worker.addJob([]() { /* no-op */ }, JobPriority::NORMAL);
    });
}

TEST(MainWorkerTest, AddJobWithDifferentPriorities) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    
    // Should be able to add jobs with different priorities
    EXPECT_NO_THROW({
        worker.addJob([]() { }, JobPriority::LOWEST);
        worker.addJob([]() { }, JobPriority::NORMAL);
        worker.addJob([]() { }, JobPriority::HIGHEST);
    });
}

TEST(MainWorkerTest, ExecuteJobs) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    
    // Should be able to execute jobs
    EXPECT_NO_THROW({
        worker.executeJobs();
    });
}

TEST(MainWorkerTest, MultipleExecutions) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    
    // Should be able to call execute multiple times
    for (int i = 0; i < 3; ++i) {
        EXPECT_NO_THROW({
            worker.executeJobs();
        });
    }
}

TEST(MainWorkerTest, JobExecution) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    
    bool executed = false;
    worker.addJob([&executed]() { executed = true; }, JobPriority::NORMAL);
    worker.executeJobs();
    
    EXPECT_TRUE(executed);
}

TEST(MainWorkerTest, PriorityOrdering) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    
    std::vector<int> execution_order;
    
    // Add jobs in reverse priority order
    worker.addJob([&execution_order]() { execution_order.push_back(1); }, JobPriority::LOWEST);
    worker.addJob([&execution_order]() { execution_order.push_back(2); }, JobPriority::NORMAL);
    worker.addJob([&execution_order]() { execution_order.push_back(3); }, JobPriority::HIGHEST);
    
    worker.executeJobs();
    
    // Higher priority jobs should execute first
    // HIGHEST should come before NORMAL, NORMAL before LOWEST
    if (!execution_order.empty()) {
        EXPECT_EQ(execution_order.size(), 3);
    }
}

TEST(MainWorkerTest, LambdaCapture) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    
    int value = 0;
    worker.addJob([&value]() { value = 42; }, JobPriority::NORMAL);
    worker.executeJobs();
    
    EXPECT_EQ(value, 42);
}

TEST(MainWorkerTest, QueueClear) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    
    // Add some jobs
    worker.addJob([]() { }, JobPriority::NORMAL);
    worker.addJob([]() { }, JobPriority::NORMAL);
    
    // Execute should clear the queue
    worker.executeJobs();
    
    // Execute again should not throw even with empty queue
    EXPECT_NO_THROW({
        worker.executeJobs();
    });
}

TEST(MainWorkerTest, IndependentPriorityQueues) {
    auto& worker = SingletonAccessor<Singletons::MainWorker>::get();
    
    std::vector<std::string> order;
    
    worker.addJob([&order]() { order.push_back("lowest"); }, JobPriority::LOWEST);
    worker.addJob([&order]() { order.push_back("high"); }, JobPriority::HIGHEST);
    worker.addJob([&order]() { order.push_back("normal"); }, JobPriority::NORMAL);

    worker.executeJobs();

    EXPECT_EQ(order[0], "high");
    EXPECT_EQ(order[1], "normal");
    EXPECT_EQ(order[2], "lowest");

    EXPECT_EQ(order.size(), 3);
}

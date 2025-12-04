#include <gtest/gtest.h>
#include <RaeptorCogs/Singleton.hpp>

using namespace RaeptorCogs;

// Test singleton class
class TestSingleton {
private:
    int value_ = 0;
    TestSingleton() = default;
    
    friend SingletonAccessor<TestSingleton>;
    
public:
    void setValue(int val) { value_ = val; }
    int getValue() const { return value_; }
    
    // Prevent copying
    TestSingleton(const TestSingleton&) = delete;
    TestSingleton& operator=(const TestSingleton&) = delete;
};

// Another test singleton
class AnotherSingleton {
private:
    std::string name_;
    AnotherSingleton() : name_("default") {}
    
    friend SingletonAccessor<AnotherSingleton>;
    
public:
    void setName(const std::string& name) { name_ = name; }
    std::string getName() const { return name_; }
};

TEST(SingletonTest, GetInstance) {
    auto& instance = SingletonAccessor<TestSingleton>::get();
    EXPECT_EQ(instance.getValue(), 0);
}

TEST(SingletonTest, SameInstanceReturned) {
    auto& instance1 = SingletonAccessor<TestSingleton>::get();
    auto& instance2 = SingletonAccessor<TestSingleton>::get();
    
    // Should be the same object
    EXPECT_EQ(&instance1, &instance2);
}

TEST(SingletonTest, StatePreserved) {
    auto& instance1 = SingletonAccessor<TestSingleton>::get();
    instance1.setValue(42);
    
    auto& instance2 = SingletonAccessor<TestSingleton>::get();
    EXPECT_EQ(instance2.getValue(), 42);
}

TEST(SingletonTest, ModificationsPersist) {
    auto& instance = SingletonAccessor<TestSingleton>::get();
    instance.setValue(100);
    
    EXPECT_EQ(instance.getValue(), 100);
    
    instance.setValue(200);
    EXPECT_EQ(instance.getValue(), 200);
    
    // Get again and verify
    auto& sameInstance = SingletonAccessor<TestSingleton>::get();
    EXPECT_EQ(sameInstance.getValue(), 200);
}

TEST(SingletonTest, DifferentSingletonsAreIndependent) {
    auto& singleton1 = SingletonAccessor<TestSingleton>::get();
    auto& singleton2 = SingletonAccessor<AnotherSingleton>::get();
    
    singleton1.setValue(42);
    singleton2.setName("test");
    
    EXPECT_EQ(singleton1.getValue(), 42);
    EXPECT_EQ(singleton2.getName(), "test");
    
    // They should be different objects
    EXPECT_NE(static_cast<void*>(&singleton1), static_cast<void*>(&singleton2));
}

TEST(SingletonTest, DefaultInitialization) {
    auto& instance = SingletonAccessor<AnotherSingleton>::get();
    EXPECT_EQ(instance.getName(), "default");
}

TEST(SingletonTest, MultipleAccesses) {
    auto& instance = SingletonAccessor<AnotherSingleton>::get();
    
    for (int i = 0; i < 10; ++i) {
        auto& ref = SingletonAccessor<AnotherSingleton>::get();
        EXPECT_EQ(&ref, &instance);
    }
}

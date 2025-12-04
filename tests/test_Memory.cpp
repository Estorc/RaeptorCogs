#include <gtest/gtest.h>
#include <RaeptorCogs/Memory.hpp>
#include <sstream>

using namespace RaeptorCogs;

// Test resource class
class TestResource {
public:
    int value;
    std::string name;
    
    TestResource(int v, const std::string& n) : value(v), name(n) {}
};

TEST(UniqueKeyTest, CreateFromIntegers) {
    UniqueKey key(1, 2, 3);
    EXPECT_EQ(key.value, "123");
}

TEST(UniqueKeyTest, CreateFromStrings) {
    UniqueKey key("hello", "world");
    EXPECT_EQ(key.value, "helloworld");
}

TEST(UniqueKeyTest, CreateFromMixed) {
    UniqueKey key("prefix_", 42, "_suffix");
    EXPECT_EQ(key.value, "prefix_42_suffix");
}

TEST(UniqueKeyTest, CreateFromFloats) {
    UniqueKey key(3.14f, 2.71f);
    EXPECT_TRUE(key.value.find("3.14") != std::string::npos);
    EXPECT_TRUE(key.value.find("2.71") != std::string::npos);
}

TEST(UniqueKeyTest, EmptyKey) {
    UniqueKey key;
    EXPECT_EQ(key.value, "");
}

TEST(UniqueKeyTest, SingleElement) {
    UniqueKey key("single");
    EXPECT_EQ(key.value, "single");
}

TEST(UniqueKeyTest, NumericSequence) {
    UniqueKey key(1, 2, 3, 4, 5);
    EXPECT_EQ(key.value, "12345");
}

TEST(UniqueKeyTest, PathLike) {
    UniqueKey key("path", "/", "to", "/", "resource");
    EXPECT_EQ(key.value, "path/to/resource");
}

TEST(UniqueKeyTest, ComplexMixture) {
    UniqueKey key("tex_", 512, "x", 512, "_rgba");
    EXPECT_EQ(key.value, "tex_512x512_rgba");
}

TEST(ResourceManagerTest, CreateAndRetrieve) {
    auto& manager = SingletonAccessor<Singletons::ResourceManager<TestResource>>::get();
    manager.exists("test1") ? manager.get("test1") : manager.create(UniqueKey("test1"), 42, "resource1");
    
    TestResource& res = manager.get("test1");
    EXPECT_EQ(res.value, 42);
    EXPECT_EQ(res.name, "resource1");
}

TEST(ResourceManagerTest, ExistsCheckBeforeCreate) {
    auto& manager = SingletonAccessor<Singletons::ResourceManager<TestResource>>::get();
    
    EXPECT_FALSE(manager.exists("test1")); // Should not exist yet
}

TEST(ResourceManagerTest, CreateWithKey) {
    auto& manager = SingletonAccessor<Singletons::ResourceManager<TestResource>>::get();
    UniqueKey key("custom_key");
    
    TestResource& res = manager.create(key, 100, "custom");
    EXPECT_EQ(res.value, 100);
    EXPECT_EQ(res.name, "custom");
    
    EXPECT_TRUE(manager.exists("custom_key"));
}

TEST(ResourceManagerTest, GetNonExistentThrows) {
    auto& manager = SingletonAccessor<Singletons::ResourceManager<TestResource>>::get();
    
    EXPECT_THROW(manager.get("nonexistent_resource"), std::runtime_error);
}

TEST(ResourceManagerTest, MultipleResources) {
    auto& manager = SingletonAccessor<Singletons::ResourceManager<TestResource>>::get();
    
    if (!manager.exists("multi_1")) {
        manager.create(UniqueKey("multi_1"), 10, "first");
    }
    if (!manager.exists("multi_2")) {
        manager.create(UniqueKey("multi_2"), 20, "second");
    }
    if (!manager.exists("multi_3")) {
        manager.create(UniqueKey("multi_3"), 30, "third");
    }
    
    EXPECT_TRUE(manager.exists("multi_1"));
    EXPECT_TRUE(manager.exists("multi_2"));
    EXPECT_TRUE(manager.exists("multi_3"));
    
    EXPECT_EQ(manager.get("multi_1").value, 10);
    EXPECT_EQ(manager.get("multi_2").value, 20);
    EXPECT_EQ(manager.get("multi_3").value, 30);
}

TEST(ResourceManagerTest, NumericKey) {
    auto& manager = SingletonAccessor<Singletons::ResourceManager<TestResource>>::get();
    UniqueKey key(99, 88, 77);
    
    if (!manager.exists(key.value)) {
        manager.create(key, 77, "numeric");
    }
    
    EXPECT_TRUE(manager.exists(key.value));
    EXPECT_EQ(manager.get(key.value).value, 77);
}

TEST(ResourceManagerTest, SingletonBehavior) {
    auto& manager1 = SingletonAccessor<Singletons::ResourceManager<TestResource>>::get();
    auto& manager2 = SingletonAccessor<Singletons::ResourceManager<TestResource>>::get();
    
    EXPECT_EQ(&manager1, &manager2);
}

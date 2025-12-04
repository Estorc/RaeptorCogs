#include <gtest/gtest.h>
#include <RaeptorCogs/Serializable.hpp>
#include <sstream>

using namespace RaeptorCogs;

// Test serializable class
class TestSerializable : public Serializable {
public:
    int intValue = 42;
    float floatValue = 3.14f;
    std::string stringValue = "test";
    
    void reflect(Visitor& v) override {
        v.visit(intValue);
        v.visit(floatValue);
        v.visit(stringValue);
    }
};

TEST(SerializationFlagTest, EnumValues) {
    EXPECT_EQ(static_cast<uint8_t>(SerializationFlag::NONE), 0);
    EXPECT_EQ(static_cast<uint8_t>(SerializationFlag::COMPRESS), 1);
}

TEST(SerializationFlagTest, BitwiseOperations) {
    // Test flag operations work with enum
    SerializationFlag flag1 = SerializationFlag::NONE;
    SerializationFlag flag2 = SerializationFlag::COMPRESS;
    
    EXPECT_NE(flag1, flag2);
}

TEST(VisitorModeTest, EnumValues) {
    EXPECT_EQ(static_cast<uint8_t>(VisitorMode::SERIALIZE), 0);
    EXPECT_EQ(static_cast<uint8_t>(VisitorMode::DESERIALIZE), 1);
}

TEST(SerializableTest, DumpBasic) {
    TestSerializable obj;
    obj.intValue = 100;
    obj.floatValue = 2.5f;
    obj.stringValue = "hello";
    
    std::vector<uint8_t> data = obj.dump();
    
    // Serialized data should not be empty
    EXPECT_FALSE(data.empty());
}

TEST(SerializableTest, DumpAndLoad) {
    TestSerializable obj1;
    obj1.intValue = 999;
    obj1.floatValue = 1.23f;
    obj1.stringValue = "world";
    
    std::vector<uint8_t> data = obj1.dump();
    
    TestSerializable obj2;
    obj2.load(data);
    
    EXPECT_EQ(obj2.intValue, 999);
    EXPECT_FLOAT_EQ(obj2.floatValue, 1.23f);
    EXPECT_EQ(obj2.stringValue, "world");
}

TEST(SerializableTest, DumpWithNoneFlag) {
    TestSerializable obj;
    std::vector<uint8_t> data = obj.dump(SerializationFlag::NONE);
    
    EXPECT_FALSE(data.empty());
}

TEST(SerializableTest, MultipleSerializations) {
    TestSerializable obj;
    obj.intValue = 42;
    
    std::vector<uint8_t> data1 = obj.dump();
    std::vector<uint8_t> data2 = obj.dump();
    
    // Same object should produce same serialization
    EXPECT_EQ(data1.size(), data2.size());
    EXPECT_EQ(data1, data2);
}

TEST(SerializableTest, ModifyAndReserialize) {
    TestSerializable obj;
    obj.intValue = 10;
    
    std::vector<uint8_t> data1 = obj.dump();
    
    obj.intValue = 20;
    std::vector<uint8_t> data2 = obj.dump();
    
    // Different values should produce different serialization
    EXPECT_NE(data1, data2);
}

TEST(SerializableTest, LoadEmptyData) {
    TestSerializable obj;
    std::vector<uint8_t> empty_data;
    
    // Loading empty data should not crash
    EXPECT_NO_THROW(obj.load(empty_data));
}

TEST(SerializableTest, RoundTripPreservesValues) {
    TestSerializable original;
    original.intValue = 12345;
    original.floatValue = 67.89f;
    original.stringValue = "round trip test";
    
    std::vector<uint8_t> serialized = original.dump();
    
    TestSerializable restored;
    restored.load(serialized);
    
    EXPECT_EQ(restored.intValue, original.intValue);
    EXPECT_FLOAT_EQ(restored.floatValue, original.floatValue);
    EXPECT_EQ(restored.stringValue, original.stringValue);
}

TEST(SerializableTest, DefaultValues) {
    TestSerializable obj;
    
    EXPECT_EQ(obj.intValue, 42);
    EXPECT_FLOAT_EQ(obj.floatValue, 3.14f);
    EXPECT_EQ(obj.stringValue, "test");
}

TEST(SerializableTest, NegativeNumbers) {
    TestSerializable obj;
    obj.intValue = -999;
    obj.floatValue = -3.14f;
    
    std::vector<uint8_t> data = obj.dump();
    
    TestSerializable restored;
    restored.load(data);
    
    EXPECT_EQ(restored.intValue, -999);
    EXPECT_FLOAT_EQ(restored.floatValue, -3.14f);
}

TEST(SerializableTest, EmptyString) {
    TestSerializable obj;
    obj.stringValue = "";
    
    std::vector<uint8_t> data = obj.dump();
    
    TestSerializable restored;
    restored.load(data);
    
    EXPECT_EQ(restored.stringValue, "");
}

TEST(SerializableTest, LongString) {
    TestSerializable obj;
    obj.stringValue = std::string(1000, 'x');
    
    std::vector<uint8_t> data = obj.dump();
    
    TestSerializable restored;
    restored.load(data);
    
    EXPECT_EQ(restored.stringValue.length(), 1000);
    EXPECT_EQ(restored.stringValue, std::string(1000, 'x'));
}

TEST(SerializableTest, ZeroValues) {
    TestSerializable obj;
    obj.intValue = 0;
    obj.floatValue = 0.0f;
    obj.stringValue = "";
    
    std::vector<uint8_t> data = obj.dump();
    
    TestSerializable restored;
    restored.intValue = 999; // Set to different value first
    restored.load(data);
    
    EXPECT_EQ(restored.intValue, 0);
    EXPECT_FLOAT_EQ(restored.floatValue, 0.0f);
    EXPECT_EQ(restored.stringValue, "");
}

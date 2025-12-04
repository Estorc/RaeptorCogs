#include <gtest/gtest.h>
#include <RaeptorCogs/Graphic.hpp>

using namespace RaeptorCogs;

TEST(GraphicFlagsTest, EnumValues) {
    EXPECT_EQ(static_cast<uint32_t>(GraphicFlags::NONE), 0);
    EXPECT_EQ(static_cast<uint32_t>(GraphicFlags::DATA_DIRTY), 1);
    EXPECT_EQ(static_cast<uint32_t>(GraphicFlags::INHERIT_READ_MASK), 2);
    EXPECT_EQ(static_cast<uint32_t>(GraphicFlags::NO_BATCHING), 4);
}

TEST(GraphicFlagsTest, BitwiseOperations) {
    // Test bitwise OR
    GraphicFlags combined = GraphicFlags::DATA_DIRTY | GraphicFlags::NO_BATCHING;
    EXPECT_EQ(static_cast<uint32_t>(combined), 5); // 1 | 4 = 5
}

TEST(GraphicFlagsTest, AllFlags) {
    GraphicFlags all = GraphicFlags::DATA_DIRTY | 
                       GraphicFlags::INHERIT_READ_MASK | 
                       GraphicFlags::NO_BATCHING;
    EXPECT_EQ(static_cast<uint32_t>(all), 7); // 1 | 2 | 4 = 7
}

TEST(GraphicFlagsTest, NoneFlag) {
    GraphicFlags none = GraphicFlags::NONE;
    EXPECT_EQ(static_cast<uint32_t>(none), 0);
}

TEST(ComputeInstanceDataModeTest, EnumValues) {
    EXPECT_EQ(static_cast<int>(ComputeInstanceDataMode::NONE), 0);
    EXPECT_EQ(static_cast<int>(ComputeInstanceDataMode::FORCE_REBUILD), 1);
    EXPECT_EQ(static_cast<int>(ComputeInstanceDataMode::REBUILD_TEXTURE), 2);
}

TEST(ComputeInstanceDataModeTest, ValueOrdering) {
    EXPECT_LT(static_cast<int>(ComputeInstanceDataMode::NONE), 
              static_cast<int>(ComputeInstanceDataMode::FORCE_REBUILD));
    EXPECT_LT(static_cast<int>(ComputeInstanceDataMode::FORCE_REBUILD), 
              static_cast<int>(ComputeInstanceDataMode::REBUILD_TEXTURE));
}

TEST(ComputeInstanceDataModeTest, DistinctValues) {
    int none = static_cast<int>(ComputeInstanceDataMode::NONE);
    int rebuild = static_cast<int>(ComputeInstanceDataMode::FORCE_REBUILD);
    int texture = static_cast<int>(ComputeInstanceDataMode::REBUILD_TEXTURE);
    
    EXPECT_NE(none, rebuild);
    EXPECT_NE(rebuild, texture);
    EXPECT_NE(none, texture);
}

// Test FlagSet operations with GraphicFlags
TEST(GraphicFlagSetTest, SetAndCheck) {
    FlagSet<GraphicFlags> flags;
    
    flags.setFlag(GraphicFlags::DATA_DIRTY);
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::DATA_DIRTY));
}

TEST(GraphicFlagSetTest, SetMultipleFlags) {
    FlagSet<GraphicFlags> flags;
    
    flags.setFlag(GraphicFlags::DATA_DIRTY);
    flags.setFlag(GraphicFlags::NO_BATCHING);
    
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::DATA_DIRTY));
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::NO_BATCHING));
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::INHERIT_READ_MASK));
}

TEST(GraphicFlagSetTest, ClearFlag) {
    FlagSet<GraphicFlags> flags;
    
    flags.setFlag(GraphicFlags::DATA_DIRTY);
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::DATA_DIRTY));
    
    flags.clearFlag(GraphicFlags::DATA_DIRTY);
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::DATA_DIRTY));
}

TEST(GraphicFlagSetTest, ToggleFlag) {
    FlagSet<GraphicFlags> flags;
    
    flags.toggleFlag(GraphicFlags::NO_BATCHING);
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::NO_BATCHING));
    
    flags.toggleFlag(GraphicFlags::NO_BATCHING);
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::NO_BATCHING));
}

TEST(GraphicFlagSetTest, SetCombinedFlags) {
    FlagSet<GraphicFlags> flags;
    
    GraphicFlags combined = GraphicFlags::DATA_DIRTY | GraphicFlags::INHERIT_READ_MASK;
    flags.setFlag(combined);
    
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::DATA_DIRTY));
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::INHERIT_READ_MASK));
}

TEST(GraphicFlagSetTest, ClearAll) {
    FlagSet<GraphicFlags> flags;
    
    flags.setFlag(GraphicFlags::DATA_DIRTY);
    flags.setFlag(GraphicFlags::NO_BATCHING);
    flags.setFlag(GraphicFlags::INHERIT_READ_MASK);
    
    flags.clearFlag();
    
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::DATA_DIRTY));
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::NO_BATCHING));
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::INHERIT_READ_MASK));
}

TEST(GraphicFlagSetTest, InitialState) {
    FlagSet<GraphicFlags> flags;
    
    // Initially, no flags should be set
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::DATA_DIRTY));
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::NO_BATCHING));
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::INHERIT_READ_MASK));
}

TEST(GraphicFlagSetTest, SetAndClearMultiple) {
    FlagSet<GraphicFlags> flags;
    
    flags.setFlag(GraphicFlags::DATA_DIRTY);
    flags.setFlag(GraphicFlags::NO_BATCHING);
    
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::DATA_DIRTY));
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::NO_BATCHING));
    
    flags.clearFlag(GraphicFlags::DATA_DIRTY);
    
    EXPECT_FALSE(flags.hasFlag(GraphicFlags::DATA_DIRTY));
    EXPECT_TRUE(flags.hasFlag(GraphicFlags::NO_BATCHING));
}

TEST(GraphicFlagSetTest, IndependentFlagSets) {
    FlagSet<GraphicFlags> flags1;
    FlagSet<GraphicFlags> flags2;
    
    flags1.setFlag(GraphicFlags::DATA_DIRTY);
    flags2.setFlag(GraphicFlags::NO_BATCHING);
    
    EXPECT_TRUE(flags1.hasFlag(GraphicFlags::DATA_DIRTY));
    EXPECT_FALSE(flags1.hasFlag(GraphicFlags::NO_BATCHING));
    
    EXPECT_FALSE(flags2.hasFlag(GraphicFlags::DATA_DIRTY));
    EXPECT_TRUE(flags2.hasFlag(GraphicFlags::NO_BATCHING));
}

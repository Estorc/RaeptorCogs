#include <gtest/gtest.h>
#include <RaeptorCogs/Region.hpp>

using namespace RaeptorCogs;

TEST(RegionBufferTest, DefaultConstruction) {
    RegionBuffer buffer;
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0);
}

TEST(RegionBufferTest, PushSingleRegion) {
    RegionBuffer buffer;
    buffer.push(0, 512);
    
    EXPECT_FALSE(buffer.empty());
    EXPECT_EQ(buffer.size(), 1);
    
    Region region = buffer.get();
    EXPECT_EQ(region.first, 0);
    EXPECT_EQ(region.second, 512);
}

TEST(RegionBufferTest, PushMultipleNonOverlapping) {
    RegionBuffer buffer;
    buffer.push(0, 512);
    buffer.push(1024, 2048);
    buffer.push(3072, 4096);
    
    EXPECT_EQ(buffer.size(), 3);
}

TEST(RegionBufferTest, PushSwapsBeginEnd) {
    RegionBuffer buffer;
    buffer.push(512, 0); // End before begin - should swap
    
    Region region = buffer.get();
    EXPECT_EQ(region.first, 0);
    EXPECT_EQ(region.second, 512);
}

TEST(RegionBufferTest, MergeAdjacentRegions) {
    RegionBuffer buffer;
    buffer.push(0, 512);
    buffer.push(512, 1024); // Adjacent to previous
    
    EXPECT_EQ(buffer.size(), 1); // Should merge
    Region region = buffer.get();
    EXPECT_EQ(region.first, 0);
    EXPECT_EQ(region.second, 1024);
}

TEST(RegionBufferTest, MergeOverlappingRegions) {
    RegionBuffer buffer;
    buffer.push(0, 600);
    buffer.push(500, 1024); // Overlaps previous
    
    EXPECT_EQ(buffer.size(), 1); // Should merge
    Region region = buffer.get();
    EXPECT_EQ(region.first, 0);
    EXPECT_EQ(region.second, 1024);
}

TEST(RegionBufferTest, MergeMultipleRegions) {
    RegionBuffer buffer;
    buffer.push(0, 100);
    buffer.push(200, 300);
    buffer.push(400, 500);
    buffer.push(50, 450); // Should merge all
    
    EXPECT_EQ(buffer.size(), 1);
    Region region = buffer.get();
    EXPECT_EQ(region.first, 0);
    EXPECT_EQ(region.second, 500);
}

TEST(RegionBufferTest, Clear) {
    RegionBuffer buffer;
    buffer.push(0, 512);
    buffer.push(1024, 2048);
    
    EXPECT_EQ(buffer.size(), 2);
    
    buffer.clear();
    
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0);
}

TEST(RegionBufferTest, EraseEntireRegion) {
    RegionBuffer buffer;
    buffer.push(0, 512);
    buffer.push(1024, 2048);
    
    buffer.erase(0, 512);
    
    EXPECT_EQ(buffer.size(), 1);
}

TEST(RegionBufferTest, ErasePartialStart) {
    RegionBuffer buffer;
    buffer.push(0, 1024);
    
    buffer.erase(0, 512);
    
    EXPECT_EQ(buffer.size(), 1);
    Region region = buffer.get();
    EXPECT_EQ(region.first, 512);
    EXPECT_EQ(region.second, 1024);
}

TEST(RegionBufferTest, ErasePartialEnd) {
    RegionBuffer buffer;
    buffer.push(0, 1024);
    
    buffer.erase(512, 1024);
    
    EXPECT_EQ(buffer.size(), 1);
    Region region = buffer.get();
    EXPECT_EQ(region.first, 0);
    EXPECT_EQ(region.second, 512);
}

TEST(RegionBufferTest, EraseSplitRegion) {
    RegionBuffer buffer;
    buffer.push(0, 1024);
    
    buffer.erase(256, 768); // Split in middle
    
    EXPECT_EQ(buffer.size(), 2);
    
    // Check both resulting regions
    auto it = buffer.begin();
    EXPECT_EQ(it->first, 0);
    EXPECT_EQ(it->second, 256);
    
    ++it;
    EXPECT_EQ(it->first, 768);
    EXPECT_EQ(it->second, 1024);
}

TEST(RegionBufferTest, EraseSwapsBeginEnd) {
    RegionBuffer buffer;
    buffer.push(0, 1024);
    
    buffer.erase(512, 256); // End before begin - should swap
    
    EXPECT_EQ(buffer.size(), 2);
}

TEST(RegionBufferTest, EraseMultipleRegions) {
    RegionBuffer buffer;
    buffer.push(0, 100);
    buffer.push(200, 300);
    buffer.push(400, 500);
    
    buffer.erase(50, 450); // Should affect all three
    
    EXPECT_EQ(buffer.size(), 2);
    
    auto it = buffer.begin();
    EXPECT_EQ(it->first, 0);
    EXPECT_EQ(it->second, 50);
    
    ++it;
    EXPECT_EQ(it->first, 450);
    EXPECT_EQ(it->second, 500);
}

TEST(RegionBufferTest, GetThrowsWhenEmpty) {
    RegionBuffer buffer;
    EXPECT_THROW(buffer.get(), std::runtime_error);
}

TEST(RegionBufferTest, IteratorSupport) {
    RegionBuffer buffer;
    buffer.push(0, 100);
    buffer.push(200, 300);
    buffer.push(400, 500);
    
    size_t count = 0;
    for (const auto& region : buffer) {
        EXPECT_GT(region.second, region.first);
        ++count;
    }
    
    EXPECT_EQ(count, 3);
}

TEST(RegionBufferTest, ComplexScenario) {
    RegionBuffer buffer;
    
    // Build up regions
    buffer.push(0, 100);
    buffer.push(200, 300);
    buffer.push(400, 500);
    buffer.push(600, 700);
    
    EXPECT_EQ(buffer.size(), 4);
    
    // Merge some
    buffer.push(100, 200);
    EXPECT_EQ(buffer.size(), 3);
    
    // Erase middle section
    buffer.erase(250, 450);
    EXPECT_EQ(buffer.size(), 3);
    
    // Verify regions
    auto it = buffer.begin();
    EXPECT_EQ(it->first, 0);
    EXPECT_EQ(it->second, 250);
    
    ++it;
    EXPECT_EQ(it->first, 450);
    EXPECT_EQ(it->second, 500);
    
    ++it;
    EXPECT_EQ(it->first, 600);
    EXPECT_EQ(it->second, 700);
}

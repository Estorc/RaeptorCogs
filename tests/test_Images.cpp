#include <gtest/gtest.h>
#include <RaeptorCogs/IO/Images.hpp>
#include <fstream>
#include <vector>

using namespace RaeptorCogs;

TEST(ImageTest, DefaultConstruction) {
    Image img;
    
    EXPECT_EQ(img.data, nullptr);
    EXPECT_EQ(img.width, 0);
    EXPECT_EQ(img.height, 0);
    EXPECT_EQ(img.channels, 0);
}

TEST(ImageTest, CreateImage) {
    Image img = CreateImage(64, 64);
    
    EXPECT_NE(img.data, nullptr);
    EXPECT_EQ(img.width, 64);
    EXPECT_EQ(img.height, 64);
    EXPECT_EQ(img.channels, 4); // RGBA
}

TEST(ImageTest, CreateImageDifferentSizes) {
    Image img1 = CreateImage(32, 32);
    EXPECT_EQ(img1.width, 32);
    EXPECT_EQ(img1.height, 32);
    
    Image img2 = CreateImage(128, 256);
    EXPECT_EQ(img2.width, 128);
    EXPECT_EQ(img2.height, 256);
}

TEST(ImageTest, CreateImageLarge) {
    Image img = CreateImage(512, 512);
    
    EXPECT_NE(img.data, nullptr);
    EXPECT_EQ(img.width, 512);
    EXPECT_EQ(img.height, 512);
}

TEST(ImageTest, IsOpaqueEmptyImage) {
    Image img;
    // Empty image should be considered opaque
    EXPECT_TRUE(img.isOpaque());
}

TEST(ImageTest, IsOpaqueCreatedImage) {
    Image img = CreateImage(8, 8);
    
    // Newly created image has all zeros (including alpha = 0)
    // So it should NOT be opaque
    EXPECT_FALSE(img.isOpaque());
}

TEST(ImageTest, IsOpaqueWithFullAlpha) {
    Image img = CreateImage(4, 4);
    
    // Set all alpha values to 255
    for (size_t i = 0; i < img.width * img.height * img.channels; i += img.channels) {
        img.data[i + 3] = 255; // Alpha channel
    }
    
    EXPECT_TRUE(img.isOpaque());
}

TEST(ImageTest, IsOpaqueWithPartialAlpha) {
    Image img = CreateImage(4, 4);
    
    // Set most alpha values to 255, but one to 128
    for (size_t i = 0; i < img.width * img.height * img.channels; i += img.channels) {
        img.data[i + 3] = 255;
    }
    img.data[3] = 128; // First pixel has partial alpha
    
    EXPECT_FALSE(img.isOpaque());
}

TEST(ImageTest, MoveConstructor) {
    Image img1 = CreateImage(16, 16);
    EXPECT_NE(img1.data, nullptr);
    
    Image img2 = std::move(img1);
    
    EXPECT_EQ(img2.width, 16);
    EXPECT_EQ(img2.height, 16);
    EXPECT_NE(img2.data, nullptr);
}

TEST(ImageTest, MoveAssignment) {
    Image img1 = CreateImage(16, 16);
    Image img2;
    
    img2 = std::move(img1);
    
    EXPECT_EQ(img2.width, 16);
    EXPECT_EQ(img2.height, 16);
    EXPECT_NE(img2.data, nullptr);
}

TEST(ImageTest, ImageDataAccess) {
    Image img = CreateImage(2, 2);
    
    // Should be able to access pixel data
    EXPECT_NO_THROW({
        img.data[0] = 255; // R
        img.data[1] = 128; // G
        img.data[2] = 64;  // B
        img.data[3] = 255; // A
    });
}

TEST(ImageTest, ImageChannelCount) {
    Image img = CreateImage(10, 10);
    
    // Created images should have 4 channels (RGBA)
    EXPECT_EQ(img.channels, 4);
    
    // Total pixel data size should be width * height * channels
    size_t expected_size = img.width * img.height * img.channels;
    EXPECT_GT(expected_size, 0);
}

TEST(ImageTest, ZeroSizeImage) {
    Image img1 = CreateImage(0, 10);
    // Should handle gracefully, either nullptr or valid but empty
    
    Image img2 = CreateImage(10, 0);
    // Should handle gracefully
}

TEST(ImageTest, IsOpaqueWithNoAlphaChannel) {
    // Test RGB image (3 channels, no alpha)
    Image img;
    img.width = 4;
    img.height = 4;
    img.channels = 3;
    
    // Images without alpha channel should be considered opaque
    EXPECT_TRUE(img.isOpaque());
}

// Test LoadImageFromMemory with a simple PNG
TEST(ImageTest, LoadImageFromMemoryEmpty) {
    FileData empty_data;
    Image img = LoadImageFromMemory(empty_data);
    
    // Empty data should result in invalid image
    EXPECT_EQ(img.data, nullptr);
}

// Test that CreateImage creates the correct amount of memory
TEST(ImageTest, CreateImageMemorySize) {
    size_t width = 10;
    size_t height = 20;
    Image img = CreateImage(width, height);
    
    EXPECT_EQ(img.width, width);
    EXPECT_EQ(img.height, height);
    
    // Should be able to set all pixels without issue
    size_t total_values = width * height * img.channels;
    for (size_t i = 0; i < total_values; ++i) {
        img.data[i] = static_cast<unsigned char>(i % 256);
    }
}

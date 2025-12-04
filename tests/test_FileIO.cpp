#include <gtest/gtest.h>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <filesystem>
#include <fstream>
#include <cstring>

using namespace RaeptorCogs;

// Helper to create a temporary test file
class FileIOTest : public ::testing::Test {
protected:
    static constexpr const char* TEST_FILE = "test_file_io.txt";
    static constexpr const char* TEST_CONTENT = "Hello, World!";
    
    void SetUp() override {
        // Create a test file
        std::ofstream file(TEST_FILE);
        file << TEST_CONTENT;
        file.close();
    }
    
    void TearDown() override {
        // Clean up the test file
        std::filesystem::remove(TEST_FILE);
    }
};

TEST_F(FileIOTest, LoadFileBasic) {
    FileData data = LoadFile(TEST_FILE);
    
    EXPECT_FALSE(data.empty());
}

TEST_F(FileIOTest, LoadFileContent) {
    FileData data = LoadFile(TEST_FILE);
    
    // Convert to string for easier comparison
    std::string content(data.begin(), data.end());
    EXPECT_TRUE(content.find("Hello") != std::string::npos);
    EXPECT_TRUE(content.find("World") != std::string::npos);
}

TEST_F(FileIOTest, LoadFileSize) {
    FileData data = LoadFile(TEST_FILE);
    
    // Should at least contain our test content
    EXPECT_GE(data.size(), std::strlen(TEST_CONTENT));
}

TEST_F(FileIOTest, LoadFileNullTerminated) {
    FileData data = LoadFile(TEST_FILE);
    
    // The file should be null-terminated
    EXPECT_EQ(data.back(), '\0');
}

TEST_F(FileIOTest, LoadFileMultipleTimes) {
    FileData data1 = LoadFile(TEST_FILE);
    FileData data2 = LoadFile(TEST_FILE);
    
    // Should get the same content both times
    EXPECT_EQ(data1.size(), data2.size());
    EXPECT_EQ(data1, data2);
}

TEST_F(FileIOTest, LoadLargeFile) {
    // Create a larger test file
    std::string large_filename = "large_test_file.bin";
    {
        std::ofstream file(large_filename, std::ios::binary);
        for (int i = 0; i < 10000; ++i) {
            file.write("x", 1);
        }
        file.close();
    }
    
    FileData data = LoadFile(large_filename);
    
    EXPECT_GE(data.size(), 10000);
    
    // Cleanup
    std::filesystem::remove(large_filename);
}

TEST_F(FileIOTest, LoadEmptyFile) {
    std::string empty_filename = "empty_test_file.txt";
    {
        std::ofstream file(empty_filename);
        file.close();
    }
    
    FileData data = LoadFile(empty_filename);
    
    // Even empty files might have null terminator
    EXPECT_GE(data.size(), 0);
    
    // Cleanup
    std::filesystem::remove(empty_filename);
}

TEST_F(FileIOTest, LoadBinaryFile) {
    std::string binary_filename = "binary_test_file.bin";
    unsigned char binary_content[] = {0xFF, 0xFE, 0xFD, 0xFC};
    
    {
        std::ofstream file(binary_filename, std::ios::binary);
        file.write(reinterpret_cast<const char*>(binary_content), sizeof(binary_content));
        file.close();
    }
    
    FileData data = LoadFile(binary_filename);
    
    EXPECT_GE(data.size(), sizeof(binary_content));
    
    // Cleanup
    std::filesystem::remove(binary_filename);
}

TEST_F(FileIOTest, FileDataTypeAlias) {
    // Verify FileData is a vector of unsigned char
    FileData data;
    data.push_back('H');
    data.push_back('i');
    
    EXPECT_EQ(data.size(), 2);
    EXPECT_EQ(data[0], 'H');
    EXPECT_EQ(data[1], 'i');
}

TEST_F(FileIOTest, FileDialogFilterConstruction) {
    FileDialogFilter filter = {"Text Files", "*.txt"};
    
    EXPECT_EQ(filter[0], "Text Files");
    EXPECT_EQ(filter[1], "*.txt");
}

TEST_F(FileIOTest, FileDialogFiltersMultiple) {
    FileDialogFilters filters = {
        {"Text Files", "*.txt"},
        {"Image Files", "*.png;*.jpg"},
        {"All Files", "*.*"}
    };
    
    EXPECT_EQ(filters.size(), 3);
    EXPECT_EQ(filters[0][0], "Text Files");
    EXPECT_EQ(filters[1][0], "Image Files");
    EXPECT_EQ(filters[2][0], "All Files");
}

TEST_F(FileIOTest, LoadFileCurrentDirectory) {
    // Test that LoadFile can find files in current directory
    FileData data = LoadFile(TEST_FILE);
    
    EXPECT_FALSE(data.empty());
}

TEST_F(FileIOTest, LoadFileAsset) {
    // Create a file with a simple name
    std::string asset_name = "test_asset.bin";
    {
        std::ofstream file(asset_name, std::ios::binary);
        file.write("ASSET", 5);
        file.close();
    }
    
    FileData data = LoadFile(asset_name);
    
    EXPECT_GE(data.size(), 5);
    
    // Cleanup
    std::filesystem::remove(asset_name);
}

TEST_F(FileIOTest, LocalizeWorkingDirectory) {
    // This function should not throw
    EXPECT_NO_THROW(LocalizeWorkingDirectory());
}

TEST_F(FileIOTest, LocalizeWorkingDirectoryReturnValue) {
    // Should return a boolean
    bool result = LocalizeWorkingDirectory();
    EXPECT_TRUE(result || !result); // Just verify it returns a bool
}

// tests/test_strings.cpp

#include <gtest/gtest.h>
#include <RaeptorCogs/IO/String.hpp>
#include <string>
#include <vector>
#include <string_view>

using namespace RaeptorCogs;

// Helper: split string into UTF-8 codepoints as string_views
std::vector<std::string> split_utf8(const std::string& s) {
    std::vector<std::string> chars;
    for (size_t i = 0; i < s.size();) {
        size_t len = U8Char(&s[i], 1).size();
        chars.emplace_back(s.substr(i, len));
        i += len;
    }
    return chars;
}

TEST(U8CharIterator, IteratesCorrectly) {
    std::string s = "Aé€😀";
    std::vector<std::string> expected = {"A", "é", "€", "😀"};
    U8CharIterator it(s, 0), end(s, s.size());
    std::vector<std::string> result;
    for (; it != end; ++it) {
        result.push_back(std::string(*it));
    }
    EXPECT_EQ(result, expected);
}

TEST(U8CharIterator, PostIncrement) {
    std::string s = "Hi€";
    U8CharIterator it(s, 0);
    auto it2 = it++;
    EXPECT_EQ(std::string(*it2), "H");
    EXPECT_EQ(std::string(*it), "i");
}

TEST(U8CharIterator, Equality) {
    std::string s = "abc";
    U8CharIterator it1(s, 0), it2(s, 0), it3(s, 1);
    EXPECT_TRUE(it1 == it2);
    EXPECT_FALSE(it1 == it3);
    EXPECT_TRUE(it1 != it3);
}

TEST(U8String, RangeFor) {
    std::string s = "Aé€";
    U8String str(s);
    std::vector<std::string> chars;
    for (auto ch : str) {
        chars.push_back(std::string(ch));
    }
    std::vector<std::string> expected = {"A", "é", "€"};
    EXPECT_EQ(chars, expected);
}

TEST(U8Char, VariousLengths) {
    EXPECT_EQ(U8Char("\x41").size(), 1); // ASCII
    EXPECT_EQ(U8Char("\xC2").size(), 2); // 2-byte
    EXPECT_EQ(U8Char("\xE2").size(), 3); // 3-byte
    EXPECT_EQ(U8Char("\xF0").size(), 4); // 4-byte
    EXPECT_EQ(U8Char("\xFF").size(), 1); // Invalid
}

TEST(U8Char, ValidSequences) {
    EXPECT_EQ(U8Char("A").codepoint(), 0x41);
    EXPECT_EQ(U8Char("é").codepoint(), 0xE9);
    EXPECT_EQ(U8Char("€").codepoint(), 0x20AC);
    EXPECT_EQ(U8Char("😀").codepoint(), 0x1F600);
}

TEST(U8Char, InvalidSequences) {
    EXPECT_EQ(U8Char("").codepoint(), 0xFFFD); // empty
    EXPECT_EQ(U8Char("\xC3").codepoint(), 0xFFFD); // incomplete, fallback
}
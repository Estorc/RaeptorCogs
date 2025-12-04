#include <gtest/gtest.h>
#include <RaeptorCogs/IO/Path.hpp>

using namespace RaeptorCogs;

TEST(PathTest, ParseURLWithFullComponents) {
    URLParts parts = ParseURL("https://example.com/path/to/resource");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_EQ(parts.domain, "example.com");
    EXPECT_EQ(parts.path, "/path/to/resource");
}

TEST(PathTest, ParseURLWithoutPath) {
    URLParts parts = ParseURL("https://example.com");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_EQ(parts.domain, "example.com");
    EXPECT_EQ(parts.path, "/");
}

TEST(PathTest, ParseURLWithTrailingSlash) {
    URLParts parts = ParseURL("https://example.com/");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_EQ(parts.domain, "example.com");
    EXPECT_EQ(parts.path, "/");
}

TEST(PathTest, ParseURLWithQueryString) {
    URLParts parts = ParseURL("https://example.com/search?q=test");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_EQ(parts.domain, "example.com");
    EXPECT_TRUE(parts.path.find("/search") != std::string::npos);
}

TEST(PathTest, ParseURLWithPort) {
    URLParts parts = ParseURL("https://example.com:8080/api");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_TRUE(parts.domain.find("example.com") != std::string::npos);
    EXPECT_TRUE(parts.path.find("/api") != std::string::npos);
}

TEST(PathTest, ParseURLHttpProtocol) {
    URLParts parts = ParseURL("http://example.com/page");
    
    EXPECT_EQ(parts.protocol, "http");
    EXPECT_EQ(parts.domain, "example.com");
    EXPECT_EQ(parts.path, "/page");
}

TEST(PathTest, ParseURLWithSubdomain) {
    URLParts parts = ParseURL("https://api.example.com/v1/users");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_TRUE(parts.domain.find("api.example.com") != std::string::npos);
    EXPECT_TRUE(parts.path.find("/v1/users") != std::string::npos);
}

TEST(PathTest, ParseURLComplexPath) {
    URLParts parts = ParseURL("https://example.com/path/to/deep/resource/file.html");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_EQ(parts.domain, "example.com");
    EXPECT_TRUE(parts.path.find("file.html") != std::string::npos);
}

TEST(PathTest, ParseURLWithFragment) {
    URLParts parts = ParseURL("https://example.com/page#section");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_EQ(parts.domain, "example.com");
    // Fragment may be part of path
}

TEST(PathTest, ParseURLLocalhostWithPort) {
    URLParts parts = ParseURL("http://localhost:3000/api");
    
    EXPECT_EQ(parts.protocol, "http");
    EXPECT_TRUE(parts.domain.find("localhost") != std::string::npos);
    EXPECT_TRUE(parts.path.find("/api") != std::string::npos);
}

TEST(PathTest, ParseURLIPAddress) {
    URLParts parts = ParseURL("http://192.168.1.1/admin");
    
    EXPECT_EQ(parts.protocol, "http");
    EXPECT_TRUE(parts.domain.find("192.168.1.1") != std::string::npos);
    EXPECT_EQ(parts.path, "/admin");
}

TEST(PathTest, ParseURLWithAuthentication) {
    URLParts parts = ParseURL("https://user:pass@example.com/secure");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_TRUE(parts.domain.find("example.com") != std::string::npos);
    EXPECT_EQ(parts.path, "/secure");
}

TEST(PathTest, ParseURLEmptyPath) {
    URLParts parts = ParseURL("https://example.com");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_EQ(parts.path, "/");
}

TEST(PathTest, ParseURLDomainNotEmpty) {
    URLParts parts = ParseURL("https://example.com/path");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_FALSE(parts.domain.empty());
}

TEST(PathTest, ParseURLMultipleLevels) {
    URLParts parts = ParseURL("https://example.com/a/b/c/d/e/f");
    
    EXPECT_EQ(parts.protocol, "https");
    EXPECT_EQ(parts.domain, "example.com");
    EXPECT_TRUE(parts.path.find("/a") != std::string::npos);
    EXPECT_TRUE(parts.path.find("/f") != std::string::npos);
}

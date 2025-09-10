#include "UtilTest.h"
#include <fstream>
#include <filesystem>

namespace CHTL {
namespace Test {

std::unique_ptr<TestSuite> UtilTest::createTestSuite() {
    auto suite = std::make_unique<TestSuite>("UtilTest");
    
    // 字符串工具测试
    suite->addTestCase(std::make_unique<TestCase>("testStringUtilTrim", "测试字符串修剪功能", testStringUtilTrim));
    suite->addTestCase(std::make_unique<TestCase>("testStringUtilSplit", "测试字符串分割功能", testStringUtilSplit));
    suite->addTestCase(std::make_unique<TestCase>("testStringUtilJoin", "测试字符串连接功能", testStringUtilJoin));
    suite->addTestCase(std::make_unique<TestCase>("testStringUtilReplace", "测试字符串替换功能", testStringUtilReplace));
    suite->addTestCase(std::make_unique<TestCase>("testStringUtilToLower", "测试字符串转小写功能", testStringUtilToLower));
    suite->addTestCase(std::make_unique<TestCase>("testStringUtilToUpper", "测试字符串转大写功能", testStringUtilToUpper));
    suite->addTestCase(std::make_unique<TestCase>("testStringUtilStartsWith", "测试字符串前缀检查功能", testStringUtilStartsWith));
    suite->addTestCase(std::make_unique<TestCase>("testStringUtilEndsWith", "测试字符串后缀检查功能", testStringUtilEndsWith));
    
    // 文件系统测试
    suite->addTestCase(std::make_unique<TestCase>("testFileSystemExists", "测试文件存在性检查功能", testFileSystemExists));
    suite->addTestCase(std::make_unique<TestCase>("testFileSystemReadWrite", "测试文件读写功能", testFileSystemReadWrite));
    suite->addTestCase(std::make_unique<TestCase>("testFileSystemCreateDirectory", "测试目录创建功能", testFileSystemCreateDirectory));
    suite->addTestCase(std::make_unique<TestCase>("testFileSystemListFiles", "测试文件列表功能", testFileSystemListFiles));
    
    // 压缩工具测试
    suite->addTestCase(std::make_unique<TestCase>("testZipUtilCompress", "测试ZIP压缩功能", testZipUtilCompress));
    suite->addTestCase(std::make_unique<TestCase>("testZipUtilDecompress", "测试ZIP解压功能", testZipUtilDecompress));
    suite->addTestCase(std::make_unique<TestCase>("testZipUtilListFiles", "测试ZIP文件列表功能", testZipUtilListFiles));
    
    return suite;
}

// 字符串工具测试实现
void UtilTest::testStringUtilTrim() {
    CHTL_ASSERT_EQUAL("hello", StringUtil::trim("  hello  "));
    CHTL_ASSERT_EQUAL("world", StringUtil::trim("\t\nworld\t\n"));
    CHTL_ASSERT_EQUAL("", StringUtil::trim("   "));
    CHTL_ASSERT_EQUAL("test", StringUtil::trim("test"));
}

void UtilTest::testStringUtilSplit() {
    auto result1 = StringUtil::split("a,b,c", ",");
    CHTL_ASSERT_EQUAL("a", result1[0]);
    CHTL_ASSERT_EQUAL("b", result1[1]);
    CHTL_ASSERT_EQUAL("c", result1[2]);
    
    auto result2 = StringUtil::split("hello world test", " ");
    CHTL_ASSERT_EQUAL("hello", result2[0]);
    CHTL_ASSERT_EQUAL("world", result2[1]);
    CHTL_ASSERT_EQUAL("test", result2[2]);
}

void UtilTest::testStringUtilJoin() {
    std::vector<std::string> vec1 = {"a", "b", "c"};
    CHTL_ASSERT_EQUAL("a,b,c", StringUtil::join(vec1, ","));
    
    std::vector<std::string> vec2 = {"hello", "world", "test"};
    CHTL_ASSERT_EQUAL("hello world test", StringUtil::join(vec2, " "));
}

void UtilTest::testStringUtilReplace() {
    CHTL_ASSERT_EQUAL("hello world", StringUtil::replace("hello test", "test", "world"));
    CHTL_ASSERT_EQUAL("abc", StringUtil::replace("a b c", " ", ""));
    CHTL_ASSERT_EQUAL("original", StringUtil::replace("original", "notfound", "replacement"));
}

void UtilTest::testStringUtilToLower() {
    CHTL_ASSERT_EQUAL("hello world", StringUtil::toLower("HELLO WORLD"));
    CHTL_ASSERT_EQUAL("test123", StringUtil::toLower("TEST123"));
    CHTL_ASSERT_EQUAL("", StringUtil::toLower(""));
}

void UtilTest::testStringUtilToUpper() {
    CHTL_ASSERT_EQUAL("HELLO WORLD", StringUtil::toUpper("hello world"));
    CHTL_ASSERT_EQUAL("TEST123", StringUtil::toUpper("test123"));
    CHTL_ASSERT_EQUAL("", StringUtil::toUpper(""));
}

void UtilTest::testStringUtilStartsWith() {
    CHTL_ASSERT_TRUE(StringUtil::startsWith("hello world", "hello"));
    CHTL_ASSERT_FALSE(StringUtil::startsWith("hello world", "world"));
    CHTL_ASSERT_TRUE(StringUtil::startsWith("test", ""));
}

void UtilTest::testStringUtilEndsWith() {
    CHTL_ASSERT_TRUE(StringUtil::endsWith("hello world", "world"));
    CHTL_ASSERT_FALSE(StringUtil::endsWith("hello world", "hello"));
    CHTL_ASSERT_TRUE(StringUtil::endsWith("test", ""));
}

// 文件系统测试实现
void UtilTest::testFileSystemExists() {
    // 创建临时文件
    std::ofstream file("test_file.txt");
    file << "test content";
    file.close();
    
    CHTL_ASSERT_TRUE(FileSystem::exists("test_file.txt"));
    CHTL_ASSERT_FALSE(FileSystem::exists("nonexistent_file.txt"));
    
    // 清理
    std::remove("test_file.txt");
}

void UtilTest::testFileSystemReadWrite() {
    std::string testContent = "This is a test file content\nwith multiple lines";
    
    // 测试写入
    CHTL_ASSERT_TRUE(FileSystem::writeFile("test_read_write.txt", testContent));
    
    // 测试读取
    std::string readContent = FileSystem::readFile("test_read_write.txt");
    CHTL_ASSERT_EQUAL(testContent, readContent);
    
    // 清理
    std::remove("test_read_write.txt");
}

void UtilTest::testFileSystemCreateDirectory() {
    // 测试目录创建
    CHTL_ASSERT_TRUE(FileSystem::createDirectory("test_dir"));
    CHTL_ASSERT_TRUE(FileSystem::exists("test_dir"));
    
    // 测试嵌套目录创建
    CHTL_ASSERT_TRUE(FileSystem::createDirectory("test_dir/nested_dir"));
    CHTL_ASSERT_TRUE(FileSystem::exists("test_dir/nested_dir"));
    
    // 清理
    std::filesystem::remove_all("test_dir");
}

void UtilTest::testFileSystemListFiles() {
    // 创建测试目录和文件
    FileSystem::createDirectory("test_list_dir");
    FileSystem::writeFile("test_list_dir/file1.txt", "content1");
    FileSystem::writeFile("test_list_dir/file2.txt", "content2");
    FileSystem::writeFile("test_list_dir/file3.log", "content3");
    
    // 测试列出所有文件
    auto allFiles = FileSystem::listFiles("test_list_dir");
    CHTL_ASSERT_TRUE(allFiles.size() >= 3);
    
    // 测试按扩展名过滤
    auto txtFiles = FileSystem::listFiles("test_list_dir", ".txt");
    CHTL_ASSERT_EQUAL("2", std::to_string(txtFiles.size()));
    
    // 清理
    std::filesystem::remove_all("test_list_dir");
}

// 压缩工具测试实现
void UtilTest::testZipUtilCompress() {
    // 创建测试文件
    FileSystem::writeFile("test_compress.txt", "This is test content for compression");
    
    // 测试压缩
    CHTL_ASSERT_TRUE(ZipUtil::compress("test_compress.txt", "test_compress.zip"));
    CHTL_ASSERT_TRUE(FileSystem::exists("test_compress.zip"));
    
    // 清理
    std::remove("test_compress.txt");
    std::remove("test_compress.zip");
}

void UtilTest::testZipUtilDecompress() {
    // 创建测试文件并压缩
    FileSystem::writeFile("test_decompress.txt", "This is test content for decompression");
    ZipUtil::compress("test_decompress.txt", "test_decompress.zip");
    std::remove("test_decompress.txt");
    
    // 测试解压
    CHTL_ASSERT_TRUE(ZipUtil::decompress("test_decompress.zip", "test_decompress_extracted.txt"));
    CHTL_ASSERT_TRUE(FileSystem::exists("test_decompress_extracted.txt"));
    
    // 验证内容
    std::string content = FileSystem::readFile("test_decompress_extracted.txt");
    CHTL_ASSERT_EQUAL("This is test content for decompression", content);
    
    // 清理
    std::remove("test_decompress.zip");
    std::remove("test_decompress_extracted.txt");
}

void UtilTest::testZipUtilListFiles() {
    // 创建测试文件并压缩
    FileSystem::writeFile("test_list1.txt", "content1");
    FileSystem::writeFile("test_list2.txt", "content2");
    ZipUtil::compress("test_list1.txt", "test_list.zip");
    ZipUtil::compress("test_list2.txt", "test_list.zip", true); // 追加模式
    
    // 测试列出ZIP文件内容
    auto files = ZipUtil::listFiles("test_list.zip");
    CHTL_ASSERT_TRUE(files.size() >= 2);
    
    // 清理
    std::remove("test_list1.txt");
    std::remove("test_list2.txt");
    std::remove("test_list.zip");
}

} // namespace Test
} // namespace CHTL
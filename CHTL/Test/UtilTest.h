#ifndef CHTL_UTIL_TEST_H
#define CHTL_UTIL_TEST_H

#include "TestFramework.h"
#include "../Util/StringUtil/StringUtil.h"
#include "../Util/FileSystem/FileSystem.h"
#include "../Util/ZipUtil/ZipUtil.h"

namespace CHTL {
namespace Test {

class UtilTest {
public:
    static std::unique_ptr<TestSuite> createTestSuite();
    
private:
    static void testStringUtil();
    static void testFileSystem();
    static void testZipUtil();
    static void testStringUtilTrim();
    static void testStringUtilSplit();
    static void testStringUtilJoin();
    static void testStringUtilReplace();
    static void testStringUtilToLower();
    static void testStringUtilToUpper();
    static void testStringUtilStartsWith();
    static void testStringUtilEndsWith();
    static void testFileSystemExists();
    static void testFileSystemReadWrite();
    static void testFileSystemCreateDirectory();
    static void testFileSystemListFiles();
    static void testZipUtilCompress();
    static void testZipUtilDecompress();
    static void testZipUtilListFiles();
};

} // namespace Test
} // namespace CHTL

#endif // CHTL_UTIL_TEST_H
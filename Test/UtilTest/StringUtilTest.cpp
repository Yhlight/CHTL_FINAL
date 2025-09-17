#include "../Test.h"
#include "../../Util/StringUtil/StringUtil.h"

// Test case for the Trim function
TEST_CASE(StringUtil_Trim) {
    // Test with leading/trailing spaces
    std::string s1 = "  hello world  ";
    CHTL::StringUtil::Trim(s1);
    ASSERT_EQUAL(std::string("hello world"), s1);

    // Test with no spaces
    std::string s2 = "no-spaces";
    CHTL::StringUtil::Trim(s2);
    ASSERT_EQUAL(std::string("no-spaces"), s2);

    // Test with only spaces
    std::string s3 = "    ";
    CHTL::StringUtil::Trim(s3);
    ASSERT_EQUAL(std::string(""), s3);

    // Test with mixed whitespace
    std::string s4 = "\n\t  hello\t\r ";
    CHTL::StringUtil::Trim(s4);
    ASSERT_EQUAL(std::string("hello"), s4);

    return true; // Return true if all asserts pass
}

// Test case for the Split function
TEST_CASE(StringUtil_Split) {
    // Test with a simple case
    std::string s1 = "one,two,three";
    std::vector<std::string> expected1 = {"one", "two", "three"};
    std::vector<std::string> result1 = CHTL::StringUtil::Split(s1, ',');
    ASSERT_EQUAL(expected1.size(), result1.size());
    for(size_t i=0; i<expected1.size(); ++i) {
        ASSERT_EQUAL(expected1[i], result1[i]);
    }

    // Test with a single element
    std::string s2 = "single";
    std::vector<std::string> expected2 = {"single"};
    std::vector<std::string> result2 = CHTL::StringUtil::Split(s2, ',');
    ASSERT_EQUAL(expected2.size(), result2.size());
    ASSERT_EQUAL(expected2[0], result2[0]);

    // Test with empty elements
    std::string s3 = "a,,b,";
    std::vector<std::string> expected3 = {"a", "", "b", ""};
    std::vector<std::string> result3 = CHTL::StringUtil::Split(s3, ',');
    ASSERT_EQUAL(expected3.size(), result3.size());
    for(size_t i=0; i<expected3.size(); ++i) {
        ASSERT_EQUAL(expected3[i], result3[i]);
    }

    return true;
}

#ifndef CHTL_STRINGUTIL_H
#define CHTL_STRINGUTIL_H

#include <string>
#include <vector>

namespace CHTL {
namespace Util {

class StringUtil {
public:
    // Trim from start (in place)
    static void ltrim(std::string &s);

    // Trim from end (in place)
    static void rtrim(std::string &s);

    // Trim from both ends (in place)
    static void trim(std::string &s);
};

} // namespace Util
} // namespace CHTL

#endif // CHTL_STRINGUTIL_H

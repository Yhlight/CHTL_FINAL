#include "StringUtil.h"
#include <algorithm>
#include <cctype>

std::string trim(std::string_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(" \t\n\r"), s.size()));
    s.remove_suffix(std::min(s.size() - s.find_last_not_of(" \t\n\r") - 1, s.size()));
    return std::string(s);
}

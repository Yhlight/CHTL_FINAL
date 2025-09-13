#include "CompilerDispatcher.h"

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(const std::unordered_map<std::string, std::string>& placeholders)
    : placeholders_(placeholders) {}

std::string CompilerDispatcher::getPlaceholderContent(const std::string& placeholder) const {
    auto it = placeholders_.find(placeholder);
    if (it != placeholders_.end()) {
        return it->second;
    }
    return ""; // Or throw an error
}

} // namespace CHTL

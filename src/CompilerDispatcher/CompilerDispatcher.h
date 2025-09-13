#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

class CompilerDispatcher {
public:
    explicit CompilerDispatcher(const std::unordered_map<std::string, std::string>& placeholders);

    std::string getPlaceholderContent(const std::string& placeholder) const;

private:
    std::unordered_map<std::string, std::string> placeholders_;
};

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H

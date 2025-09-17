#ifndef CHTL_CJMOD_MANAGER_H
#define CHTL_CJMOD_MANAGER_H

#include "API/Arg.h"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {
namespace CJMOD {

struct CJMODRule {
    std::string keyword;
    Arg syntax_arg;
};

class CJMODManager {
public:
    void registerRule(const std::string& keyword, Arg syntax_arg) {
        rules_.push_back({keyword, std::move(syntax_arg)});
    }

    const std::vector<CJMODRule>& getRules() const {
        return rules_;
    }

private:
    std::vector<CJMODRule> rules_;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_MANAGER_H

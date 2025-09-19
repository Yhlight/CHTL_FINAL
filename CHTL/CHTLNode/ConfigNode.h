#ifndef CONFIG_NODE_H
#define CONFIG_NODE_H

#include "BaseNode.h"
#include "../Expression/Expr.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

class ConfigNode : public BaseNode {
public:
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::map<std::string, std::unique_ptr<Expr>> settings;
    std::map<std::string, std::vector<std::string>> name_settings;
};

} // namespace CHTL

#endif // CONFIG_NODE_H

#pragma once

#include "../CHTLJSNode/CjsBaseNode.h"
#include <vector>
#include <memory>
#include <string>

class ChtlJsGenerator : public CjsVisitor {
public:
    ChtlJsGenerator() = default;
    std::string generate(const std::vector<std::shared_ptr<CjsBaseNode>>& ast);

    void visit(CjsSelectorNode& node) override;
    void visit(CjsPassthroughNode& node) override;

private:
    std::string generatedCode;
};

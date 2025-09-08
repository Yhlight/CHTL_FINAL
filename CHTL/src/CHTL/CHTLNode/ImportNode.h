#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

// Represents an `[Import]` directive.
class ImportNode : public BaseNode {
public:
    // A simple struct to hold the parts of the import
    struct ImportTarget {
        std::string type; // e.g., "[Custom]", "@Element"
        std::string name; // e.g., "MyBox"
    };

    ImportNode(std::vector<ImportTarget> targets, std::string path, std::string alias)
        : m_targets(std::move(targets)), m_path(std::move(path)), m_alias(std::move(alias)) {}

    NodeType getType() const override { return NodeType::Import; }

    std::unique_ptr<BaseNode> clone() const override {
        // Import nodes are processed at parse time and don't need to be cloned for generation.
        // Returning nullptr is a valid strategy for nodes that don't persist in the final AST.
        // However, to be safe, we can return a copy.
        return std::make_unique<ImportNode>(m_targets, m_path, m_alias);
    }

    const std::vector<ImportTarget>& getTargets() const { return m_targets; }
    const std::string& getPath() const { return m_path; }
    const std::string& getAlias() const { return m_alias; }

private:
    std::vector<ImportTarget> m_targets;
    std::string m_path;
    std::string m_alias;
};

} // namespace CHTL

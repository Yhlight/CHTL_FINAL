#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

enum class ImportType {
    Chtl,
    Style,
    JavaScript,
    // Potentially others later
};

class ImportNode : public BaseNode {
public:
    ImportNode(ImportType importType, std::string path)
        : m_importType(importType), m_path(std::move(path)) {}

    NodeType GetType() const override { return NodeType::Import; }
    NodePtr Clone() const override {
        // Import nodes are processed at a global level and don't need to be cloned for specialization.
        // However, to satisfy the interface, we return a shared pointer to self.
        // A better implementation might make them non-copyable and handle them separately.
        return std::make_shared<ImportNode>(m_importType, m_path);
    }

    ImportType GetImportType() const { return m_importType; }
    const std::string& GetPath() const { return m_path; }

private:
    ImportType m_importType;
    std::string m_path;
};

} // namespace CHTL

#include "CompilationContext.h"
#include "CHTLNode/ImportNode.h"
#include <stdexcept>

namespace CHTL {

void CompilationContext::AddTemplate(const std::string& name, const TemplateDefinitionNode* node) {
    if (m_template_repo.count(name)) {
        // For now, we don't handle redefinitions.
        // A more robust implementation might allow it or have specific rules.
        return;
    }
    m_template_repo[name] = node;
}

const TemplateDefinitionNode* CompilationContext::GetTemplate(const std::string& name) const {
    auto it = m_template_repo.find(name);
    if (it != m_template_repo.end()) {
        return it->second;
    }
    return nullptr;
}

void CompilationContext::SetBasePath(const std::string& path) {
    m_basePath = path;
}

const std::string& CompilationContext::GetBasePath() const {
    return m_basePath;
}

void CompilationContext::AddImport(const ImportNode* node) {
    m_imports.push_back(node);
}

const std::vector<const ImportNode*>& CompilationContext::GetImports() const {
    return m_imports;
}

void CompilationContext::ClearImports() {
    m_imports.clear();
}

void CompilationContext::MarkAsProcessed(const std::string& path) {
    m_processed_files.insert(path);
}

bool CompilationContext::IsProcessed(const std::string& path) const {
    return m_processed_files.count(path);
}

void CompilationContext::AddAst(NodeList ast) {
    m_imported_asts.push_back(std::move(ast));
}

} // namespace CHTL

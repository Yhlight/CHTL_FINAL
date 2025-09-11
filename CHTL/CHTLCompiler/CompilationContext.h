#pragma once

#include "CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <set>

namespace CHTL {

class ImportNode; // Forward declaration

class CompilationContext {
public:
    void AddTemplate(const std::string& name, const TemplateDefinitionNode* node);
    const TemplateDefinitionNode* GetTemplate(const std::string& name) const;

    void AddImport(const ImportNode* node);
    const std::vector<const ImportNode*>& GetImports() const;
    void ClearImports();

    // This will later be used to resolve file paths
    void SetBasePath(const std::string& path);
    const std::string& GetBasePath() const;

    // To prevent circular imports
    void MarkAsProcessed(const std::string& path);
    bool IsProcessed(const std::string& path) const;

    // To keep ASTs from imported files alive
    void AddAst(NodeList ast);

private:
    std::vector<NodeList> m_imported_asts;
    std::map<std::string, const TemplateDefinitionNode*> m_template_repo;
    std::vector<const ImportNode*> m_imports;
    std::set<std::string> m_processed_files;
    std::string m_basePath;
};

} // namespace CHTL

#ifndef CHTLTEMPLATE_H
#define CHTLTEMPLATE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

// Template types
enum class TemplateType {
    Style,
    Element,
    Var
};

// Template inheritance types
enum class InheritanceType {
    None,
    Inherit,
    Delete,
    Insert,
    After,
    Before,
    Replace
};

// Template modification
struct TemplateModification {
    InheritanceType type;
    std::string target;
    std::string value;
    size_t position;
    
    TemplateModification(InheritanceType t, const std::string& target_name, const std::string& val = "", size_t pos = 0)
        : type(t), target(target_name), value(val), position(pos) {}
};

// Template definition
class CHTLTemplate {
public:
    std::string name;
    TemplateType type;
    std::vector<std::shared_ptr<CHTLNode>> content;
    std::vector<TemplateModification> modifications;
    std::map<std::string, std::string> parameters;
    std::set<std::string> dependencies;
    bool is_abstract;
    std::string parent_template;
    
    CHTLTemplate(const std::string& template_name, TemplateType template_type)
        : name(template_name), type(template_type), is_abstract(false) {}
    
    virtual ~CHTLTemplate() = default;
    
    // Content management
    void addContent(std::shared_ptr<CHTLNode> node);
    void addModification(const TemplateModification& modification);
    void setParameter(const std::string& key, const std::string& value);
    void addDependency(const std::string& dependency);
    
    // Template processing
    std::vector<std::shared_ptr<CHTLNode>> processTemplate(const std::map<std::string, std::string>& parameters = {});
    std::vector<std::shared_ptr<CHTLNode>> applyModifications(const std::vector<std::shared_ptr<CHTLNode>>& base_content);
    
    // Validation
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
};

// Style template specialization
class StyleTemplate : public CHTLTemplate {
public:
    StyleTemplate(const std::string& name) : CHTLTemplate(name, TemplateType::Style) {}
    
    std::string generateCSS(const std::map<std::string, std::string>& parameters = {});
    std::vector<std::string> getCSSProperties() const;
    std::vector<std::string> getCSSSelectors() const;
};

// Element template specialization
class ElementTemplate : public CHTLTemplate {
public:
    ElementTemplate(const std::string& name) : CHTLTemplate(name, TemplateType::Element) {}
    
    std::string generateHTML(const std::map<std::string, std::string>& parameters = {});
    std::vector<std::string> getElementNames() const;
    std::vector<std::string> getRequiredAttributes() const;
};

// Variable template specialization
class VarTemplate : public CHTLTemplate {
public:
    VarTemplate(const std::string& name) : CHTLTemplate(name, TemplateType::Var) {}
    
    std::string generateValue(const std::map<std::string, std::string>& parameters = {});
    std::vector<std::string> getVariableNames() const;
    std::string getDefaultValue() const;
};

// Template manager
class CHTLTemplateManager {
public:
    CHTLTemplateManager();
    ~CHTLTemplateManager() = default;
    
    // Template registration
    void registerTemplate(std::shared_ptr<CHTLTemplate> template_def);
    void registerStyleTemplate(std::shared_ptr<StyleTemplate> template_def);
    void registerElementTemplate(std::shared_ptr<ElementTemplate> template_def);
    void registerVarTemplate(std::shared_ptr<VarTemplate> template_def);
    
    // Template resolution
    std::shared_ptr<CHTLTemplate> resolveTemplate(const std::string& name, TemplateType type);
    std::shared_ptr<StyleTemplate> resolveStyleTemplate(const std::string& name);
    std::shared_ptr<ElementTemplate> resolveElementTemplate(const std::string& name);
    std::shared_ptr<VarTemplate> resolveVarTemplate(const std::string& name);
    
    // Template processing
    std::vector<std::shared_ptr<CHTLNode>> processTemplate(const std::string& name, TemplateType type, const std::map<std::string, std::string>& parameters = {});
    std::string processStyleTemplate(const std::string& name, const std::map<std::string, std::string>& parameters = {});
    std::string processElementTemplate(const std::string& name, const std::map<std::string, std::string>& parameters = {});
    std::string processVarTemplate(const std::string& name, const std::map<std::string, std::string>& parameters = {});
    
    // Template inheritance
    std::vector<std::shared_ptr<CHTLNode>> processInheritance(std::shared_ptr<CHTLTemplate> template_def, const std::map<std::string, std::string>& parameters = {});
    std::vector<std::shared_ptr<CHTLNode>> applyInheritanceModifications(const std::vector<std::shared_ptr<CHTLNode>>& base_content, const std::vector<TemplateModification>& modifications);
    
    // Template analysis
    std::vector<std::string> getTemplateDependencies(const std::string& name, TemplateType type);
    std::vector<std::string> getCircularDependencies();
    std::map<std::string, std::vector<std::string>> getDependencyGraph();
    
    // Template validation
    bool validateTemplate(const std::string& name, TemplateType type);
    std::vector<std::string> getTemplateValidationErrors(const std::string& name, TemplateType type);
    bool validateAllTemplates();
    
    // Template utilities
    std::vector<std::string> getAllTemplateNames(TemplateType type);
    std::vector<std::string> getAllTemplateNames();
    bool templateExists(const std::string& name, TemplateType type);
    void removeTemplate(const std::string& name, TemplateType type);
    void clearTemplates();
    
private:
    // Template storage
    std::map<std::string, std::shared_ptr<CHTLTemplate>> templates;
    std::map<std::string, std::shared_ptr<StyleTemplate>> style_templates;
    std::map<std::string, std::shared_ptr<ElementTemplate>> element_templates;
    std::map<std::string, std::shared_ptr<VarTemplate>> var_templates;
    
    // Dependency tracking
    std::map<std::string, std::set<std::string>> dependency_graph;
    std::set<std::string> processing_templates;
    
    // Helper methods
    void buildDependencyGraph();
    bool hasCircularDependency(const std::string& template_name, std::set<std::string>& visited, std::set<std::string>& recursion_stack);
    std::vector<std::string> topologicalSort();
    std::string generateTemplateKey(const std::string& name, TemplateType type);
    std::shared_ptr<CHTLTemplate> findTemplate(const std::string& name, TemplateType type);
    
    // Template processing helpers
    std::vector<std::shared_ptr<CHTLNode>> cloneNodes(const std::vector<std::shared_ptr<CHTLNode>>& nodes);
    std::vector<std::shared_ptr<CHTLNode>> applyParameterSubstitution(const std::vector<std::shared_ptr<CHTLNode>>& nodes, const std::map<std::string, std::string>& parameters);
    std::string substituteParameters(const std::string& text, const std::map<std::string, std::string>& parameters);
    
    // Modification processing
    std::vector<std::shared_ptr<CHTLNode>> processInheritModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification);
    std::vector<std::shared_ptr<CHTLNode>> processDeleteModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification);
    std::vector<std::shared_ptr<CHTLNode>> processInsertModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification);
    std::vector<std::shared_ptr<CHTLNode>> processAfterModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification);
    std::vector<std::shared_ptr<CHTLNode>> processBeforeModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification);
    std::vector<std::shared_ptr<CHTLNode>> processReplaceModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification);
};

// Template processor for runtime processing
class CHTLTemplateProcessor {
public:
    CHTLTemplateProcessor(std::shared_ptr<CHTLTemplateManager> manager);
    ~CHTLTemplateProcessor() = default;
    
    // Template processing
    std::string processTemplateReference(const std::string& reference, const std::map<std::string, std::string>& context = {});
    std::vector<std::shared_ptr<CHTLNode>> processTemplateNode(std::shared_ptr<TemplateNode> template_node, const std::map<std::string, std::string>& context = {});
    
    // Parameter resolution
    std::map<std::string, std::string> resolveParameters(const std::map<std::string, std::string>& template_params, const std::map<std::string, std::string>& context);
    std::string resolveParameterValue(const std::string& param_name, const std::map<std::string, std::string>& context);
    
    // Context management
    void pushContext(const std::map<std::string, std::string>& context);
    void popContext();
    std::map<std::string, std::string> getCurrentContext();
    
private:
    std::shared_ptr<CHTLTemplateManager> template_manager;
    std::vector<std::map<std::string, std::string>> context_stack;
    
    // Helper methods
    std::string parseTemplateReference(const std::string& reference);
    std::map<std::string, std::string> parseTemplateParameters(const std::string& param_string);
    bool isTemplateReference(const std::string& text);
};

} // namespace CHTL

#endif // CHTLTEMPLATE_H
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <functional>

namespace chtl {
namespace template_system {

// 模板类型
enum class TemplateType {
    STYLE_GROUP,    // 样式组模板
    ELEMENT,        // 元素模板
    VARIABLE_GROUP  // 变量组模板
};

// 继承类型
enum class InheritanceType {
    COMPOSITIONAL,  // 组合继承
    EXPLICIT        // 显性继承
};

// 模板参数
struct TemplateParameter {
    std::string name;
    std::string type;
    std::string defaultValue;
    bool required;
    
    TemplateParameter(const std::string& n, const std::string& t = "", const std::string& def = "", bool req = false)
        : name(n), type(t), defaultValue(def), required(req) {}
};

// 样式组模板
struct StyleGroupTemplate {
    std::string name;
    std::vector<TemplateParameter> parameters;
    std::vector<std::string> properties;
    std::vector<std::string> nestedGroups;
    std::string parentTemplate;
    InheritanceType inheritanceType;
    
    StyleGroupTemplate(const std::string& n = "") : name(n), inheritanceType(InheritanceType::COMPOSITIONAL) {}
};

// 元素模板
struct ElementTemplate {
    std::string name;
    std::vector<TemplateParameter> parameters;
    std::string elementType;
    std::vector<std::string> attributes;
    std::vector<std::string> children;
    std::string parentTemplate;
    InheritanceType inheritanceType;
    
    ElementTemplate(const std::string& n = "") : name(n), inheritanceType(InheritanceType::COMPOSITIONAL) {}
};

// 变量组模板
struct VariableGroupTemplate {
    std::string name;
    std::vector<TemplateParameter> parameters;
    std::map<std::string, std::string> variables;
    std::string parentTemplate;
    InheritanceType inheritanceType;
    
    VariableGroupTemplate(const std::string& n = "") : name(n), inheritanceType(InheritanceType::COMPOSITIONAL) {}
};

// 模板实例
struct TemplateInstance {
    std::string templateName;
    std::string instanceName;
    std::map<std::string, std::string> parameters;
    TemplateType type;
    
    TemplateInstance(const std::string& tName, const std::string& iName, TemplateType t)
        : templateName(tName), instanceName(iName), type(t) {}
};

// 模板解析器
class TemplateParser {
public:
    TemplateParser(const std::string& input);
    
    std::vector<std::shared_ptr<TemplateInstance>> parse();
    
private:
    std::string input_;
    size_t position_;
    
    void skipWhitespace();
    char currentChar() const;
    char peekChar() const;
    void advance();
    
    std::shared_ptr<StyleGroupTemplate> parseStyleGroupTemplate();
    std::shared_ptr<ElementTemplate> parseElementTemplate();
    std::shared_ptr<VariableGroupTemplate> parseVariableGroupTemplate();
    
    std::vector<TemplateParameter> parseParameters();
    TemplateParameter parseParameter();
    std::string parseString();
    std::string parseIdentifier();
    std::string parseNumber();
    
    bool isKeyword(const std::string& word) const;
    bool isTemplateKeyword(const std::string& word) const;
};

// 模板编译器
class TemplateCompiler {
public:
    TemplateCompiler();
    
    std::string compile(const std::vector<std::shared_ptr<TemplateInstance>>& instances);
    std::string compileStyleGroupTemplate(const StyleGroupTemplate& tmpl, const TemplateInstance& instance);
    std::string compileElementTemplate(const ElementTemplate& tmpl, const TemplateInstance& instance);
    std::string compileVariableGroupTemplate(const VariableGroupTemplate& tmpl, const TemplateInstance& instance);
    
    void addTemplate(const std::string& name, std::shared_ptr<StyleGroupTemplate> tmpl);
    void addTemplate(const std::string& name, std::shared_ptr<ElementTemplate> tmpl);
    void addTemplate(const std::string& name, std::shared_ptr<VariableGroupTemplate> tmpl);
    
    std::shared_ptr<StyleGroupTemplate> getStyleGroupTemplate(const std::string& name);
    std::shared_ptr<ElementTemplate> getElementTemplate(const std::string& name);
    std::shared_ptr<VariableGroupTemplate> getVariableGroupTemplate(const std::string& name);
    
private:
    std::map<std::string, std::shared_ptr<StyleGroupTemplate>> styleGroupTemplates_;
    std::map<std::string, std::shared_ptr<ElementTemplate>> elementTemplates_;
    std::map<std::string, std::shared_ptr<VariableGroupTemplate>> variableGroupTemplates_;
    
    std::string resolveParameters(const std::string& content, const std::map<std::string, std::string>& parameters);
    std::string processInheritance(const std::string& templateName, const std::string& content, InheritanceType type);
    std::string mergeTemplates(const std::string& parent, const std::string& child);
};

// 模板管理器
class TemplateManager {
public:
    TemplateManager();
    
    void addTemplate(const std::string& name, std::shared_ptr<StyleGroupTemplate> tmpl);
    void addTemplate(const std::string& name, std::shared_ptr<ElementTemplate> tmpl);
    void addTemplate(const std::string& name, std::shared_ptr<VariableGroupTemplate> tmpl);
    
    std::shared_ptr<StyleGroupTemplate> getStyleGroupTemplate(const std::string& name);
    std::shared_ptr<ElementTemplate> getElementTemplate(const std::string& name);
    std::shared_ptr<VariableGroupTemplate> getVariableGroupTemplate(const std::string& name);
    
    std::string generateCode(const std::vector<std::shared_ptr<TemplateInstance>>& instances);
    std::string generateStyleGroupCode(const StyleGroupTemplate& tmpl, const TemplateInstance& instance);
    std::string generateElementCode(const ElementTemplate& tmpl, const TemplateInstance& instance);
    std::string generateVariableGroupCode(const VariableGroupTemplate& tmpl, const TemplateInstance& instance);
    
    void clear();
    size_t getTemplateCount() const;
    
private:
    TemplateCompiler compiler_;
    
    std::string processTemplateInheritance(const std::string& templateName, const std::string& content, TemplateType type);
    std::string resolveTemplateDependencies(const std::string& content);
    std::string validateTemplateParameters(const std::string& templateName, const std::map<std::string, std::string>& parameters, TemplateType type);
};

} // namespace template_system
} // namespace chtl
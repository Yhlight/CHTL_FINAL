#include "TemplateSystem.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// Template基类方法实现
void Template::addChild(std::unique_ptr<ASTNode> child) {
    children.push_back(std::move(child));
}

// StyleTemplate方法实现
std::string StyleTemplate::expand(const std::map<std::string, std::string>& params) {
    std::stringstream css;
    
    css << "." << name << " {\n";
    
    for (const auto& prop : properties) {
        std::string value = prop.second;
        
        // 替换参数
        for (const auto& param : params) {
            std::string placeholder = "{" + param.first + "}";
            size_t pos = 0;
            while ((pos = value.find(placeholder, pos)) != std::string::npos) {
                value.replace(pos, placeholder.length(), param.second);
                pos += param.second.length();
            }
        }
        
        css << "    " << prop.first << ": " << value << ";\n";
    }
    
    css << "}\n";
    
    return css.str();
}

// ElementTemplate方法实现
std::string ElementTemplate::expand(const std::map<std::string, std::string>& params) {
    std::stringstream html;
    
    // 这里需要根据具体的元素类型来生成HTML
    // 简化实现，实际需要更复杂的逻辑
    html << "<!-- Element Template: " << name << " -->\n";
    
    for (const auto& child : children) {
        // 递归处理子节点
        if (child->type == ASTNode::NodeType::ELEMENT) {
            auto element = dynamic_cast<ElementNode*>(child.get());
            html << "<" << element->getTagName() << ">";
            html << "</" << element->getTagName() << ">\n";
        }
    }
    
    return html.str();
}

// VarTemplate方法实现
std::string VarTemplate::expand(const std::map<std::string, std::string>& params) {
    std::stringstream result;
    
    for (const auto& prop : properties) {
        std::string value = prop.second;
        
        // 替换参数
        for (const auto& param : params) {
            std::string placeholder = "{" + param.first + "}";
            size_t pos = 0;
            while ((pos = value.find(placeholder, pos)) != std::string::npos) {
                value.replace(pos, placeholder.length(), param.second);
                pos += param.second.length();
            }
        }
        
        result << prop.first << ": " << value << ";\n";
    }
    
    return result.str();
}

// Custom基类方法实现
void Custom::addChild(std::unique_ptr<ASTNode> child) {
    children.push_back(std::move(child));
}

void Custom::addSpecialization(const std::string& spec) {
    specializations.push_back(spec);
}

// CustomStyle方法实现
std::string CustomStyle::expand(const std::map<std::string, std::string>& params) {
    std::stringstream css;
    
    css << "." << name << " {\n";
    
    for (const auto& prop : properties) {
        std::string value = prop.second;
        
        // 替换参数
        for (const auto& param : params) {
            std::string placeholder = "{" + param.first + "}";
            size_t pos = 0;
            while ((pos = value.find(placeholder, pos)) != std::string::npos) {
                value.replace(pos, placeholder.length(), param.second);
                pos += param.second.length();
            }
        }
        
        css << "    " << prop.first << ": " << value << ";\n";
    }
    
    css << "}\n";
    
    return css.str();
}

// CustomElement方法实现
std::string CustomElement::expand(const std::map<std::string, std::string>& params) {
    std::stringstream html;
    
    html << "<!-- Custom Element: " << name << " -->\n";
    
    for (const auto& child : children) {
        if (child->type == ASTNode::NodeType::ELEMENT) {
            auto element = dynamic_cast<ElementNode*>(child.get());
            html << "<" << element->getTagName() << ">";
            html << "</" << element->getTagName() << ">\n";
        }
    }
    
    return html.str();
}

// CustomVar方法实现
std::string CustomVar::expand(const std::map<std::string, std::string>& params) {
    std::stringstream result;
    
    for (const auto& prop : properties) {
        std::string value = prop.second;
        
        // 替换参数
        for (const auto& param : params) {
            std::string placeholder = "{" + param.first + "}";
            size_t pos = 0;
            while ((pos = value.find(placeholder, pos)) != std::string::npos) {
                value.replace(pos, placeholder.length(), param.second);
                pos += param.second.length();
            }
        }
        
        result << prop.first << ": " << value << ";\n";
    }
    
    return result.str();
}

// Namespace方法实现
void Namespace::addTemplate(std::unique_ptr<Template> templateNode) {
    templates[templateNode->name] = std::move(templateNode);
}

void Namespace::addCustom(std::unique_ptr<Custom> custom) {
    customs[custom->name] = std::move(custom);
}

void Namespace::addImport(std::unique_ptr<ImportInfo> import) {
    imports[import->alias.empty() ? import->path : import->alias] = std::move(import);
}

Template* Namespace::getTemplate(const std::string& name) {
    auto it = templates.find(name);
    return it != templates.end() ? it->second.get() : nullptr;
}

Custom* Namespace::getCustom(const std::string& name) {
    auto it = customs.find(name);
    return it != customs.end() ? it->second.get() : nullptr;
}

ImportInfo* Namespace::getImport(const std::string& name) {
    auto it = imports.find(name);
    return it != imports.end() ? it->second.get() : nullptr;
}

// TemplateSystem方法实现
TemplateSystem::TemplateSystem() {}

TemplateSystem::~TemplateSystem() {}

void TemplateSystem::registerTemplate(std::unique_ptr<Template> templateNode) {
    templates[templateNode->name] = std::move(templateNode);
}

void TemplateSystem::registerCustom(std::unique_ptr<Custom> custom) {
    customs[custom->name] = std::move(custom);
}

void TemplateSystem::registerImport(std::unique_ptr<ImportInfo> import) {
    imports[import->alias.empty() ? import->path : import->alias] = std::move(import);
}

void TemplateSystem::registerNamespace(std::unique_ptr<Namespace> namespaceNode) {
    namespaces[namespaceNode->name] = std::move(namespaceNode);
}

Template* TemplateSystem::getTemplate(const std::string& name, const std::string& namespaceName) {
    if (namespaceName.empty()) {
        auto it = templates.find(name);
        return it != templates.end() ? it->second.get() : nullptr;
    } else {
        auto ns = getNamespace(namespaceName);
        return ns ? ns->getTemplate(name) : nullptr;
    }
}

Custom* TemplateSystem::getCustom(const std::string& name, const std::string& namespaceName) {
    if (namespaceName.empty()) {
        auto it = customs.find(name);
        return it != customs.end() ? it->second.get() : nullptr;
    } else {
        auto ns = getNamespace(namespaceName);
        return ns ? ns->getCustom(name) : nullptr;
    }
}

ImportInfo* TemplateSystem::getImport(const std::string& name, const std::string& namespaceName) {
    if (namespaceName.empty()) {
        auto it = imports.find(name);
        return it != imports.end() ? it->second.get() : nullptr;
    } else {
        auto ns = getNamespace(namespaceName);
        return ns ? ns->getImport(name) : nullptr;
    }
}

Namespace* TemplateSystem::getNamespace(const std::string& name) {
    auto it = namespaces.find(name);
    return it != namespaces.end() ? it->second.get() : nullptr;
}

std::string TemplateSystem::expandTemplate(const std::string& name, 
                                          const std::map<std::string, std::string>& params,
                                          const std::string& namespaceName) {
    auto templateNode = getTemplate(name, namespaceName);
    if (templateNode) {
        return templateNode->expand(params);
    }
    return "";
}

std::string TemplateSystem::expandCustom(const std::string& name, 
                                        const std::map<std::string, std::string>& params,
                                        const std::string& namespaceName) {
    auto custom = getCustom(name, namespaceName);
    if (custom) {
        return custom->expand(params);
    }
    return "";
}

std::string TemplateSystem::processImport(const ImportInfo& import) {
    std::stringstream result;
    
    switch (import.type) {
        case ImportType::HTML:
            result << "<!-- Imported HTML: " << import.path << " -->\n";
            break;
        case ImportType::STYLE:
            result << "/* Imported CSS: " << import.path << " */\n";
            break;
        case ImportType::JAVASCRIPT:
            result << "// Imported JS: " << import.path << "\n";
            break;
        case ImportType::CHTL:
            result << "// Imported CHTL: " << import.path << "\n";
            break;
        case ImportType::CJMOD:
            result << "// Imported CJMOD: " << import.path << "\n";
            break;
    }
    
    return result.str();
}

std::map<std::string, std::string> TemplateSystem::parseTemplateReference(const std::string& reference) {
    return parseReference(reference);
}

std::map<std::string, std::string> TemplateSystem::parseCustomReference(const std::string& reference) {
    return parseReference(reference);
}

std::map<std::string, std::string> TemplateSystem::parseNamespaceReference(const std::string& reference) {
    return parseReference(reference);
}

std::unique_ptr<Custom> TemplateSystem::processSpecialization(Custom* custom, 
                                                             const std::vector<std::string>& specializations) {
    if (!custom) return nullptr;
    
    // 创建特例化后的自定义
    auto specialized = std::make_unique<CustomStyle>(custom->name + "_specialized");
    if (custom->type == CustomType::ELEMENT) {
        // specialized = std::make_unique<CustomElement>(custom->name + "_specialized");
    } else if (custom->type == CustomType::VAR) {
        // specialized = std::make_unique<CustomVar>(custom->name + "_specialized");
    }
    
    // 复制原始属性
    specialized->properties = custom->properties;
    
    // 应用特例化
    for (const auto& spec : specializations) {
        specialized->addSpecialization(spec);
    }
    
    return specialized;
}

std::unique_ptr<Template> TemplateSystem::processInheritance(Template* child, Template* parent) {
    if (!child || !parent) return nullptr;
    
    // 创建继承后的模板
    auto inherited = std::make_unique<StyleTemplate>(child->name + "_inherited");
    if (parent->type == TemplateType::ELEMENT) {
        // inherited = std::make_unique<ElementTemplate>(child->name + "_inherited");
    } else if (parent->type == TemplateType::VAR) {
        // inherited = std::make_unique<VarTemplate>(child->name + "_inherited");
    }
    
    // 合并属性
    inherited->properties = parent->properties;
    for (const auto& prop : child->properties) {
        inherited->properties[prop.first] = prop.second;
    }
    
    return inherited;
}

std::unique_ptr<Custom> TemplateSystem::processInheritance(Custom* child, Custom* parent) {
    if (!child || !parent) return nullptr;
    
    // 创建继承后的自定义
    auto inherited = std::make_unique<CustomStyle>(child->name + "_inherited");
    if (parent->type == CustomType::ELEMENT) {
        // inherited = std::make_unique<CustomElement>(child->name + "_inherited");
    } else if (parent->type == CustomType::VAR) {
        // inherited = std::make_unique<CustomVar>(child->name + "_inherited");
    }
    
    // 合并属性
    inherited->properties = parent->properties;
    for (const auto& prop : child->properties) {
        inherited->properties[prop.first] = prop.second;
    }
    
    return inherited;
}

std::unique_ptr<Template> TemplateSystem::processDeletion(Template* target, 
                                                         const std::vector<std::string>& deletions) {
    if (!target) return nullptr;
    
    // 创建删除后的模板
    auto deleted = std::make_unique<StyleTemplate>(target->name + "_deleted");
    if (target->type == TemplateType::ELEMENT) {
        // deleted = std::make_unique<ElementTemplate>(target->name + "_deleted");
    } else if (target->type == TemplateType::VAR) {
        // deleted = std::make_unique<VarTemplate>(target->name + "_deleted");
    }
    
    // 复制属性，排除删除的
    for (const auto& prop : target->properties) {
        if (std::find(deletions.begin(), deletions.end(), prop.first) == deletions.end()) {
            deleted->properties[prop.first] = prop.second;
        }
    }
    
    return deleted;
}

std::unique_ptr<Custom> TemplateSystem::processDeletion(Custom* target, 
                                                       const std::vector<std::string>& deletions) {
    if (!target) return nullptr;
    
    // 创建删除后的自定义
    auto deleted = std::make_unique<CustomStyle>(target->name + "_deleted");
    if (target->type == CustomType::ELEMENT) {
        // deleted = std::make_unique<CustomElement>(target->name + "_deleted");
    } else if (target->type == CustomType::VAR) {
        // deleted = std::make_unique<CustomVar>(target->name + "_deleted");
    }
    
    // 复制属性，排除删除的
    for (const auto& prop : target->properties) {
        if (std::find(deletions.begin(), deletions.end(), prop.first) == deletions.end()) {
            deleted->properties[prop.first] = prop.second;
        }
    }
    
    return deleted;
}

std::unique_ptr<Custom> TemplateSystem::processInsertion(Custom* target, 
                                                        const std::string& position, 
                                                        const std::string& content) {
    if (!target) return nullptr;
    
    // 创建插入后的自定义
    auto inserted = std::make_unique<CustomElement>(target->name + "_inserted");
    if (target->type == CustomType::STYLE) {
        // inserted = std::make_unique<CustomStyle>(target->name + "_inserted");
    } else if (target->type == CustomType::VAR) {
        // inserted = std::make_unique<CustomVar>(target->name + "_inserted");
    }
    
    // 复制原始内容
    inserted->properties = target->properties;
    
    // 根据位置插入内容
    if (position == "after" || position == "before") {
        // 插入逻辑
    }
    
    return inserted;
}

bool TemplateSystem::validateTemplateReference(const std::string& reference) {
    return !reference.empty() && reference.find("@") != std::string::npos;
}

bool TemplateSystem::validateCustomReference(const std::string& reference) {
    return !reference.empty() && reference.find("@") != std::string::npos;
}

bool TemplateSystem::validateNamespaceReference(const std::string& reference) {
    return !reference.empty() && reference.find("from") != std::string::npos;
}

const std::map<std::string, std::unique_ptr<Template>>& TemplateSystem::getAllTemplates() const {
    return templates;
}

const std::map<std::string, std::unique_ptr<Custom>>& TemplateSystem::getAllCustoms() const {
    return customs;
}

const std::map<std::string, std::unique_ptr<Namespace>>& TemplateSystem::getAllNamespaces() const {
    return namespaces;
}

void TemplateSystem::clear() {
    templates.clear();
    customs.clear();
    imports.clear();
    namespaces.clear();
}

// 私有方法实现
std::map<std::string, std::string> TemplateSystem::parseReference(const std::string& reference) {
    std::map<std::string, std::string> result;
    
    // 简单的引用解析，实际需要更复杂的逻辑
    std::regex refRegex(R"((\w+)\s*=\s*(\w+))");
    std::smatch matches;
    
    if (std::regex_search(reference, matches, refRegex)) {
        result[matches[1].str()] = matches[2].str();
    }
    
    return result;
}

std::string TemplateSystem::resolvePath(const std::string& path) {
    // 路径解析逻辑
    return path;
}

std::vector<std::string> TemplateSystem::processWildcardImport(const std::string& pattern) {
    // 通配符导入处理
    return {};
}

std::string TemplateSystem::processModuleUnpack(const std::string& path) {
    // 模块解包处理
    return "";
}

std::vector<std::string> TemplateSystem::resolveDependencies(const ImportInfo& import) {
    // 依赖解析
    return {};
}

bool TemplateSystem::detectCircularDependency(const ImportInfo& import) {
    // 循环依赖检测
    return false;
}

bool TemplateSystem::checkVersionCompatibility(const ImportInfo& import) {
    // 版本兼容性检查
    return true;
}

} // namespace CHTL
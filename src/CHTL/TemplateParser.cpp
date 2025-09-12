#include "CHTL/TemplateParser.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

TemplateParser::TemplateParser() = default;

TemplateParser::~TemplateParser() = default;

std::vector<TemplateInfo> TemplateParser::parseTemplates(const std::string& content) {
    std::vector<TemplateInfo> templates;
    
    // 使用正则表达式查找所有模板定义
    std::regex template_regex(R"(\[Template\]\s+@(\w+)\s+(\w+)\s*\{([^}]+)\})");
    std::sregex_iterator iter(content.begin(), content.end(), template_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string type_str = match[1].str();
        std::string name = match[2].str();
        std::string template_content = match[3].str();
        
        CHTLNode::TemplateType type;
        if (type_str == "Style") {
            type = TemplateType::STYLE;
        } else if (type_str == "Element") {
            type = TemplateType::ELEMENT;
        } else if (type_str == "Var") {
            type = TemplateType::VARIABLE;
        } else {
            continue; // 跳过未知类型
        }
        
        TemplateInfo template_info(type, name, template_content);
        
        // 解析模板内容
        if (type == TemplateType::STYLE) {
            template_info.properties = parseStyleProperties(template_content);
        } else if (type == TemplateType::ELEMENT) {
            template_info.children = parseElementChildren(template_content);
        } else if (type == TemplateType::VARIABLE) {
            template_info.properties = parseVariableProperties(template_content);
        }
        
        templates.push_back(template_info);
        addTemplate(template_info);
    }
    
    return templates;
}

TemplateInfo TemplateParser::parseTemplate(const std::string& template_content) {
    TemplateInfo template_info;
    
    // 查找模板类型和名称
    std::regex header_regex(R"(\[Template\]\s+@(\w+)\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(template_content, match, header_regex)) {
        std::string type_str = match[1].str();
        std::string name = match[2].str();
        
        CHTLNode::TemplateType type;
        if (type_str == "Style") {
            type = TemplateType::STYLE;
        } else if (type_str == "Element") {
            type = TemplateType::ELEMENT;
        } else if (type_str == "Var") {
            type = TemplateType::VARIABLE;
        } else {
            return template_info; // 返回空的模板信息
        }
        
        template_info.type = type;
        template_info.name = name;
        template_info.content = template_content;
        
        // 提取模板内容
        std::regex content_regex(R"(\[Template\]\s+@\w+\s+\w+\s*\{([^}]+)\})");
        std::smatch content_match;
        if (std::regex_search(template_content, content_match, content_regex)) {
            std::string inner_content = content_match[1].str();
            
            if (type == TemplateType::STYLE) {
                template_info.properties = parseStyleProperties(inner_content);
            } else if (type == TemplateType::ELEMENT) {
                template_info.children = parseElementChildren(inner_content);
            } else if (type == TemplateType::VARIABLE) {
                template_info.properties = parseVariableProperties(inner_content);
            }
        }
    }
    
    return template_info;
}

std::string TemplateParser::applyStyleTemplate(const std::string& content, const std::string& template_name) {
    auto it = templates_.find(template_name);
    if (it == templates_.end() || it->second.type != TemplateType::STYLE) {
        return content;
    }
    
    return replaceStyleTemplate(content, it->second);
}

std::string TemplateParser::applyElementTemplate(const std::string& content, const std::string& template_name) {
    auto it = templates_.find(template_name);
    if (it == templates_.end() || it->second.type != TemplateType::ELEMENT) {
        return content;
    }
    
    return replaceElementTemplate(content, it->second);
}

std::string TemplateParser::applyVariableTemplate(const std::string& content, const std::string& template_name) {
    auto it = templates_.find(template_name);
    if (it == templates_.end() || it->second.type != TemplateType::VARIABLE) {
        return content;
    }
    
    return replaceVariableTemplate(content, it->second);
}

void TemplateParser::addTemplate(const TemplateInfo& template_info) {
    templates_[template_info.name] = template_info;
}

void TemplateParser::removeTemplate(const std::string& name) {
    templates_.erase(name);
}

TemplateInfo TemplateParser::getTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    if (it != templates_.end()) {
        return it->second;
    }
    return TemplateInfo();
}

std::vector<std::string> TemplateParser::getTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : templates_) {
        names.push_back(pair.first);
    }
    return names;
}

void TemplateParser::clear() {
    templates_.clear();
}

CHTLNode::TemplateType TemplateParser::parseTemplateType(const std::string& content) {
    if (content.find("@Style") != std::string::npos) {
        return CHTLNode::TemplateType::STYLE;
    } else if (content.find("@Element") != std::string::npos) {
        return CHTLNode::TemplateType::ELEMENT;
    } else if (content.find("@Var") != std::string::npos) {
        return CHTLNode::TemplateType::VARIABLE;
    }
    return CHTLNode::TemplateType::STYLE; // 默认
}

std::string TemplateParser::extractTemplateName(const std::string& content) {
    std::regex name_regex(R"(@\w+\s+(\w+))");
    std::smatch match;
    if (std::regex_search(content, match, name_regex)) {
        return match[1].str();
    }
    return "";
}

std::string TemplateParser::extractTemplateContent(const std::string& content) {
    std::regex content_regex(R"(\{([^}]+)\})");
    std::smatch match;
    if (std::regex_search(content, match, content_regex)) {
        return match[1].str();
    }
    return "";
}

std::map<std::string, std::string> TemplateParser::parseStyleProperties(const std::string& content) {
    std::map<std::string, std::string> properties;
    
    // 简单的CSS属性解析
    std::regex property_regex(R"(([^:;]+):\s*([^;]+);?)");
    std::sregex_iterator iter(content.begin(), content.end(), property_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string key = trim(match[1].str());
        std::string value = trim(match[2].str());
        
        // 移除引号
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        properties[key] = value;
    }
    
    return properties;
}

std::vector<std::shared_ptr<CHTLNode>> TemplateParser::parseElementChildren(const std::string& content) {
    // 简化实现，返回空向量
    // 实际实现需要解析HTML元素
    return std::vector<std::shared_ptr<CHTLNode>>();
}

std::map<std::string, std::string> TemplateParser::parseVariableProperties(const std::string& content) {
    std::map<std::string, std::string> properties;
    
    // 解析变量定义
    std::regex variable_regex(R"((\w+):\s*([^;]+);?)");
    std::sregex_iterator iter(content.begin(), content.end(), variable_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string key = trim(match[1].str());
        std::string value = trim(match[2].str());
        
        // 移除引号
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        properties[key] = value;
    }
    
    return properties;
}

std::string TemplateParser::replaceStyleTemplate(const std::string& content, const TemplateInfo& template_info) {
    std::string result = content;
    
    // 查找 @Style 模板引用
    std::regex style_ref_regex(R"(@Style\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), style_ref_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string ref_name = match[1].str();
        
        if (ref_name == template_info.name) {
            // 生成CSS属性
            std::string css_properties;
            for (const auto& pair : template_info.properties) {
                css_properties += "    " + pair.first + ": " + pair.second + ";\n";
            }
            
            // 替换引用
            result = std::regex_replace(result, std::regex(match[0].str()), css_properties);
        }
    }
    
    return result;
}

std::string TemplateParser::replaceElementTemplate(const std::string& content, const TemplateInfo& template_info) {
    std::string result = content;
    
    // 查找 @Element 模板引用
    std::regex element_ref_regex(R"(@Element\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), element_ref_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string ref_name = match[1].str();
        
        if (ref_name == template_info.name) {
            // 替换为模板内容
            result = std::regex_replace(result, std::regex(match[0].str()), template_info.content);
        }
    }
    
    return result;
}

std::string TemplateParser::replaceVariableTemplate(const std::string& content, const TemplateInfo& template_info) {
    std::string result = content;
    
    // 查找变量引用
    std::regex variable_ref_regex(R"(" + template_info.name + R"\((\w+)\))");
    std::sregex_iterator iter(content.begin(), content.end(), variable_ref_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string var_name = match[1].str();
        
        auto it = template_info.properties.find(var_name);
        if (it != template_info.properties.end()) {
            result = std::regex_replace(result, std::regex(match[0].str()), it->second);
        }
    }
    
    return result;
}

std::string TemplateParser::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> TemplateParser::split(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool TemplateParser::startsWith(const std::string& str, const std::string& prefix) const {
    return str.length() >= prefix.length() && 
           str.substr(0, prefix.length()) == prefix;
}

bool TemplateParser::endsWith(const std::string& str, const std::string& suffix) const {
    return str.length() >= suffix.length() && 
           str.substr(str.length() - suffix.length()) == suffix;
}

} // namespace CHTL
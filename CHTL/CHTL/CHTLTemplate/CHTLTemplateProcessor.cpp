#include "CHTLTemplateProcessor.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <queue>
#include <stack>
#include <set>
#include <unordered_set>
#include <functional>

namespace CHTL {

CHTLTemplateProcessor::CHTLTemplateProcessor() 
    : debugMode(false), strictMode(false), enableCaching(true), enableInheritance(true) {
}

bool CHTLTemplateProcessor::addTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) {
        addError("模板节点为空");
        return false;
    }
    
    std::string templateName = templateNode->getTemplateName();
    if (templateName.empty()) {
        addError("模板名称为空");
        return false;
    }
    
    // 检查模板是否已存在
    if (templateRegistry.find(templateName) != templateRegistry.end()) {
        if (strictMode) {
            addError("模板 '" + templateName + "' 已存在");
            return false;
        } else {
            addWarning("模板 '" + templateName + "' 已存在，将被覆盖");
        }
    }
    
    // 验证模板
    if (!validateTemplate(templateNode)) {
        addError("模板 '" + templateName + "' 验证失败");
        return false;
    }
    
    // 注册模板
    templateRegistry[templateName] = templateNode;
    
    // 处理继承
    if (enableInheritance) {
        processInheritance(templateNode);
    }
    
    // 清除相关缓存
    clearTemplateCache(templateName);
    
    if (debugMode) {
        std::cout << "模板 '" << templateName << "' 已添加" << std::endl;
    }
    
    return true;
}

bool CHTLTemplateProcessor::removeTemplate(const std::string& templateName) {
    auto it = templateRegistry.find(templateName);
    if (it == templateRegistry.end()) {
        addError("模板 '" + templateName + "' 不存在");
        return false;
    }
    
    // 检查是否有其他模板依赖此模板
    auto depIt = templateDependencies.find(templateName);
    if (depIt != templateDependencies.end() && !depIt->second.empty()) {
        addError("无法删除模板 '" + templateName + "'，有其他模板依赖它");
        return false;
    }
    
    // 移除模板
    templateRegistry.erase(it);
    
    // 清除相关缓存
    clearTemplateCache(templateName);
    
    // 更新依赖图
    templateDependencies.erase(templateName);
    inheritanceGraph.erase(templateName);
    reverseInheritanceGraph.erase(templateName);
    
    if (debugMode) {
        std::cout << "模板 '" << templateName << "' 已删除" << std::endl;
    }
    
    return true;
}

bool CHTLTemplateProcessor::updateTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) {
        addError("模板节点为空");
        return false;
    }
    
    std::string templateName = templateNode->getTemplateName();
    if (templateName.empty()) {
        addError("模板名称为空");
        return false;
    }
    
    // 检查模板是否存在
    if (templateRegistry.find(templateName) == templateRegistry.end()) {
        addError("模板 '" + templateName + "' 不存在");
        return false;
    }
    
    // 验证模板
    if (!validateTemplate(templateNode)) {
        addError("模板 '" + templateName + "' 验证失败");
        return false;
    }
    
    // 更新模板
    templateRegistry[templateName] = templateNode;
    
    // 处理继承
    if (enableInheritance) {
        processInheritance(templateNode);
    }
    
    // 清除相关缓存
    clearTemplateCache(templateName);
    
    if (debugMode) {
        std::cout << "模板 '" << templateName << "' 已更新" << std::endl;
    }
    
    return true;
}

std::shared_ptr<TemplateNode> CHTLTemplateProcessor::getTemplate(const std::string& templateName) const {
    auto it = templateRegistry.find(templateName);
    return (it != templateRegistry.end()) ? it->second : nullptr;
}

std::shared_ptr<BaseNode> CHTLTemplateProcessor::instantiate(const std::string& templateName, 
                                                             const std::map<std::string, std::string>& args) {
    if (templateName.empty()) {
        addError("模板名称为空");
        return nullptr;
    }
    
    // 检查缓存
    if (enableCaching && isCached(templateName, args)) {
        std::string cacheKey = getCacheKey(templateName, args);
        return instanceCache[cacheKey];
    }
    
    // 获取模板
    auto templateNode = getTemplate(templateName);
    if (!templateNode) {
        addError("模板 '" + templateName + "' 不存在");
        return nullptr;
    }
    
    // 验证参数
    if (!templateNode->validateParameters(args)) {
        addError("模板 '" + templateName + "' 参数验证失败");
        return nullptr;
    }
    
    // 实例化模板
    std::shared_ptr<BaseNode> instance = nullptr;
    switch (templateNode->getTemplateType()) {
        case TemplateNode::TemplateType::STYLE:
            instance = instantiateStyleTemplate(templateNode, args);
            break;
        case TemplateNode::TemplateType::ELEMENT:
            instance = instantiateElementTemplate(templateNode, args);
            break;
        case TemplateNode::TemplateType::VARIABLE:
            instance = instantiateVariableTemplate(templateNode, args);
            break;
        case TemplateNode::TemplateType::MIXED:
            instance = instantiateElementTemplate(templateNode, args);
            break;
        default:
            addError("未知的模板类型");
            return nullptr;
    }
    
    if (!instance) {
        addError("模板 '" + templateName + "' 实例化失败");
        return nullptr;
    }
    
    // 缓存实例
    if (enableCaching) {
        std::string cacheKey = getCacheKey(templateName, args);
        instanceCache[cacheKey] = instance;
    }
    
    if (debugMode) {
        std::cout << "模板 '" << templateName << "' 实例化成功" << std::endl;
    }
    
    return instance;
}

std::shared_ptr<BaseNode> CHTLTemplateProcessor::instantiateStyleTemplate(const std::string& templateName, 
                                                                          const std::map<std::string, std::string>& args) {
    auto templateNode = getTemplate(templateName);
    if (!templateNode || templateNode->getTemplateType() != TemplateNode::TemplateType::STYLE) {
        addError("样式模板 '" + templateName + "' 不存在或类型不匹配");
        return nullptr;
    }
    
    return instantiateStyleTemplate(templateNode, args);
}

std::shared_ptr<BaseNode> CHTLTemplateProcessor::instantiateElementTemplate(const std::string& templateName, 
                                                                            const std::map<std::string, std::string>& args) {
    auto templateNode = getTemplate(templateName);
    if (!templateNode || templateNode->getTemplateType() != TemplateNode::TemplateType::ELEMENT) {
        addError("元素模板 '" + templateName + "' 不存在或类型不匹配");
        return nullptr;
    }
    
    return instantiateElementTemplate(templateNode, args);
}

std::shared_ptr<BaseNode> CHTLTemplateProcessor::instantiateVariableTemplate(const std::string& templateName, 
                                                                             const std::map<std::string, std::string>& args) {
    auto templateNode = getTemplate(templateName);
    if (!templateNode || templateNode->getTemplateType() != TemplateNode::TemplateType::VARIABLE) {
        addError("变量模板 '" + templateName + "' 不存在或类型不匹配");
        return nullptr;
    }
    
    return instantiateVariableTemplate(templateNode, args);
}

std::shared_ptr<BaseNode> CHTLTemplateProcessor::instantiateStyleTemplate(std::shared_ptr<TemplateNode> templateNode,
                                                                           const std::map<std::string, std::string>& args) const {
    auto styleNode = std::make_shared<StyleNode>(false); // 全局样式
    
    // 处理模板内容
    for (const auto& content : templateNode->getTemplateContent()) {
        if (content->getNodeType() == NodeType::STYLE) {
            auto styleContent = std::static_pointer_cast<StyleNode>(content);
            
            // 复制样式属性
            for (const auto& prop : styleContent->getStyleProperties()) {
                std::string processedValue = parseTemplateString(prop.second, args);
                styleNode->setStyleProperty(prop.first, processedValue);
            }
            
            // 复制样式规则
            for (const auto& rule : styleContent->getStyleRules()) {
                styleNode->addStyleRule(rule->clone());
            }
        }
    }
    
    return styleNode;
}

std::shared_ptr<BaseNode> CHTLTemplateProcessor::instantiateElementTemplate(std::shared_ptr<TemplateNode> templateNode,
                                                                             const std::map<std::string, std::string>& args) const {
    auto elementNode = std::make_shared<ElementNode>("div"); // 默认元素
    
    // 处理模板内容
    for (const auto& content : templateNode->getTemplateContent()) {
        if (content->getNodeType() == NodeType::ELEMENT) {
            auto elementContent = std::static_pointer_cast<ElementNode>(content);
            
            // 复制元素属性
            for (const auto& attr : elementContent->getAttributes()) {
                std::string processedValue = parseTemplateString(attr.second, args);
                elementNode->setAttribute(attr.first, processedValue);
            }
            
            // 复制子节点
            for (const auto& child : elementContent->getChildren()) {
                elementNode->addChild(child->clone());
            }
        }
    }
    
    return elementNode;
}

std::shared_ptr<BaseNode> CHTLTemplateProcessor::instantiateVariableTemplate(std::shared_ptr<TemplateNode> templateNode,
                                                                              const std::map<std::string, std::string>& args) const {
    auto variableNode = std::make_shared<BaseNode>(NodeType::TEXT, "variable");
    
    // 处理模板内容
    for (const auto& content : templateNode->getTemplateContent()) {
        if (content->getNodeType() == NodeType::TEXT) {
            auto textContent = std::static_pointer_cast<TextNode>(content);
            std::string processedValue = parseTemplateString(textContent->getText(), args);
            variableNode->setValue(processedValue);
        }
    }
    
    return variableNode;
}

std::shared_ptr<TemplateNode> CHTLTemplateProcessor::combine(const std::vector<std::string>& templateNames) const {
    if (templateNames.empty()) {
        addError("模板名称列表为空");
        return nullptr;
    }
    
    return combineTemplates(templateNames);
}

std::shared_ptr<TemplateNode> CHTLTemplateProcessor::merge(const std::vector<std::string>& templateNames) const {
    if (templateNames.empty()) {
        addError("模板名称列表为空");
        return nullptr;
    }
    
    return mergeTemplates(templateNames);
}

std::shared_ptr<TemplateNode> CHTLTemplateProcessor::combineTemplates(const std::vector<std::string>& templateNames) const {
    auto combined = std::make_shared<TemplateNode>("combined_template", TemplateNode::TemplateType::MIXED);
    
    for (const auto& templateName : templateNames) {
        auto templateNode = getTemplate(templateName);
        if (!templateNode) {
            addError("模板 '" + templateName + "' 不存在");
            continue;
        }
        
        // 合并参数
        for (const auto& param : templateNode->getParameters()) {
            combined->setParameter(param.first, param.second);
        }
        
        // 合并内容
        for (const auto& content : templateNode->getTemplateContent()) {
            combined->addTemplateContent(content->clone());
        }
        
        // 合并约束
        for (const auto& constraint : templateNode->getConstraints()) {
            combined->addConstraint(constraint);
        }
    }
    
    return combined;
}

std::shared_ptr<TemplateNode> CHTLTemplateProcessor::mergeTemplates(const std::vector<std::string>& templateNames) const {
    auto merged = std::make_shared<TemplateNode>("merged_template", TemplateNode::TemplateType::MIXED);
    
    for (const auto& templateName : templateNames) {
        auto templateNode = getTemplate(templateName);
        if (!templateNode) {
            addError("模板 '" + templateName + "' 不存在");
            continue;
        }
        
        // 合并参数（后面的模板参数优先）
        for (const auto& param : templateNode->getParameters()) {
            merged->setParameter(param.first, param.second);
        }
        
        // 合并内容（后面的模板内容优先）
        for (const auto& content : templateNode->getTemplateContent()) {
            merged->addTemplateContent(content->clone());
        }
        
        // 合并约束
        for (const auto& constraint : templateNode->getConstraints()) {
            merged->addConstraint(constraint);
        }
    }
    
    return merged;
}

bool CHTLTemplateProcessor::addInheritance(const std::string& childTemplate, const std::string& parentTemplate) {
    if (childTemplate.empty() || parentTemplate.empty()) {
        addError("子模板或父模板名称为空");
        return false;
    }
    
    if (childTemplate == parentTemplate) {
        addError("子模板和父模板不能相同");
        return false;
    }
    
    // 检查模板是否存在
    if (!getTemplate(childTemplate)) {
        addError("子模板 '" + childTemplate + "' 不存在");
        return false;
    }
    
    if (!getTemplate(parentTemplate)) {
        addError("父模板 '" + parentTemplate + "' 不存在");
        return false;
    }
    
    // 检查循环依赖
    if (hasCircularDependency(childTemplate)) {
        addError("检测到循环依赖");
        return false;
    }
    
    // 添加继承关系
    inheritanceGraph[childTemplate].push_back(parentTemplate);
    reverseInheritanceGraph[parentTemplate].push_back(childTemplate);
    
    // 处理继承
    auto childTemplateNode = getTemplate(childTemplate);
    if (childTemplateNode) {
        processInheritance(childTemplateNode);
    }
    
    if (debugMode) {
        std::cout << "模板 '" << childTemplate << "' 继承自 '" << parentTemplate << "'" << std::endl;
    }
    
    return true;
}

bool CHTLTemplateProcessor::removeInheritance(const std::string& childTemplate, const std::string& parentTemplate) {
    if (childTemplate.empty() || parentTemplate.empty()) {
        addError("子模板或父模板名称为空");
        return false;
    }
    
    // 移除继承关系
    auto it = inheritanceGraph.find(childTemplate);
    if (it != inheritanceGraph.end()) {
        auto& parents = it->second;
        auto parentIt = std::find(parents.begin(), parents.end(), parentTemplate);
        if (parentIt != parents.end()) {
            parents.erase(parentIt);
        }
    }
    
    auto revIt = reverseInheritanceGraph.find(parentTemplate);
    if (revIt != reverseInheritanceGraph.end()) {
        auto& children = revIt->second;
        auto childIt = std::find(children.begin(), children.end(), childTemplate);
        if (childIt != children.end()) {
            children.erase(childIt);
        }
    }
    
    if (debugMode) {
        std::cout << "模板 '" << childTemplate << "' 不再继承自 '" << parentTemplate << "'" << std::endl;
    }
    
    return true;
}

std::vector<std::string> CHTLTemplateProcessor::getParentTemplates(const std::string& templateName) const {
    auto it = inheritanceGraph.find(templateName);
    return (it != inheritanceGraph.end()) ? it->second : std::vector<std::string>();
}

std::vector<std::string> CHTLTemplateProcessor::getChildTemplates(const std::string& templateName) const {
    auto it = reverseInheritanceGraph.find(templateName);
    return (it != reverseInheritanceGraph.end()) ? it->second : std::vector<std::string>();
}

bool CHTLTemplateProcessor::validateAllTemplates() const {
    bool allValid = true;
    
    for (const auto& pair : templateRegistry) {
        if (!validateTemplate(pair.second)) {
            allValid = false;
        }
    }
    
    return allValid;
}

bool CHTLTemplateProcessor::validateTemplate(const std::string& templateName) const {
    auto templateNode = getTemplate(templateName);
    if (!templateNode) {
        addError("模板 '" + templateName + "' 不存在");
        return false;
    }
    
    return validateTemplate(templateNode);
}

bool CHTLTemplateProcessor::validateTemplate(std::shared_ptr<TemplateNode> templateNode) const {
    if (!templateNode) {
        return false;
    }
    
    // 检查模板名称
    if (templateNode->getTemplateName().empty()) {
        addError("模板名称为空");
        return false;
    }
    
    // 检查模板内容
    if (templateNode->getTemplateContent().empty()) {
        addError("模板内容为空");
        return false;
    }
    
    // 检查约束
    if (!templateNode->validateConstraints()) {
        addError("模板约束验证失败");
        return false;
    }
    
    return true;
}

bool CHTLTemplateProcessor::validateInheritance() const {
    // 检查循环依赖
    for (const auto& pair : templateRegistry) {
        if (hasCircularDependency(pair.first)) {
            addError("检测到循环依赖: " + pair.first);
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> CHTLTemplateProcessor::searchTemplates(const std::string& pattern) const {
    std::vector<std::string> results;
    std::regex regexPattern(pattern, std::regex_constants::icase);
    
    for (const auto& pair : templateRegistry) {
        if (std::regex_search(pair.first, regexPattern)) {
            results.push_back(pair.first);
        }
    }
    
    return results;
}

std::vector<std::string> CHTLTemplateProcessor::searchTemplatesByType(TemplateNode::TemplateType type) const {
    std::vector<std::string> results;
    
    for (const auto& pair : templateRegistry) {
        if (pair.second->getTemplateType() == type) {
            results.push_back(pair.first);
        }
    }
    
    return results;
}

std::vector<std::string> CHTLTemplateProcessor::searchTemplatesByTag(const std::string& tag) const {
    std::vector<std::string> results;
    
    for (const auto& pair : templateRegistry) {
        if (pair.second->hasMetadata("tag") && pair.second->getMetadata("tag") == tag) {
            results.push_back(pair.first);
        }
    }
    
    return results;
}

void CHTLTemplateProcessor::optimizeAllTemplates() {
    for (const auto& pair : templateRegistry) {
        optimizeTemplate(pair.second);
    }
}

void CHTLTemplateProcessor::optimizeTemplate(const std::string& templateName) {
    auto templateNode = getTemplate(templateName);
    if (templateNode) {
        optimizeTemplate(templateNode);
    }
}

void CHTLTemplateProcessor::optimizeTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) {
        return;
    }
    
    // 移除未使用的参数
    removeUnusedParameters(templateNode);
    
    // 合并重复内容
    mergeDuplicateContent(templateNode);
}

std::string CHTLTemplateProcessor::exportTemplate(const std::string& templateName) const {
    auto templateNode = getTemplate(templateName);
    if (!templateNode) {
        addError("模板 '" + templateName + "' 不存在");
        return "";
    }
    
    return templateNode->toTemplateString();
}

std::string CHTLTemplateProcessor::exportAllTemplates() const {
    std::ostringstream oss;
    
    for (const auto& pair : templateRegistry) {
        oss << pair.second->toTemplateString() << "\n\n";
    }
    
    return oss.str();
}

bool CHTLTemplateProcessor::importTemplate(const std::string& templateData) {
    // 简化的模板导入实现
    // 这里需要解析模板数据并创建模板节点
    addError("模板导入功能尚未实现");
    return false;
}

bool CHTLTemplateProcessor::importTemplates(const std::string& templatesData) {
    // 简化的模板导入实现
    // 这里需要解析模板数据并创建模板节点
    addError("模板导入功能尚未实现");
    return false;
}

std::vector<std::string> CHTLTemplateProcessor::getAllTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : templateRegistry) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLTemplateProcessor::getTemplateNamesByType(TemplateNode::TemplateType type) const {
    return searchTemplatesByType(type);
}

size_t CHTLTemplateProcessor::getTemplateCount() const {
    return templateRegistry.size();
}

size_t CHTLTemplateProcessor::getTemplateCountByType(TemplateNode::TemplateType type) const {
    return searchTemplatesByType(type).size();
}

void CHTLTemplateProcessor::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLTemplateProcessor::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLTemplateProcessor::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLTemplateProcessor::reset() {
    templateRegistry.clear();
    templateDependencies.clear();
    inheritanceGraph.clear();
    reverseInheritanceGraph.clear();
    instanceCache.clear();
    clearMessages();
}

std::string CHTLTemplateProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL模板处理器调试信息:\n";
    oss << "调试模式: " << (debugMode ? "开启" : "关闭") << "\n";
    oss << "严格模式: " << (strictMode ? "开启" : "关闭") << "\n";
    oss << "缓存启用: " << (enableCaching ? "开启" : "关闭") << "\n";
    oss << "继承启用: " << (enableInheritance ? "开启" : "关闭") << "\n";
    oss << "模板数量: " << templateRegistry.size() << "\n";
    oss << "继承关系: " << inheritanceGraph.size() << "\n";
    oss << "缓存大小: " << instanceCache.size() << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

std::string CHTLTemplateProcessor::getTemplateInfo(const std::string& templateName) const {
    auto templateNode = getTemplate(templateName);
    if (!templateNode) {
        return "模板 '" + templateName + "' 不存在";
    }
    
    return templateNode->getDebugInfo();
}

std::string CHTLTemplateProcessor::getInheritanceGraph() const {
    std::ostringstream oss;
    oss << "继承关系图:\n";
    
    for (const auto& pair : inheritanceGraph) {
        oss << pair.first << " -> ";
        for (size_t i = 0; i < pair.second.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << pair.second[i];
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string CHTLTemplateProcessor::getDependencyGraph() const {
    std::ostringstream oss;
    oss << "依赖关系图:\n";
    
    for (const auto& pair : templateDependencies) {
        oss << pair.first << " -> ";
        for (size_t i = 0; i < pair.second.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << pair.second[i];
        }
        oss << "\n";
    }
    
    return oss.str();
}

// 私有方法实现
void CHTLTemplateProcessor::processInheritance(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) {
        return;
    }
    
    mergeParentTemplates(templateNode);
    resolveInheritanceConflicts(templateNode);
}

void CHTLTemplateProcessor::mergeParentTemplates(std::shared_ptr<TemplateNode> templateNode) {
    std::string templateName = templateNode->getTemplateName();
    auto it = inheritanceGraph.find(templateName);
    if (it == inheritanceGraph.end()) {
        return;
    }
    
    for (const auto& parentName : it->second) {
        auto parentTemplate = getTemplate(parentName);
        if (parentTemplate) {
            // 合并父模板的内容
            for (const auto& content : parentTemplate->getTemplateContent()) {
                templateNode->addTemplateContent(content->clone());
            }
            
            // 合并父模板的参数
            for (const auto& param : parentTemplate->getParameters()) {
                if (!templateNode->hasParameter(param.first)) {
                    templateNode->setParameter(param.first, param.second);
                }
            }
            
            // 合并父模板的约束
            for (const auto& constraint : parentTemplate->getConstraints()) {
                templateNode->addConstraint(constraint);
            }
        }
    }
}

void CHTLTemplateProcessor::resolveInheritanceConflicts(std::shared_ptr<TemplateNode> templateNode) {
    // 解决继承冲突
    // 简化实现：子模板的属性优先
}

bool CHTLTemplateProcessor::hasCircularDependency(const std::string& templateName) const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    
    std::function<bool(const std::string&)> hasCycle = [&](const std::string& name) -> bool {
        if (recursionStack.find(name) != recursionStack.end()) {
            return true;
        }
        
        if (visited.find(name) != visited.end()) {
            return false;
        }
        
        visited.insert(name);
        recursionStack.insert(name);
        
        auto it = inheritanceGraph.find(name);
        if (it != inheritanceGraph.end()) {
            for (const auto& parent : it->second) {
                if (hasCycle(parent)) {
                    return true;
                }
            }
        }
        
        recursionStack.erase(name);
        return false;
    };
    
    return hasCycle(templateName);
}

std::string CHTLTemplateProcessor::parseTemplateString(const std::string& templateStr, 
                                                       const std::map<std::string, std::string>& args) const {
    std::string result = templateStr;
    
    // 替换参数占位符
    for (const auto& arg : args) {
        std::string placeholder = "{{" + arg.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), arg.second);
            pos += arg.second.length();
        }
    }
    
    return result;
}

std::vector<std::string> CHTLTemplateProcessor::extractPlaceholders(const std::string& templateStr) const {
    std::vector<std::string> placeholders;
    std::regex placeholderRegex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator begin(templateStr.begin(), templateStr.end(), placeholderRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        placeholders.push_back(it->str(1));
    }
    
    return placeholders;
}

std::string CHTLTemplateProcessor::replacePlaceholders(const std::string& templateStr, 
                                                       const std::map<std::string, std::string>& replacements) const {
    return parseTemplateString(templateStr, replacements);
}

void CHTLTemplateProcessor::optimizeTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) {
        return;
    }
    
    // 移除未使用的参数
    removeUnusedParameters(templateNode);
    
    // 合并重复内容
    mergeDuplicateContent(templateNode);
}

void CHTLTemplateProcessor::removeUnusedParameters(std::shared_ptr<TemplateNode> templateNode) {
    // 简化实现：移除未使用的参数
    // 这里需要分析模板内容中使用的参数
}

void CHTLTemplateProcessor::mergeDuplicateContent(std::shared_ptr<TemplateNode> templateNode) {
    // 简化实现：合并重复内容
    // 这里需要分析模板内容中的重复项
}

void CHTLTemplateProcessor::clearCache() {
    instanceCache.clear();
}

void CHTLTemplateProcessor::clearTemplateCache(const std::string& templateName) {
    // 清除特定模板的缓存
    auto it = instanceCache.begin();
    while (it != instanceCache.end()) {
        if (it->first.find(templateName) != std::string::npos) {
            it = instanceCache.erase(it);
        } else {
            ++it;
        }
    }
}

bool CHTLTemplateProcessor::isCached(const std::string& templateName, const std::map<std::string, std::string>& args) const {
    std::string cacheKey = getCacheKey(templateName, args);
    return instanceCache.find(cacheKey) != instanceCache.end();
}

std::string CHTLTemplateProcessor::getCacheKey(const std::string& templateName, const std::map<std::string, std::string>& args) const {
    std::ostringstream oss;
    oss << templateName;
    
    for (const auto& arg : args) {
        oss << "|" << arg.first << "=" << arg.second;
    }
    
    return oss.str();
}

std::vector<std::string> CHTLTemplateProcessor::getTemplateDependencies(const std::string& templateName) const {
    auto it = templateDependencies.find(templateName);
    return (it != templateDependencies.end()) ? it->second : std::vector<std::string>();
}

std::vector<std::string> CHTLTemplateProcessor::getDependentTemplates(const std::string& templateName) const {
    auto it = reverseInheritanceGraph.find(templateName);
    return (it != reverseInheritanceGraph.end()) ? it->second : std::vector<std::string>();
}

} // namespace CHTL
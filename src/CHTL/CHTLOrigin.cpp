#include "CHTL/CHTLOrigin.h"
#include <sstream>
#include <regex>

namespace CHTL {

// OriginContent 实现
OriginContent::OriginContent(OriginType type, const std::string& content, 
                            const std::string& name, int line, int column)
    : m_type(type), m_content(content), m_name(name), m_line(line), m_column(column) {
}

std::string OriginContent::processContent() const {
    // 根据类型处理内容
    switch (m_type) {
        case OriginType::HTML:
            return m_content; // HTML 内容直接返回
        case OriginType::CSS:
            return m_content; // CSS 内容直接返回
        case OriginType::JAVASCRIPT:
            return m_content; // JavaScript 内容直接返回
        case OriginType::CUSTOM:
            return m_content; // 自定义内容直接返回
        default:
            return m_content;
    }
}

bool OriginContent::isValid() const {
    return !m_content.empty();
}

// OriginManager 实现
OriginManager::OriginManager(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void OriginManager::registerOrigin(std::shared_ptr<OriginContent> origin) {
    if (origin) {
        registerOrigin(origin->getName(), origin);
    }
}

void OriginManager::registerOrigin(const std::string& name, std::shared_ptr<OriginContent> origin) {
    if (origin) {
        m_origins[name] = origin;
        m_originsByType[origin->getType()].push_back(origin);
    }
}

std::shared_ptr<OriginContent> OriginManager::getOrigin(const std::string& name) const {
    auto it = m_origins.find(name);
    return (it != m_origins.end()) ? it->second : nullptr;
}

std::shared_ptr<OriginContent> OriginManager::getOrigin(const std::string& name, OriginType type) const {
    auto origin = getOrigin(name);
    if (origin && origin->getType() == type) {
        return origin;
    }
    return nullptr;
}

std::vector<std::shared_ptr<OriginContent>> OriginManager::getOrigins(OriginType type) const {
    auto it = m_originsByType.find(type);
    return (it != m_originsByType.end()) ? it->second : std::vector<std::shared_ptr<OriginContent>>();
}

std::vector<std::shared_ptr<OriginContent>> OriginManager::getAllOrigins() const {
    std::vector<std::shared_ptr<OriginContent>> allOrigins;
    for (const auto& [name, origin] : m_origins) {
        allOrigins.push_back(origin);
    }
    return allOrigins;
}

std::string OriginManager::processOrigins(OriginType type) const {
    switch (type) {
        case OriginType::HTML:
            return processHtmlOrigins();
        case OriginType::CSS:
            return processCssOrigins();
        case OriginType::JAVASCRIPT:
            return processJavaScriptOrigins();
        case OriginType::CUSTOM:
            return processCustomOrigins();
        default:
            return "";
    }
}

std::string OriginManager::processAllOrigins() const {
    std::stringstream ss;
    
    ss << processHtmlOrigins() << "\n";
    ss << processCssOrigins() << "\n";
    ss << processJavaScriptOrigins() << "\n";
    ss << processCustomOrigins() << "\n";
    
    return ss.str();
}

void OriginManager::clear() {
    m_origins.clear();
    m_originsByType.clear();
}

void OriginManager::removeOrigin(const std::string& name) {
    auto origin = getOrigin(name);
    if (origin) {
        m_origins.erase(name);
        
        auto& typeOrigins = m_originsByType[origin->getType()];
        typeOrigins.erase(
            std::remove(typeOrigins.begin(), typeOrigins.end(), origin),
            typeOrigins.end()
        );
    }
}

std::string OriginManager::processHtmlOrigins() const {
    std::stringstream ss;
    
    auto htmlOrigins = getOrigins(OriginType::HTML);
    for (const auto& origin : htmlOrigins) {
        if (origin->isValid()) {
            ss << origin->processContent() << "\n";
        }
    }
    
    return ss.str();
}

std::string OriginManager::processCssOrigins() const {
    std::stringstream ss;
    
    auto cssOrigins = getOrigins(OriginType::CSS);
    for (const auto& origin : cssOrigins) {
        if (origin->isValid()) {
            ss << origin->processContent() << "\n";
        }
    }
    
    return ss.str();
}

std::string OriginManager::processJavaScriptOrigins() const {
    std::stringstream ss;
    
    auto jsOrigins = getOrigins(OriginType::JAVASCRIPT);
    for (const auto& origin : jsOrigins) {
        if (origin->isValid()) {
            ss << origin->processContent() << "\n";
        }
    }
    
    return ss.str();
}

std::string OriginManager::processCustomOrigins() const {
    std::stringstream ss;
    
    auto customOrigins = getOrigins(OriginType::CUSTOM);
    for (const auto& origin : customOrigins) {
        if (origin->isValid()) {
            ss << origin->processContent() << "\n";
        }
    }
    
    return ss.str();
}

// OriginParser 实现
OriginParser::OriginParser(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::shared_ptr<OriginContent> OriginParser::parseOrigin(std::shared_ptr<OriginNode> node) {
    if (!node) {
        return nullptr;
    }
    
    OriginType type = parseOriginType(node->getOriginType());
    std::string content = extractContent(node);
    std::string name = node->getName();
    
    return std::make_shared<OriginContent>(type, content, name, 
                                          node->getLine(), node->getColumn());
}

std::vector<std::shared_ptr<OriginContent>> OriginParser::parseOrigins(std::shared_ptr<BaseNode> root) {
    std::vector<std::shared_ptr<OriginContent>> origins;
    
    if (!root) {
        return origins;
    }
    
    // 递归查找原始嵌入节点
    std::function<void(std::shared_ptr<BaseNode>)> findOrigins = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::ORIGIN) {
                auto originNode = std::static_pointer_cast<OriginNode>(node);
                auto origin = parseOrigin(originNode);
                if (origin) {
                    origins.push_back(origin);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findOrigins(child);
            }
        };
    
    findOrigins(root);
    return origins;
}

std::shared_ptr<OriginContent> OriginParser::parseNamedOrigin(std::shared_ptr<OriginNode> node) {
    if (!node || node->getName().empty()) {
        return nullptr;
    }
    
    return parseOrigin(node);
}

OriginType OriginParser::parseOriginType(const std::string& typeString) const {
    if (typeString == "@Html") {
        return OriginType::HTML;
    } else if (typeString == "@Style") {
        return OriginType::CSS;
    } else if (typeString == "@JavaScript") {
        return OriginType::JAVASCRIPT;
    } else {
        return OriginType::CUSTOM;
    }
}

std::string OriginParser::extractContent(std::shared_ptr<OriginNode> node) const {
    if (node->hasAttribute("content")) {
        return node->getAttribute("content");
    }
    
    // 从子节点提取内容
    std::stringstream ss;
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            ss << textNode->getContent();
        }
    }
    
    return ss.str();
}

// OriginGenerator 实现
OriginGenerator::OriginGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string OriginGenerator::generate(std::shared_ptr<OriginContent> origin) {
    if (!origin || !origin->isValid()) {
        return "";
    }
    
    switch (origin->getType()) {
        case OriginType::HTML:
            return generateHtmlContent(origin->getContent());
        case OriginType::CSS:
            return generateCssContent(origin->getContent());
        case OriginType::JAVASCRIPT:
            return generateJavaScriptContent(origin->getContent());
        case OriginType::CUSTOM:
            return generateCustomContent(origin->getContent(), origin->getName());
        default:
            return origin->getContent();
    }
}

std::string OriginGenerator::generateAll(const std::vector<std::shared_ptr<OriginContent>>& origins) {
    std::stringstream ss;
    
    for (const auto& origin : origins) {
        std::string content = generate(origin);
        if (!content.empty()) {
            ss << content << "\n";
        }
    }
    
    return ss.str();
}

std::string OriginGenerator::generateByType(OriginType type) const {
    // 这里需要从管理器中获取指定类型的原始嵌入
    // 暂时返回空字符串
    return "";
}

std::string OriginGenerator::generateHtmlWrapper(const std::string& content) const {
    return content; // HTML 内容不需要额外包装
}

std::string OriginGenerator::generateCssWrapper(const std::string& content) const {
    return "<style>\n" + content + "\n</style>";
}

std::string OriginGenerator::generateJavaScriptWrapper(const std::string& content) const {
    return "<script>\n" + content + "\n</script>";
}

std::string OriginGenerator::generateHtmlContent(const std::string& content) const {
    return content;
}

std::string OriginGenerator::generateCssContent(const std::string& content) const {
    return content;
}

std::string OriginGenerator::generateJavaScriptContent(const std::string& content) const {
    return content;
}

std::string OriginGenerator::generateCustomContent(const std::string& content, const std::string& type) const {
    return "<!-- Custom " + type + " content -->\n" + content;
}

// OriginProcessor 实现
OriginProcessor::OriginProcessor(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    m_manager = std::make_unique<OriginManager>(context);
    m_parser = std::make_unique<OriginParser>(context);
    m_generator = std::make_unique<OriginGenerator>(context);
}

void OriginProcessor::processOrigins(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 解析原始嵌入
    auto origins = m_parser->parseOrigins(root);
    
    // 注册到管理器
    for (const auto& origin : origins) {
        m_manager->registerOrigin(origin);
    }
    
    // 验证内容
    for (const auto& origin : origins) {
        if (!validateContent(origin->getContent(), origin->getType())) {
            m_context->addWarning("Invalid content in origin: " + origin->getName());
        }
    }
}

bool OriginProcessor::validateContent(const std::string& content, OriginType type) const {
    switch (type) {
        case OriginType::HTML:
            return validateHtmlContent(content);
        case OriginType::CSS:
            return validateCssContent(content);
        case OriginType::JAVASCRIPT:
            return validateJavaScriptContent(content);
        case OriginType::CUSTOM:
            return validateCustomContent(content, "");
        default:
            return true;
    }
}

std::string OriginProcessor::cleanContent(const std::string& content, OriginType type) const {
    switch (type) {
        case OriginType::HTML:
            return cleanHtmlContent(content);
        case OriginType::CSS:
            return cleanCssContent(content);
        case OriginType::JAVASCRIPT:
            return cleanJavaScriptContent(content);
        case OriginType::CUSTOM:
            return cleanCustomContent(content, "");
        default:
            return content;
    }
}

bool OriginProcessor::validateHtmlContent(const std::string& content) const {
    // 简单的 HTML 验证
    return !content.empty();
}

bool OriginProcessor::validateCssContent(const std::string& content) const {
    // 简单的 CSS 验证
    return !content.empty();
}

bool OriginProcessor::validateJavaScriptContent(const std::string& content) const {
    // 简单的 JavaScript 验证
    return !content.empty();
}

bool OriginProcessor::validateCustomContent(const std::string& content, const std::string& type) const {
    // 自定义内容验证
    return !content.empty();
}

std::string OriginProcessor::cleanHtmlContent(const std::string& content) const {
    // 清理 HTML 内容
    return content;
}

std::string OriginProcessor::cleanCssContent(const std::string& content) const {
    // 清理 CSS 内容
    return content;
}

std::string OriginProcessor::cleanJavaScriptContent(const std::string& content) const {
    // 清理 JavaScript 内容
    return content;
}

std::string OriginProcessor::cleanCustomContent(const std::string& content, const std::string& type) const {
    // 清理自定义内容
    return content;
}

} // namespace CHTL
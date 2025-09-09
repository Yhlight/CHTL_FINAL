#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief 原始嵌入类型枚举
 */
enum class OriginType {
    HTML,           // HTML 代码
    CSS,            // CSS 代码
    JAVASCRIPT,     // JavaScript 代码
    CUSTOM          // 自定义类型
};

/**
 * @brief 原始嵌入内容
 */
class OriginContent {
public:
    OriginContent(OriginType type, const std::string& content, 
                  const std::string& name = "", int line = 0, int column = 0);
    ~OriginContent() = default;

    OriginType getType() const { return m_type; }
    std::string getContent() const { return m_content; }
    std::string getName() const { return m_name; }
    int getLine() const { return m_line; }
    int getColumn() const { return m_column; }
    
    void setContent(const std::string& content) { m_content = content; }
    void setName(const std::string& name) { m_name = name; }
    
    // 内容处理
    std::string processContent() const;
    bool isValid() const;

private:
    OriginType m_type;
    std::string m_content;
    std::string m_name;
    int m_line;
    int m_column;
};

/**
 * @brief 原始嵌入管理器
 */
class OriginManager {
public:
    explicit OriginManager(std::shared_ptr<CHTLContext> context);
    ~OriginManager() = default;

    // 原始嵌入注册
    void registerOrigin(std::shared_ptr<OriginContent> origin);
    void registerOrigin(const std::string& name, std::shared_ptr<OriginContent> origin);
    
    // 原始嵌入查找
    std::shared_ptr<OriginContent> getOrigin(const std::string& name) const;
    std::shared_ptr<OriginContent> getOrigin(const std::string& name, OriginType type) const;
    
    // 原始嵌入列表
    std::vector<std::shared_ptr<OriginContent>> getOrigins(OriginType type) const;
    std::vector<std::shared_ptr<OriginContent>> getAllOrigins() const;
    
    // 内容处理
    std::string processOrigins(OriginType type) const;
    std::string processAllOrigins() const;
    
    // 清理
    void clear();
    void removeOrigin(const std::string& name);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<OriginContent>> m_origins;
    std::unordered_map<OriginType, std::vector<std::shared_ptr<OriginContent>>> m_originsByType;
    
    // 辅助方法
    std::string processHtmlOrigins() const;
    std::string processCssOrigins() const;
    std::string processJavaScriptOrigins() const;
    std::string processCustomOrigins() const;
};

/**
 * @brief 原始嵌入解析器
 */
class OriginParser {
public:
    explicit OriginParser(std::shared_ptr<CHTLContext> context);
    ~OriginParser() = default;

    // 解析原始嵌入
    std::shared_ptr<OriginContent> parseOrigin(std::shared_ptr<OriginNode> node);
    std::vector<std::shared_ptr<OriginContent>> parseOrigins(std::shared_ptr<BaseNode> root);
    
    // 解析命名原始嵌入
    std::shared_ptr<OriginContent> parseNamedOrigin(std::shared_ptr<OriginNode> node);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    OriginType parseOriginType(const std::string& typeString) const;
    std::string extractContent(std::shared_ptr<OriginNode> node) const;
};

/**
 * @brief 原始嵌入生成器
 */
class OriginGenerator {
public:
    explicit OriginGenerator(std::shared_ptr<CHTLContext> context);
    ~OriginGenerator() = default;

    // 生成代码
    std::string generate(std::shared_ptr<OriginContent> origin);
    std::string generateAll(const std::vector<std::shared_ptr<OriginContent>>& origins);
    std::string generateByType(OriginType type) const;
    
    // 生成 HTML 包装
    std::string generateHtmlWrapper(const std::string& content) const;
    std::string generateCssWrapper(const std::string& content) const;
    std::string generateJavaScriptWrapper(const std::string& content) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::string generateHtmlContent(const std::string& content) const;
    std::string generateCssContent(const std::string& content) const;
    std::string generateJavaScriptContent(const std::string& content) const;
    std::string generateCustomContent(const std::string& content, const std::string& type) const;
};

/**
 * @brief 原始嵌入处理器
 */
class OriginProcessor {
public:
    explicit OriginProcessor(std::shared_ptr<CHTLContext> context);
    ~OriginProcessor() = default;

    // 处理原始嵌入
    void processOrigins(std::shared_ptr<BaseNode> root);
    
    // 内容验证
    bool validateContent(const std::string& content, OriginType type) const;
    
    // 内容清理
    std::string cleanContent(const std::string& content, OriginType type) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<OriginManager> m_manager;
    std::unique_ptr<OriginParser> m_parser;
    std::unique_ptr<OriginGenerator> m_generator;
    
    // 辅助方法
    bool validateHtmlContent(const std::string& content) const;
    bool validateCssContent(const std::string& content) const;
    bool validateJavaScriptContent(const std::string& content) const;
    bool validateCustomContent(const std::string& content, const std::string& type) const;
    
    std::string cleanHtmlContent(const std::string& content) const;
    std::string cleanCssContent(const std::string& content) const;
    std::string cleanJavaScriptContent(const std::string& content) const;
    std::string cleanCustomContent(const std::string& content, const std::string& type) const;
};

} // namespace CHTL
#ifndef CHTL_ORIGIN_NODE_H
#define CHTL_ORIGIN_NODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

class OriginNode : public BaseNode {
private:
    // 原始嵌入类型
    enum class OriginType {
        HTML,       // HTML原始嵌入
        STYLE,      // CSS原始嵌入
        JAVASCRIPT, // JavaScript原始嵌入
        TEXT,       // 纯文本原始嵌入
        MIXED       // 混合原始嵌入
    };
    
    OriginType originType;
    std::string originName;
    std::string originContent;
    std::map<std::string, std::string> originAttributes;
    
    // 原始嵌入元数据
    std::map<std::string, std::string> metadata;
    bool isNamed;
    bool isProcessed;
    int priority;
    
    // 原始嵌入约束
    std::vector<std::string> constraints;
    std::map<std::string, std::string> typeConstraints;
    
    // 原始嵌入处理
    std::string processedContent;
    std::vector<std::string> processingErrors;
    std::vector<std::string> processingWarnings;

public:
    OriginNode(const std::string& name = "", OriginType type = OriginType::HTML);
    virtual ~OriginNode() = default;
    
    // 原始嵌入类型
    OriginType getOriginType() const { return originType; }
    void setOriginType(OriginType type) { originType = type; }
    std::string getOriginTypeName() const;
    
    // 原始嵌入名称
    const std::string& getOriginName() const { return originName; }
    void setOriginName(const std::string& name) { originName = name; }
    
    // 原始嵌入内容
    const std::string& getOriginContent() const { return originContent; }
    void setOriginContent(const std::string& content) { originContent = content; }
    
    // 原始嵌入属性
    void setOriginAttribute(const std::string& name, const std::string& value);
    std::string getOriginAttribute(const std::string& name) const;
    bool hasOriginAttribute(const std::string& name) const;
    void removeOriginAttribute(const std::string& name);
    const std::map<std::string, std::string>& getOriginAttributes() const { return originAttributes; }
    
    // 原始嵌入元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    
    // 原始嵌入特性
    bool isNamedOrigin() const { return isNamed; }
    void setNamed(bool named) { isNamed = named; }
    
    bool isProcessedOrigin() const { return isProcessed; }
    void setProcessed(bool processed) { isProcessed = processed; }
    
    int getPriority() const { return priority; }
    void setPriority(int pri) { priority = pri; }
    
    // 原始嵌入约束
    void addConstraint(const std::string& constraint);
    void removeConstraint(const std::string& constraint);
    const std::vector<std::string>& getConstraints() const { return constraints; }
    bool hasConstraint(const std::string& constraint) const;
    
    void setTypeConstraint(const std::string& type, const std::string& constraint);
    std::string getTypeConstraint(const std::string& type) const;
    bool hasTypeConstraint(const std::string& type) const;
    void removeTypeConstraint(const std::string& type);
    const std::map<std::string, std::string>& getTypeConstraints() const { return typeConstraints; }
    
    // 原始嵌入处理
    void processOrigin();
    void processHtmlOrigin();
    void processStyleOrigin();
    void processJavaScriptOrigin();
    void processTextOrigin();
    void processMixedOrigin();
    
    const std::string& getProcessedContent() const { return processedContent; }
    void setProcessedContent(const std::string& content) { processedContent = content; }
    
    // 处理错误和警告
    const std::vector<std::string>& getProcessingErrors() const { return processingErrors; }
    const std::vector<std::string>& getProcessingWarnings() const { return processingWarnings; }
    void addProcessingError(const std::string& error);
    void addProcessingWarning(const std::string& warning);
    void clearProcessingMessages();
    
    // 原始嵌入验证
    bool validateOrigin() const;
    bool validateContent() const;
    bool validateConstraints() const;
    bool validateType() const;
    
    // 原始嵌入转换
    std::string toHTML() const override;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toText() const;
    std::string toOriginString() const;
    
    // 原始嵌入格式化
    std::string formatContent() const;
    std::string minifyContent() const;
    std::string beautifyContent() const;
    
    // 原始嵌入压缩
    std::string compressContent() const;
    std::string decompressContent() const;
    
    // 原始嵌入编码
    std::string encodeContent() const;
    std::string decodeContent() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const OriginNode& other) const;
    bool operator!=(const OriginNode& other) const;

private:
    // 辅助方法
    std::string processContent(const std::string& content) const;
    std::string validateContent(const std::string& content) const;
    std::string sanitizeContent(const std::string& content) const;
    
    // 内容处理
    std::string processHtmlContent(const std::string& content) const;
    std::string processStyleContent(const std::string& content) const;
    std::string processJavaScriptContent(const std::string& content) const;
    std::string processTextContent(const std::string& content) const;
    
    // 内容验证
    bool validateHtmlContent(const std::string& content) const;
    bool validateStyleContent(const std::string& content) const;
    bool validateJavaScriptContent(const std::string& content) const;
    bool validateTextContent(const std::string& content) const;
    
    // 内容清理
    std::string sanitizeHtmlContent(const std::string& content) const;
    std::string sanitizeStyleContent(const std::string& content) const;
    std::string sanitizeJavaScriptContent(const std::string& content) const;
    std::string sanitizeTextContent(const std::string& content) const;
    
    // 内容格式化
    std::string formatHtmlContent(const std::string& content) const;
    std::string formatStyleContent(const std::string& content) const;
    std::string formatJavaScriptContent(const std::string& content) const;
    std::string formatTextContent(const std::string& content) const;
    
    // 内容压缩
    std::string compressHtmlContent(const std::string& content) const;
    std::string compressStyleContent(const std::string& content) const;
    std::string compressJavaScriptContent(const std::string& content) const;
    std::string compressTextContent(const std::string& content) const;
    
    // 内容编码
    std::string encodeHtmlContent(const std::string& content) const;
    std::string encodeStyleContent(const std::string& content) const;
    std::string encodeJavaScriptContent(const std::string& content) const;
    std::string encodeTextContent(const std::string& content) const;
    
    // 约束检查
    bool checkTypeConstraint(const std::string& type, const std::string& value) const;
    bool checkGeneralConstraint(const std::string& constraint) const;
    
    // 错误处理
    void handleProcessingError(const std::string& error);
    void handleProcessingWarning(const std::string& warning);
};

} // namespace CHTL

#endif // CHTL_ORIGIN_NODE_H
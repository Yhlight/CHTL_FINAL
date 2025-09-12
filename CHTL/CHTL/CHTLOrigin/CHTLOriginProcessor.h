#ifndef CHTL_ORIGIN_PROCESSOR_H
#define CHTL_ORIGIN_PROCESSOR_H

#include "../CHTLNode/OriginNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLOriginProcessor {
private:
    std::map<std::string, std::shared_ptr<OriginNode>> originNodes;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;

    void addError(const std::string& error);
    void addWarning(const std::string& warning);

    // 辅助方法
    std::shared_ptr<OriginNode> getOriginNode(const std::string& name) const;
    void mergeOriginProperties(std::shared_ptr<OriginNode> target, std::shared_ptr<OriginNode> source);

public:
    CHTLOriginProcessor();
    ~CHTLOriginProcessor() = default;

    void registerOriginNode(std::shared_ptr<OriginNode> originNode);
    void unregisterOriginNode(const std::string& name);
    bool hasOriginNode(const std::string& name) const;

    std::shared_ptr<BaseNode> processOriginNode(const std::string& name);
    std::shared_ptr<BaseNode> instantiateOriginNode(const std::string& name);

    // 原始嵌入处理
    void processAllOrigins();
    void processOriginByType(const std::string& type);
    void processNamedOrigins();
    void processUnnamedOrigins();

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

    // 原始嵌入验证
    bool validateOriginNode(const std::string& name) const;
    bool validateAllOrigins() const;

    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    void clearMessages();
    void reset();
    std::string getDebugInfo() const;
};

} // namespace CHTL

#endif // CHTL_ORIGIN_PROCESSOR_H
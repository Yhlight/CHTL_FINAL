#include "OriginNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

OriginNode::OriginNode(const std::string& name, OriginType type)
    : BaseNode(NodeType::ORIGIN, name), originType(type), originName(name), 
      isNamed(!name.empty()), isProcessed(false), priority(0) {
}

std::string OriginNode::getOriginTypeName() const {
    switch (originType) {
        case OriginType::HTML: return "Html";
        case OriginType::STYLE: return "Style";
        case OriginType::JAVASCRIPT: return "JavaScript";
        case OriginType::TEXT: return "Text";
        case OriginType::MIXED: return "Mixed";
        default: return "Unknown";
    }
}

void OriginNode::setOriginAttribute(const std::string& name, const std::string& value) {
    originAttributes[name] = value;
}

std::string OriginNode::getOriginAttribute(const std::string& name) const {
    auto it = originAttributes.find(name);
    return (it != originAttributes.end()) ? it->second : "";
}

bool OriginNode::hasOriginAttribute(const std::string& name) const {
    return originAttributes.find(name) != originAttributes.end();
}

void OriginNode::removeOriginAttribute(const std::string& name) {
    originAttributes.erase(name);
}

void OriginNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string OriginNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

bool OriginNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void OriginNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

void OriginNode::addConstraint(const std::string& constraint) {
    if (std::find(constraints.begin(), constraints.end(), constraint) == constraints.end()) {
        constraints.push_back(constraint);
    }
}

void OriginNode::removeConstraint(const std::string& constraint) {
    auto it = std::find(constraints.begin(), constraints.end(), constraint);
    if (it != constraints.end()) {
        constraints.erase(it);
    }
}

bool OriginNode::hasConstraint(const std::string& constraint) const {
    return std::find(constraints.begin(), constraints.end(), constraint) != constraints.end();
}

void OriginNode::setTypeConstraint(const std::string& type, const std::string& constraint) {
    typeConstraints[type] = constraint;
}

std::string OriginNode::getTypeConstraint(const std::string& type) const {
    auto it = typeConstraints.find(type);
    return (it != typeConstraints.end()) ? it->second : "";
}

bool OriginNode::hasTypeConstraint(const std::string& type) const {
    return typeConstraints.find(type) != typeConstraints.end();
}

void OriginNode::removeTypeConstraint(const std::string& type) {
    typeConstraints.erase(type);
}

void OriginNode::processOrigin() {
    try {
        switch (originType) {
            case OriginType::HTML:
                processHtmlOrigin();
                break;
            case OriginType::STYLE:
                processStyleOrigin();
                break;
            case OriginType::JAVASCRIPT:
                processJavaScriptOrigin();
                break;
            case OriginType::TEXT:
                processTextOrigin();
                break;
            case OriginType::MIXED:
                processMixedOrigin();
                break;
            default:
                addProcessingError("未知的原始嵌入类型");
                break;
        }
        
        isProcessed = true;
        
    } catch (const std::exception& e) {
        addProcessingError("处理原始嵌入时发生错误: " + std::string(e.what()));
    }
}

void OriginNode::processHtmlOrigin() {
    if (originContent.empty()) {
        addProcessingWarning("HTML原始嵌入内容为空");
        return;
    }
    
    // 验证HTML内容
    if (!validateHtmlContent(originContent)) {
        addProcessingError("HTML内容验证失败");
        return;
    }
    
    // 清理HTML内容
    std::string cleaned = sanitizeHtmlContent(originContent);
    
    // 格式化HTML内容
    std::string formatted = formatHtmlContent(cleaned);
    
    // 设置处理后的内容
    processedContent = formatted;
}

void OriginNode::processStyleOrigin() {
    if (originContent.empty()) {
        addProcessingWarning("CSS原始嵌入内容为空");
        return;
    }
    
    // 验证CSS内容
    if (!validateStyleContent(originContent)) {
        addProcessingError("CSS内容验证失败");
        return;
    }
    
    // 清理CSS内容
    std::string cleaned = sanitizeStyleContent(originContent);
    
    // 格式化CSS内容
    std::string formatted = formatStyleContent(cleaned);
    
    // 设置处理后的内容
    processedContent = formatted;
}

void OriginNode::processJavaScriptOrigin() {
    if (originContent.empty()) {
        addProcessingWarning("JavaScript原始嵌入内容为空");
        return;
    }
    
    // 验证JavaScript内容
    if (!validateJavaScriptContent(originContent)) {
        addProcessingError("JavaScript内容验证失败");
        return;
    }
    
    // 清理JavaScript内容
    std::string cleaned = sanitizeJavaScriptContent(originContent);
    
    // 格式化JavaScript内容
    std::string formatted = formatJavaScriptContent(cleaned);
    
    // 设置处理后的内容
    processedContent = formatted;
}

void OriginNode::processTextOrigin() {
    if (originContent.empty()) {
        addProcessingWarning("文本原始嵌入内容为空");
        return;
    }
    
    // 验证文本内容
    if (!validateTextContent(originContent)) {
        addProcessingError("文本内容验证失败");
        return;
    }
    
    // 清理文本内容
    std::string cleaned = sanitizeTextContent(originContent);
    
    // 格式化文本内容
    std::string formatted = formatTextContent(cleaned);
    
    // 设置处理后的内容
    processedContent = formatted;
}

void OriginNode::processMixedOrigin() {
    if (originContent.empty()) {
        addProcessingWarning("混合原始嵌入内容为空");
        return;
    }
    
    // 处理混合内容
    std::string processed = processContent(originContent);
    
    // 设置处理后的内容
    processedContent = processed;
}

void OriginNode::addProcessingError(const std::string& error) {
    processingErrors.push_back(error);
}

void OriginNode::addProcessingWarning(const std::string& warning) {
    processingWarnings.push_back(warning);
}

void OriginNode::clearProcessingMessages() {
    processingErrors.clear();
    processingWarnings.clear();
}

bool OriginNode::validateOrigin() const {
    // 检查原始嵌入名称
    if (isNamed && originName.empty()) {
        return false;
    }
    
    // 检查原始嵌入内容
    if (originContent.empty()) {
        return false;
    }
    
    // 检查内容验证
    if (!validateContent()) {
        return false;
    }
    
    // 检查约束
    if (!validateConstraints()) {
        return false;
    }
    
    return true;
}

bool OriginNode::validateContent() const {
    switch (originType) {
        case OriginType::HTML:
            return validateHtmlContent(originContent);
        case OriginType::STYLE:
            return validateStyleContent(originContent);
        case OriginType::JAVASCRIPT:
            return validateJavaScriptContent(originContent);
        case OriginType::TEXT:
            return validateTextContent(originContent);
        case OriginType::MIXED:
            return true; // 混合内容总是有效
        default:
            return false;
    }
}

bool OriginNode::validateConstraints() const {
    for (const auto& constraint : constraints) {
        if (!checkGeneralConstraint(constraint)) {
            return false;
        }
    }
    
    return true;
}

bool OriginNode::validateType() const {
    return originType != OriginType::MIXED || !originContent.empty();
}

std::string OriginNode::toHTML() const {
    switch (originType) {
        case OriginType::HTML:
            return processedContent.empty() ? originContent : processedContent;
        case OriginType::STYLE:
            return "<style>\n" + (processedContent.empty() ? originContent : processedContent) + "\n</style>";
        case OriginType::JAVASCRIPT:
            return "<script>\n" + (processedContent.empty() ? originContent : processedContent) + "\n</script>";
        case OriginType::TEXT:
            return processedContent.empty() ? originContent : processedContent;
        case OriginType::MIXED:
            return processedContent.empty() ? originContent : processedContent;
        default:
            return "";
    }
}

std::string OriginNode::toCSS() const {
    if (originType == OriginType::STYLE) {
        return processedContent.empty() ? originContent : processedContent;
    }
    return "";
}

std::string OriginNode::toJavaScript() const {
    if (originType == OriginType::JAVASCRIPT) {
        return processedContent.empty() ? originContent : processedContent;
    }
    return "";
}

std::string OriginNode::toText() const {
    return processedContent.empty() ? originContent : processedContent;
}

std::string OriginNode::toOriginString() const {
    std::ostringstream oss;
    
    oss << "[" << getOriginTypeName() << "]";
    if (isNamed) {
        oss << " " << originName;
    }
    oss << "\n";
    
    if (!originAttributes.empty()) {
        oss << "Attributes:\n";
        for (const auto& attr : originAttributes) {
            oss << "  " << attr.first << ": " << attr.second << "\n";
        }
    }
    
    oss << "Content:\n";
    oss << (processedContent.empty() ? originContent : processedContent);
    
    return oss.str();
}

std::string OriginNode::formatContent() const {
    switch (originType) {
        case OriginType::HTML:
            return formatHtmlContent(originContent);
        case OriginType::STYLE:
            return formatStyleContent(originContent);
        case OriginType::JAVASCRIPT:
            return formatJavaScriptContent(originContent);
        case OriginType::TEXT:
            return formatTextContent(originContent);
        case OriginType::MIXED:
            return originContent;
        default:
            return originContent;
    }
}

std::string OriginNode::minifyContent() const {
    switch (originType) {
        case OriginType::HTML:
            return compressHtmlContent(originContent);
        case OriginType::STYLE:
            return compressStyleContent(originContent);
        case OriginType::JAVASCRIPT:
            return compressJavaScriptContent(originContent);
        case OriginType::TEXT:
            return compressTextContent(originContent);
        case OriginType::MIXED:
            return originContent;
        default:
            return originContent;
    }
}

std::string OriginNode::beautifyContent() const {
    return formatContent();
}

std::string OriginNode::compressContent() const {
    return minifyContent();
}

std::string OriginNode::decompressContent() const {
    return originContent;
}

std::string OriginNode::encodeContent() const {
    switch (originType) {
        case OriginType::HTML:
            return encodeHtmlContent(originContent);
        case OriginType::STYLE:
            return encodeStyleContent(originContent);
        case OriginType::JAVASCRIPT:
            return encodeJavaScriptContent(originContent);
        case OriginType::TEXT:
            return encodeTextContent(originContent);
        case OriginType::MIXED:
            return originContent;
        default:
            return originContent;
    }
}

std::string OriginNode::decodeContent() const {
    return originContent;
}

std::string OriginNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "OriginType: " << getOriginTypeName() << "\n";
    oss << "OriginName: " << originName << "\n";
    oss << "IsNamed: " << (isNamed ? "true" : "false") << "\n";
    oss << "IsProcessed: " << (isProcessed ? "true" : "false") << "\n";
    oss << "Priority: " << priority << "\n";
    oss << "ContentLength: " << originContent.length() << "\n";
    oss << "ProcessedContentLength: " << processedContent.length() << "\n";
    oss << "Attributes: " << originAttributes.size() << "\n";
    oss << "Constraints: " << constraints.size() << "\n";
    oss << "ProcessingErrors: " << processingErrors.size() << "\n";
    oss << "ProcessingWarnings: " << processingWarnings.size() << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> OriginNode::clone() const {
    auto cloned = std::make_shared<OriginNode>(originName, originType);
    cloned->originContent = originContent;
    cloned->originAttributes = originAttributes;
    cloned->metadata = metadata;
    cloned->constraints = constraints;
    cloned->typeConstraints = typeConstraints;
    cloned->processedContent = processedContent;
    cloned->processingErrors = processingErrors;
    cloned->processingWarnings = processingWarnings;
    cloned->isNamed = isNamed;
    cloned->isProcessed = isProcessed;
    cloned->priority = priority;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    return cloned;
}

bool OriginNode::operator==(const OriginNode& other) const {
    return BaseNode::operator==(other) &&
           originType == other.originType &&
           originName == other.originName &&
           originContent == other.originContent &&
           originAttributes == other.originAttributes &&
           isNamed == other.isNamed &&
           isProcessed == other.isProcessed &&
           priority == other.priority;
}

bool OriginNode::operator!=(const OriginNode& other) const {
    return !(*this == other);
}

std::string OriginNode::processContent(const std::string& content) const {
    // 简化的内容处理
    return content;
}

std::string OriginNode::validateContent(const std::string& content) const {
    // 简化的内容验证
    return content;
}

std::string OriginNode::sanitizeContent(const std::string& content) const {
    // 简化的内容清理
    return content;
}

std::string OriginNode::processHtmlContent(const std::string& content) const {
    // 简化的HTML内容处理
    return content;
}

std::string OriginNode::processStyleContent(const std::string& content) const {
    // 简化的CSS内容处理
    return content;
}

std::string OriginNode::processJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容处理
    return content;
}

std::string OriginNode::processTextContent(const std::string& content) const {
    // 简化的文本内容处理
    return content;
}

bool OriginNode::validateHtmlContent(const std::string& content) const {
    // 简化的HTML内容验证
    return !content.empty();
}

bool OriginNode::validateStyleContent(const std::string& content) const {
    // 简化的CSS内容验证
    return !content.empty();
}

bool OriginNode::validateJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容验证
    return !content.empty();
}

bool OriginNode::validateTextContent(const std::string& content) const {
    // 简化的文本内容验证
    return !content.empty();
}

std::string OriginNode::sanitizeHtmlContent(const std::string& content) const {
    // 简化的HTML内容清理
    return content;
}

std::string OriginNode::sanitizeStyleContent(const std::string& content) const {
    // 简化的CSS内容清理
    return content;
}

std::string OriginNode::sanitizeJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容清理
    return content;
}

std::string OriginNode::sanitizeTextContent(const std::string& content) const {
    // 简化的文本内容清理
    return content;
}

std::string OriginNode::formatHtmlContent(const std::string& content) const {
    // 简化的HTML内容格式化
    return content;
}

std::string OriginNode::formatStyleContent(const std::string& content) const {
    // 简化的CSS内容格式化
    return content;
}

std::string OriginNode::formatJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容格式化
    return content;
}

std::string OriginNode::formatTextContent(const std::string& content) const {
    // 简化的文本内容格式化
    return content;
}

std::string OriginNode::compressHtmlContent(const std::string& content) const {
    // 简化的HTML内容压缩
    return content;
}

std::string OriginNode::compressStyleContent(const std::string& content) const {
    // 简化的CSS内容压缩
    return content;
}

std::string OriginNode::compressJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容压缩
    return content;
}

std::string OriginNode::compressTextContent(const std::string& content) const {
    // 简化的文本内容压缩
    return content;
}

std::string OriginNode::encodeHtmlContent(const std::string& content) const {
    // 简化的HTML内容编码
    return content;
}

std::string OriginNode::encodeStyleContent(const std::string& content) const {
    // 简化的CSS内容编码
    return content;
}

std::string OriginNode::encodeJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容编码
    return content;
}

std::string OriginNode::encodeTextContent(const std::string& content) const {
    // 简化的文本内容编码
    return content;
}

bool OriginNode::checkTypeConstraint(const std::string& type, const std::string& value) const {
    // 简化的类型约束检查
    return true;
}

bool OriginNode::checkGeneralConstraint(const std::string& constraint) const {
    // 简化的通用约束检查
    return true;
}

void OriginNode::handleProcessingError(const std::string& error) {
    addProcessingError(error);
}

void OriginNode::handleProcessingWarning(const std::string& warning) {
    addProcessingWarning(warning);
}

} // namespace CHTL
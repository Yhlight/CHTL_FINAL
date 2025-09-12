#include "CHTLOriginProcessor.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLOriginProcessor::CHTLOriginProcessor() {}

void CHTLOriginProcessor::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLOriginProcessor::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

std::shared_ptr<OriginNode> CHTLOriginProcessor::getOriginNode(const std::string& name) const {
    auto it = originNodes.find(name);
    if (it != originNodes.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLOriginProcessor::mergeOriginProperties(std::shared_ptr<OriginNode> target, std::shared_ptr<OriginNode> source) {
    for (const auto& attr : source->getOriginAttributes()) {
        if (!target->hasOriginAttribute(attr.first)) {
            target->setOriginAttribute(attr.first, attr.second);
        }
    }
    for (const auto& meta : source->getMetadata()) {
        if (!target->hasMetadata(meta.first)) {
            target->setMetadata(meta.first, meta.second);
        }
    }
}

void CHTLOriginProcessor::registerOriginNode(std::shared_ptr<OriginNode> originNode) {
    if (originNode) {
        originNodes[originNode->getOriginName()] = originNode;
    }
}

void CHTLOriginProcessor::unregisterOriginNode(const std::string& name) {
    originNodes.erase(name);
}

bool CHTLOriginProcessor::hasOriginNode(const std::string& name) const {
    return originNodes.count(name) > 0;
}

std::shared_ptr<BaseNode> CHTLOriginProcessor::processOriginNode(const std::string& name) {
    auto originNode = getOriginNode(name);
    if (!originNode) {
        addError("原始嵌入节点 '" + name + "' 未找到");
        return nullptr;
    }

    // 处理原始嵌入
    originNode->processOrigin();
    return originNode;
}

std::shared_ptr<BaseNode> CHTLOriginProcessor::instantiateOriginNode(const std::string& name) {
    auto originNode = getOriginNode(name);
    if (!originNode) {
        addError("原始嵌入节点 '" + name + "' 未找到");
        return nullptr;
    }
    return originNode->clone();
}

void CHTLOriginProcessor::processAllOrigins() {
    for (auto& pair : originNodes) {
        pair.second->processOrigin();
    }
}

void CHTLOriginProcessor::processOriginByType(const std::string& type) {
    for (auto& pair : originNodes) {
        if (pair.second->getOriginTypeName() == type) {
            pair.second->processOrigin();
        }
    }
}

void CHTLOriginProcessor::processNamedOrigins() {
    for (auto& pair : originNodes) {
        if (pair.second->isNamedOrigin()) {
            pair.second->processOrigin();
        }
    }
}

void CHTLOriginProcessor::processUnnamedOrigins() {
    for (auto& pair : originNodes) {
        if (!pair.second->isNamedOrigin()) {
            pair.second->processOrigin();
        }
    }
}

std::string CHTLOriginProcessor::processHtmlContent(const std::string& content) const {
    // 简化的HTML内容处理
    return content;
}

std::string CHTLOriginProcessor::processStyleContent(const std::string& content) const {
    // 简化的CSS内容处理
    return content;
}

std::string CHTLOriginProcessor::processJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容处理
    return content;
}

std::string CHTLOriginProcessor::processTextContent(const std::string& content) const {
    // 简化的文本内容处理
    return content;
}

bool CHTLOriginProcessor::validateHtmlContent(const std::string& content) const {
    // 简化的HTML内容验证
    return !content.empty();
}

bool CHTLOriginProcessor::validateStyleContent(const std::string& content) const {
    // 简化的CSS内容验证
    return !content.empty();
}

bool CHTLOriginProcessor::validateJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容验证
    return !content.empty();
}

bool CHTLOriginProcessor::validateTextContent(const std::string& content) const {
    // 简化的文本内容验证
    return !content.empty();
}

std::string CHTLOriginProcessor::sanitizeHtmlContent(const std::string& content) const {
    // 简化的HTML内容清理
    return content;
}

std::string CHTLOriginProcessor::sanitizeStyleContent(const std::string& content) const {
    // 简化的CSS内容清理
    return content;
}

std::string CHTLOriginProcessor::sanitizeJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容清理
    return content;
}

std::string CHTLOriginProcessor::sanitizeTextContent(const std::string& content) const {
    // 简化的文本内容清理
    return content;
}

std::string CHTLOriginProcessor::formatHtmlContent(const std::string& content) const {
    // 简化的HTML内容格式化
    return content;
}

std::string CHTLOriginProcessor::formatStyleContent(const std::string& content) const {
    // 简化的CSS内容格式化
    return content;
}

std::string CHTLOriginProcessor::formatJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容格式化
    return content;
}

std::string CHTLOriginProcessor::formatTextContent(const std::string& content) const {
    // 简化的文本内容格式化
    return content;
}

std::string CHTLOriginProcessor::compressHtmlContent(const std::string& content) const {
    // 简化的HTML内容压缩
    return content;
}

std::string CHTLOriginProcessor::compressStyleContent(const std::string& content) const {
    // 简化的CSS内容压缩
    return content;
}

std::string CHTLOriginProcessor::compressJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容压缩
    return content;
}

std::string CHTLOriginProcessor::compressTextContent(const std::string& content) const {
    // 简化的文本内容压缩
    return content;
}

std::string CHTLOriginProcessor::encodeHtmlContent(const std::string& content) const {
    // 简化的HTML内容编码
    return content;
}

std::string CHTLOriginProcessor::encodeStyleContent(const std::string& content) const {
    // 简化的CSS内容编码
    return content;
}

std::string CHTLOriginProcessor::encodeJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容编码
    return content;
}

std::string CHTLOriginProcessor::encodeTextContent(const std::string& content) const {
    // 简化的文本内容编码
    return content;
}

bool CHTLOriginProcessor::validateOriginNode(const std::string& name) const {
    auto originNode = getOriginNode(name);
    if (!originNode) {
        return false;
    }
    return originNode->validateOrigin();
}

bool CHTLOriginProcessor::validateAllOrigins() const {
    for (const auto& pair : originNodes) {
        if (!pair.second->validateOrigin()) {
            return false;
        }
    }
    return true;
}

const std::vector<std::string>& CHTLOriginProcessor::getErrors() const {
    return errors;
}

const std::vector<std::string>& CHTLOriginProcessor::getWarnings() const {
    return warnings;
}

void CHTLOriginProcessor::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLOriginProcessor::reset() {
    originNodes.clear();
    clearMessages();
}

std::string CHTLOriginProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL原始嵌入处理器调试信息:\n";
    oss << "注册原始嵌入节点数: " << originNodes.size() << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

} // namespace CHTL
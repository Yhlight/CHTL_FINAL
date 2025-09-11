#include "ImportNode.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>

namespace CHTL {

ImportNode::ImportNode(const std::string& path, ImportType type)
    : BaseNode(NodeType::IMPORT, "import"), importType(type), importPath(path), 
      importMode(ImportMode::EXACT), isImported(false), isProcessed(false), 
      priority(0), isRequired(true), isOptional(false) {
}

std::string ImportNode::getImportTypeName() const {
    switch (importType) {
        case ImportType::HTML: return "Html";
        case ImportType::CSS: return "Css";
        case ImportType::JAVASCRIPT: return "JavaScript";
        case ImportType::CHTL: return "Chtl";
        case ImportType::MIXED: return "Mixed";
        default: return "Unknown";
    }
}

void ImportNode::setImportOption(const std::string& key, const std::string& value) {
    importOptions[key] = value;
}

std::string ImportNode::getImportOption(const std::string& key) const {
    auto it = importOptions.find(key);
    return (it != importOptions.end()) ? it->second : "";
}

bool ImportNode::hasImportOption(const std::string& key) const {
    return importOptions.find(key) != importOptions.end();
}

void ImportNode::removeImportOption(const std::string& key) {
    importOptions.erase(key);
}

std::string ImportNode::getImportModeName() const {
    switch (importMode) {
        case ImportMode::EXACT: return "Exact";
        case ImportMode::TYPE: return "Type";
        case ImportMode::WILDCARD: return "Wildcard";
        default: return "Unknown";
    }
}

void ImportNode::addImportTarget(const std::string& target) {
    if (std::find(importTargets.begin(), importTargets.end(), target) == importTargets.end()) {
        importTargets.push_back(target);
    }
}

void ImportNode::removeImportTarget(const std::string& target) {
    auto it = std::find(importTargets.begin(), importTargets.end(), target);
    if (it != importTargets.end()) {
        importTargets.erase(it);
    }
}

bool ImportNode::hasImportTarget(const std::string& target) const {
    return std::find(importTargets.begin(), importTargets.end(), target) != importTargets.end();
}

void ImportNode::addExcludeTarget(const std::string& target) {
    if (std::find(excludeTargets.begin(), excludeTargets.end(), target) == excludeTargets.end()) {
        excludeTargets.push_back(target);
    }
}

void ImportNode::removeExcludeTarget(const std::string& target) {
    auto it = std::find(excludeTargets.begin(), excludeTargets.end(), target);
    if (it != excludeTargets.end()) {
        excludeTargets.erase(it);
    }
}

bool ImportNode::hasExcludeTarget(const std::string& target) const {
    return std::find(excludeTargets.begin(), excludeTargets.end(), target) != excludeTargets.end();
}

void ImportNode::setImportedExport(const std::string& name, const std::string& value) {
    importedExports[name] = value;
}

std::string ImportNode::getImportedExport(const std::string& name) const {
    auto it = importedExports.find(name);
    return (it != importedExports.end()) ? it->second : "";
}

bool ImportNode::hasImportedExport(const std::string& name) const {
    return importedExports.find(name) != importedExports.end();
}

void ImportNode::removeImportedExport(const std::string& name) {
    importedExports.erase(name);
}

void ImportNode::addImportedNode(std::shared_ptr<BaseNode> node) {
    if (node) {
        importedNodes.push_back(node);
    }
}

void ImportNode::removeImportedNode(std::shared_ptr<BaseNode> node) {
    auto it = std::find(importedNodes.begin(), importedNodes.end(), node);
    if (it != importedNodes.end()) {
        importedNodes.erase(it);
    }
}

void ImportNode::addImportError(const std::string& error) {
    importErrors.push_back(error);
}

void ImportNode::addImportWarning(const std::string& warning) {
    importWarnings.push_back(warning);
}

void ImportNode::clearImportMessages() {
    importErrors.clear();
    importWarnings.clear();
}

void ImportNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string ImportNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

bool ImportNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void ImportNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

bool ImportNode::processImport() {
    try {
        // 验证导入
        if (!validateImport()) {
            addImportError("导入验证失败");
            return false;
        }
        
        // 解析导入路径
        std::string resolvedPath = resolveImportPath();
        if (resolvedPath.empty()) {
            addImportError("无法解析导入路径: " + importPath);
            return false;
        }
        
        // 根据类型处理导入
        bool success = false;
        switch (importType) {
            case ImportType::HTML:
                success = processHtmlImport();
                break;
            case ImportType::CSS:
                success = processCssImport();
                break;
            case ImportType::JAVASCRIPT:
                success = processJavaScriptImport();
                break;
            case ImportType::CHTL:
                success = processChtlImport();
                break;
            case ImportType::MIXED:
                success = processMixedImport();
                break;
            default:
                addImportError("未知的导入类型");
                return false;
        }
        
        if (success) {
            isImported = true;
            isProcessed = true;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        addImportError("处理导入时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool ImportNode::processHtmlImport() {
    // 简化的HTML导入处理
    if (importPath.empty()) {
        addImportError("HTML导入路径为空");
        return false;
    }
    
    // 这里应该实际读取HTML文件
    // 简化实现
    importedContent = "<!-- Imported HTML from " + importPath + " -->";
    
    return true;
}

bool ImportNode::processCssImport() {
    // 简化的CSS导入处理
    if (importPath.empty()) {
        addImportError("CSS导入路径为空");
        return false;
    }
    
    // 这里应该实际读取CSS文件
    // 简化实现
    importedContent = "/* Imported CSS from " + importPath + " */";
    
    return true;
}

bool ImportNode::processJavaScriptImport() {
    // 简化的JavaScript导入处理
    if (importPath.empty()) {
        addImportError("JavaScript导入路径为空");
        return false;
    }
    
    // 这里应该实际读取JavaScript文件
    // 简化实现
    importedContent = "// Imported JavaScript from " + importPath;
    
    return true;
}

bool ImportNode::processChtlImport() {
    // 简化的CHTL导入处理
    if (importPath.empty()) {
        addImportError("CHTL导入路径为空");
        return false;
    }
    
    // 这里应该实际读取CHTL文件并解析
    // 简化实现
    importedContent = "/* Imported CHTL from " + importPath + " */";
    
    return true;
}

bool ImportNode::processMixedImport() {
    // 简化的混合导入处理
    if (importPath.empty()) {
        addImportError("混合导入路径为空");
        return false;
    }
    
    // 这里应该根据文件扩展名判断类型并处理
    // 简化实现
    importedContent = "/* Imported Mixed content from " + importPath + " */";
    
    return true;
}

bool ImportNode::validateImport() const {
    // 检查导入路径
    if (!validatePath()) {
        return false;
    }
    
    // 检查导入类型
    if (!validateType()) {
        return false;
    }
    
    // 检查导入模式
    if (!validateMode()) {
        return false;
    }
    
    // 检查导入目标
    if (!validateTargets()) {
        return false;
    }
    
    return true;
}

bool ImportNode::validatePath() const {
    if (importPath.empty()) {
        return false;
    }
    
    // 检查路径格式
    if (importPath.find("..") != std::string::npos) {
        return false; // 防止路径遍历攻击
    }
    
    return true;
}

bool ImportNode::validateType() const {
    return importType != ImportType::MIXED || !importPath.empty();
}

bool ImportNode::validateMode() const {
    return importMode == ImportMode::EXACT || 
           importMode == ImportMode::TYPE || 
           importMode == ImportMode::WILDCARD;
}

bool ImportNode::validateTargets() const {
    // 检查导入目标
    for (const auto& target : importTargets) {
        if (target.empty()) {
            return false;
        }
    }
    
    // 检查排除目标
    for (const auto& target : excludeTargets) {
        if (target.empty()) {
            return false;
        }
    }
    
    return true;
}

std::string ImportNode::parseImportPath() const {
    return processImportPath(importPath);
}

std::string ImportNode::resolveImportPath() const {
    return resolvePath(importPath);
}

std::vector<std::string> ImportNode::parseImportTargets() const {
    return importTargets;
}

std::vector<std::string> ImportNode::parseExcludeTargets() const {
    return excludeTargets;
}

std::string ImportNode::toHTML() const {
    switch (importType) {
        case ImportType::HTML:
            return importedContent;
        case ImportType::CSS:
            return "<style>\n" + importedContent + "\n</style>";
        case ImportType::JAVASCRIPT:
            return "<script>\n" + importedContent + "\n</script>";
        case ImportType::CHTL:
            return "<!-- CHTL Import: " + importPath + " -->\n" + importedContent;
        case ImportType::MIXED:
            return importedContent;
        default:
            return "";
    }
}

std::string ImportNode::toCSS() const {
    if (importType == ImportType::CSS) {
        return importedContent;
    }
    return "";
}

std::string ImportNode::toJavaScript() const {
    if (importType == ImportType::JAVASCRIPT) {
        return importedContent;
    }
    return "";
}

std::string ImportNode::toImportString() const {
    std::ostringstream oss;
    
    oss << "[" << getImportTypeName() << "] " << importPath;
    if (!importAlias.empty()) {
        oss << " as " << importAlias;
    }
    oss << "\n";
    
    if (!importTargets.empty()) {
        oss << "Targets: " << importTargets[0];
        for (size_t i = 1; i < importTargets.size(); ++i) {
            oss << ", " << importTargets[i];
        }
        oss << "\n";
    }
    
    if (!excludeTargets.empty()) {
        oss << "Exclude: " << excludeTargets[0];
        for (size_t i = 1; i < excludeTargets.size(); ++i) {
            oss << ", " << excludeTargets[i];
        }
        oss << "\n";
    }
    
    oss << "Content:\n";
    oss << importedContent;
    
    return oss.str();
}

std::string ImportNode::formatImport() const {
    return toImportString();
}

std::string ImportNode::minifyImport() const {
    return importedContent;
}

std::string ImportNode::beautifyImport() const {
    return formatImport();
}

std::string ImportNode::compressImport() const {
    return minifyImport();
}

std::string ImportNode::decompressImport() const {
    return importedContent;
}

std::string ImportNode::encodeImport() const {
    return importedContent;
}

std::string ImportNode::decodeImport() const {
    return importedContent;
}

std::string ImportNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "ImportType: " << getImportTypeName() << "\n";
    oss << "ImportPath: " << importPath << "\n";
    oss << "ImportAlias: " << importAlias << "\n";
    oss << "ImportMode: " << getImportModeName() << "\n";
    oss << "ImportTargets: " << importTargets.size() << "\n";
    oss << "ExcludeTargets: " << excludeTargets.size() << "\n";
    oss << "IsImported: " << (isImported ? "true" : "false") << "\n";
    oss << "IsProcessed: " << (isProcessed ? "true" : "false") << "\n";
    oss << "Priority: " << priority << "\n";
    oss << "IsRequired: " << (isRequired ? "true" : "false") << "\n";
    oss << "IsOptional: " << (isOptional ? "true" : "false") << "\n";
    oss << "ContentLength: " << importedContent.length() << "\n";
    oss << "Exports: " << importedExports.size() << "\n";
    oss << "Nodes: " << importedNodes.size() << "\n";
    oss << "Errors: " << importErrors.size() << "\n";
    oss << "Warnings: " << importWarnings.size() << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(importPath, importType);
    cloned->importAlias = importAlias;
    cloned->importOptions = importOptions;
    cloned->importMode = importMode;
    cloned->importTargets = importTargets;
    cloned->excludeTargets = excludeTargets;
    cloned->importedContent = importedContent;
    cloned->importedExports = importedExports;
    cloned->importedNodes = importedNodes;
    cloned->importErrors = importErrors;
    cloned->importWarnings = importWarnings;
    cloned->metadata = metadata;
    cloned->isImported = isImported;
    cloned->isProcessed = isProcessed;
    cloned->priority = priority;
    cloned->isRequired = isRequired;
    cloned->isOptional = isOptional;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    return cloned;
}

bool ImportNode::operator==(const ImportNode& other) const {
    return BaseNode::operator==(other) &&
           importType == other.importType &&
           importPath == other.importPath &&
           importAlias == other.importAlias &&
           importMode == other.importMode &&
           importTargets == other.importTargets &&
           excludeTargets == other.excludeTargets &&
           isImported == other.isImported &&
           isProcessed == other.isProcessed &&
           priority == other.priority &&
           isRequired == other.isRequired &&
           isOptional == other.isOptional;
}

bool ImportNode::operator!=(const ImportNode& other) const {
    return !(*this == other);
}

std::string ImportNode::processImportPath(const std::string& path) const {
    // 简化的导入路径处理
    return path;
}

std::string ImportNode::validateImportPath(const std::string& path) const {
    // 简化的导入路径验证
    return path;
}

std::string ImportNode::sanitizeImportPath(const std::string& path) const {
    // 简化的导入路径清理
    return path;
}

std::string ImportNode::resolvePath(const std::string& path) const {
    // 简化的路径解析
    return path;
}

std::string ImportNode::normalizePath(const std::string& path) const {
    // 简化的路径规范化
    return path;
}

std::string ImportNode::expandPath(const std::string& path) const {
    // 简化的路径展开
    return path;
}

std::string ImportNode::processHtmlContent(const std::string& content) const {
    // 简化的HTML内容处理
    return content;
}

std::string ImportNode::processCssContent(const std::string& content) const {
    // 简化的CSS内容处理
    return content;
}

std::string ImportNode::processJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容处理
    return content;
}

std::string ImportNode::processChtlContent(const std::string& content) const {
    // 简化的CHTL内容处理
    return content;
}

std::string ImportNode::processMixedContent(const std::string& content) const {
    // 简化的混合内容处理
    return content;
}

bool ImportNode::validateHtmlContent(const std::string& content) const {
    // 简化的HTML内容验证
    return !content.empty();
}

bool ImportNode::validateCssContent(const std::string& content) const {
    // 简化的CSS内容验证
    return !content.empty();
}

bool ImportNode::validateJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容验证
    return !content.empty();
}

bool ImportNode::validateChtlContent(const std::string& content) const {
    // 简化的CHTL内容验证
    return !content.empty();
}

bool ImportNode::validateMixedContent(const std::string& content) const {
    // 简化的混合内容验证
    return !content.empty();
}

std::string ImportNode::sanitizeHtmlContent(const std::string& content) const {
    // 简化的HTML内容清理
    return content;
}

std::string ImportNode::sanitizeCssContent(const std::string& content) const {
    // 简化的CSS内容清理
    return content;
}

std::string ImportNode::sanitizeJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容清理
    return content;
}

std::string ImportNode::sanitizeChtlContent(const std::string& content) const {
    // 简化的CHTL内容清理
    return content;
}

std::string ImportNode::sanitizeMixedContent(const std::string& content) const {
    // 简化的混合内容清理
    return content;
}

std::string ImportNode::formatHtmlContent(const std::string& content) const {
    // 简化的HTML内容格式化
    return content;
}

std::string ImportNode::formatCssContent(const std::string& content) const {
    // 简化的CSS内容格式化
    return content;
}

std::string ImportNode::formatJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容格式化
    return content;
}

std::string ImportNode::formatChtlContent(const std::string& content) const {
    // 简化的CHTL内容格式化
    return content;
}

std::string ImportNode::formatMixedContent(const std::string& content) const {
    // 简化的混合内容格式化
    return content;
}

std::string ImportNode::compressHtmlContent(const std::string& content) const {
    // 简化的HTML内容压缩
    return content;
}

std::string ImportNode::compressCssContent(const std::string& content) const {
    // 简化的CSS内容压缩
    return content;
}

std::string ImportNode::compressJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容压缩
    return content;
}

std::string ImportNode::compressChtlContent(const std::string& content) const {
    // 简化的CHTL内容压缩
    return content;
}

std::string ImportNode::compressMixedContent(const std::string& content) const {
    // 简化的混合内容压缩
    return content;
}

std::string ImportNode::encodeHtmlContent(const std::string& content) const {
    // 简化的HTML内容编码
    return content;
}

std::string ImportNode::encodeCssContent(const std::string& content) const {
    // 简化的CSS内容编码
    return content;
}

std::string ImportNode::encodeJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容编码
    return content;
}

std::string ImportNode::encodeChtlContent(const std::string& content) const {
    // 简化的CHTL内容编码
    return content;
}

std::string ImportNode::encodeMixedContent(const std::string& content) const {
    // 简化的混合内容编码
    return content;
}

void ImportNode::handleImportError(const std::string& error) {
    addImportError(error);
}

void ImportNode::handleImportWarning(const std::string& warning) {
    addImportWarning(warning);
}

} // namespace CHTL
#include "ImportNode.h"
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace CHTL {

ImportNode::ImportNode(ImportType type, const std::string& path) 
    : BaseNode(NodeType::Import, "import"), importType(type), targetPath(path), 
      targetType(ImportTargetType::UNKNOWN), isRelative(false) {
    // 自动检测目标类型
    targetType = detectTargetType(path);
}

void ImportNode::addSpecificImport(const std::string& import) {
    specificImports.push_back(import);
}

void ImportNode::removeSpecificImport(const std::string& import) {
    auto it = std::find(specificImports.begin(), specificImports.end(), import);
    if (it != specificImports.end()) {
        specificImports.erase(it);
    }
}

void ImportNode::addImportMapping(const std::string& from, const std::string& to) {
    importMap[from] = to;
}

void ImportNode::removeImportMapping(const std::string& from) {
    importMap.erase(from);
}

void ImportNode::addSearchPath(const std::string& path) {
    searchPaths.push_back(path);
}

std::string ImportNode::resolvePath() const {
    if (isAbsolutePath(targetPath)) {
        return resolveAbsolutePath();
    } else {
        return resolveRelativePath();
    }
}

std::string ImportNode::getFullPath() const {
    return resolvePath();
}

bool ImportNode::isValidPath() const {
    std::string fullPath = resolvePath();
    return !fullPath.empty();
}

bool ImportNode::isFileExists() const {
    std::string fullPath = resolvePath();
    if (fullPath.empty()) {
        return false;
    }
    
    return std::filesystem::exists(fullPath);
}

std::shared_ptr<BaseNode> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(importType, targetPath);
    cloned->targetType = targetType;
    cloned->alias = alias;
    cloned->specificImports = specificImports;
    cloned->importMap = importMap;
    cloned->isRelative = isRelative;
    cloned->basePath = basePath;
    cloned->searchPaths = searchPaths;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    return cloned;
}

std::string ImportNode::toString() const {
    std::ostringstream oss;
    oss << "ImportNode(" << static_cast<int>(importType) << ", \"" << targetPath << "\")";
    return oss.str();
}

std::string ImportNode::toHTML() const {
    // 导入节点不直接生成HTML
    return "";
}

std::string ImportNode::toCSS() const {
    // 导入节点不直接生成CSS
    return "";
}

std::string ImportNode::toJS() const {
    // 导入节点不直接生成JS
    return "";
}

bool ImportNode::isValid() const {
    return BaseNode::isValid() && !targetPath.empty();
}

std::string ImportNode::debugString() const {
    std::ostringstream oss;
    oss << "ImportNode: " << targetPath << " (Type: " << static_cast<int>(importType) 
        << ", TargetType: " << static_cast<int>(targetType) 
        << ", Line: " << line << ", Column: " << column << ")";
    return oss.str();
}

ImportTargetType ImportNode::detectTargetType(const std::string& path) {
    std::string ext = getFileExtension(path);
    
    if (ext == ".html" || ext == ".htm") {
        return ImportTargetType::HTML;
    } else if (ext == ".css") {
        return ImportTargetType::CSS;
    } else if (ext == ".js") {
        return ImportTargetType::JS;
    } else if (ext == ".chtl") {
        return ImportTargetType::CHTL;
    } else if (ext == ".cjjs") {
        return ImportTargetType::CJJS;
    } else if (ext == ".mod" || ext == ".cmod" || ext == ".cjmod") {
        return ImportTargetType::MODULE;
    } else {
        return ImportTargetType::UNKNOWN;
    }
}

std::string ImportNode::getFileExtension(const std::string& path) {
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        return path.substr(dotPos);
    }
    return "";
}

bool ImportNode::isAbsolutePath(const std::string& path) {
    return !path.empty() && (path[0] == '/' || path[0] == '\\' || 
                            (path.length() > 1 && path[1] == ':'));
}

std::string ImportNode::normalizePath(const std::string& path) {
    std::string normalized = path;
    
    // 替换反斜杠为正斜杠
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // 移除重复的斜杠
    std::string result;
    bool lastWasSlash = false;
    for (char c : normalized) {
        if (c == '/') {
            if (!lastWasSlash) {
                result += c;
            }
            lastWasSlash = true;
        } else {
            result += c;
            lastWasSlash = false;
        }
    }
    
    return result;
}

std::string ImportNode::resolveRelativePath() const {
    if (basePath.empty()) {
        return targetPath;
    }
    
    std::string base = normalizePath(basePath);
    std::string target = normalizePath(targetPath);
    
    // 如果basePath以斜杠结尾，直接拼接
    if (base.back() == '/') {
        return base + target;
    } else {
        return base + "/" + target;
    }
}

std::string ImportNode::resolveAbsolutePath() const {
    return normalizePath(targetPath);
}

std::string ImportNode::findInSearchPaths() const {
    for (const auto& searchPath : searchPaths) {
        std::string fullPath = normalizePath(searchPath) + "/" + normalizePath(targetPath);
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }
    }
    return "";
}

} // namespace CHTL
#include "CHTLImport.h"
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <regex>

namespace CHTL {

// ImportNode is already implemented in CHTLNode.cpp

CHTLImportManager::CHTLImportManager() {
    // Initialize with default search paths
    search_paths.push_back(".");
    search_paths.push_back("./imports");
    search_paths.push_back("./modules");
}

CHTLImportManager::~CHTLImportManager() {
    // Cleanup
}

ImportResult CHTLImportManager::processImport(const ImportDefinition& definition) {
    if (!validateImport(definition)) {
        return {false, "", "Invalid import definition", definition.type, ""};
    }
    
    std::string resolved_path = resolveImportPath(definition.path, definition.type);
    if (resolved_path.empty()) {
        return {false, "", "Could not resolve import path: " + definition.path, definition.type, ""};
    }
    
    // Check cache first
    std::string cache_key = generateImportKey(definition);
    auto cache_it = import_cache.find(cache_key);
    if (cache_it != import_cache.end()) {
        return {true, cache_it->second, "", definition.type, resolved_path};
    }
    
    // Read file content
    std::string content = readFile(resolved_path);
    if (content.empty()) {
        return {false, "", "Could not read file: " + resolved_path, definition.type, resolved_path};
    }
    
    // Process content based on type and mode
    std::string processed_content;
    switch (definition.type) {
        case ImportType::HTML:
            processed_content = processHTMLContent(content, definition.alias);
            break;
        case ImportType::CSS:
            processed_content = processCSSContent(content, definition.alias);
            break;
        case ImportType::JavaScript:
            processed_content = processJavaScriptContent(content, definition.alias);
            break;
        case ImportType::CHTL:
            processed_content = processCHTLContent(content, definition.alias, definition.mode);
            break;
        case ImportType::CHTLJS:
            processed_content = processCHTLJSContent(content, definition.alias, definition.mode);
            break;
        default:
            return {false, "", "Unsupported import type", definition.type, resolved_path};
    }
    
    // Cache the result
    import_cache[cache_key] = processed_content;
    
    return {true, processed_content, "", definition.type, resolved_path};
}

ImportResult CHTLImportManager::processHTMLImport(const std::string& path, const std::string& alias) {
    ImportDefinition def;
    def.type = ImportType::HTML;
    def.mode = ImportMode::Precise;
    def.path = path;
    def.alias = alias;
    
    return processImport(def);
}

ImportResult CHTLImportManager::processCSSImport(const std::string& path, const std::string& alias) {
    ImportDefinition def;
    def.type = ImportType::CSS;
    def.mode = ImportMode::Precise;
    def.path = path;
    def.alias = alias;
    
    return processImport(def);
}

ImportResult CHTLImportManager::processJavaScriptImport(const std::string& path, const std::string& alias) {
    ImportDefinition def;
    def.type = ImportType::JavaScript;
    def.mode = ImportMode::Precise;
    def.path = path;
    def.alias = alias;
    
    return processImport(def);
}

ImportResult CHTLImportManager::processCHTLImport(const std::string& path, const std::string& alias, ImportMode mode) {
    ImportDefinition def;
    def.type = ImportType::CHTL;
    def.mode = mode;
    def.path = path;
    def.alias = alias;
    
    return processImport(def);
}

ImportResult CHTLImportManager::processCHTLJSImport(const std::string& path, const std::string& alias, ImportMode mode) {
    ImportDefinition def;
    def.type = ImportType::CHTLJS;
    def.mode = mode;
    def.path = path;
    def.alias = alias;
    
    return processImport(def);
}

std::string CHTLImportManager::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLImportManager::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string CHTLImportManager::resolvePath(const std::string& path) {
    // Simple path resolution
    if (std::filesystem::exists(path)) {
        return std::filesystem::absolute(path).string();
    }
    
    // Try with search paths
    for (const auto& search_path : search_paths) {
        std::string full_path = search_path + "/" + path;
        if (std::filesystem::exists(full_path)) {
            return std::filesystem::absolute(full_path).string();
        }
    }
    
    return "";
}

std::string CHTLImportManager::resolveImportPath(const std::string& path, ImportType type) {
    std::string resolved = resolvePath(path);
    if (!resolved.empty()) {
        return resolved;
    }
    
    // Try with type-specific extensions
    std::string extension = getFileExtension(path);
    if (extension.empty()) {
        switch (type) {
            case ImportType::HTML:
                extension = ".html";
                break;
            case ImportType::CSS:
                extension = ".css";
                break;
            case ImportType::JavaScript:
                extension = ".js";
                break;
            case ImportType::CHTL:
                extension = ".chtl";
                break;
            case ImportType::CHTLJS:
                extension = ".cjjs";
                break;
            default:
                break;
        }
        
        std::string path_with_ext = path + extension;
        resolved = resolvePath(path_with_ext);
        if (!resolved.empty()) {
            return resolved;
        }
    }
    
    return "";
}

std::vector<std::string> CHTLImportManager::findMatchingFiles(const std::string& pattern, ImportType /* type */) {
    std::vector<std::string> matches;
    
    // Simple pattern matching implementation
    for (const auto& search_path : search_paths) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(search_path)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (isWildcardPattern(pattern)) {
                        // Simple wildcard matching
                        if (filename.find(pattern.substr(0, pattern.find('*'))) == 0) {
                            matches.push_back(entry.path().string());
                        }
                    } else {
                        if (filename == pattern) {
                            matches.push_back(entry.path().string());
                        }
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error&) {
            // Ignore filesystem errors
        }
    }
    
    return matches;
}

bool CHTLImportManager::validateImport(const ImportDefinition& definition) {
    // Validate path
    if (definition.path.empty()) {
        return false;
    }
    
    // Validate type
    switch (definition.type) {
        case ImportType::HTML:
        case ImportType::CSS:
        case ImportType::JavaScript:
        case ImportType::CHTL:
        case ImportType::CHTLJS:
            break;
        default:
            return false;
    }
    
    // Validate mode
    switch (definition.mode) {
        case ImportMode::Precise:
        case ImportMode::Type:
        case ImportMode::Wildcard:
            break;
        default:
            return false;
    }
    
    return true;
}

bool CHTLImportManager::validatePath(const std::string& path) {
    return !path.empty() && (fileExists(path) || !resolvePath(path).empty());
}

void CHTLImportManager::addSearchPath(const std::string& path) {
    if (std::find(search_paths.begin(), search_paths.end(), path) == search_paths.end()) {
        search_paths.push_back(path);
    }
}

void CHTLImportManager::removeSearchPath(const std::string& path) {
    auto it = std::find(search_paths.begin(), search_paths.end(), path);
    if (it != search_paths.end()) {
        search_paths.erase(it);
    }
}

std::vector<std::string> CHTLImportManager::getSearchPaths() const {
    return search_paths;
}

void CHTLImportManager::clearSearchPaths() {
    search_paths.clear();
}

std::string CHTLImportManager::processHTMLContent(const std::string& content, const std::string& alias) {
    std::string processed = sanitizeContent(content, ImportType::HTML);
    return applyAlias(processed, alias, ImportType::HTML);
}

std::string CHTLImportManager::processCSSContent(const std::string& content, const std::string& alias) {
    std::string processed = sanitizeContent(content, ImportType::CSS);
    return applyAlias(processed, alias, ImportType::CSS);
}

std::string CHTLImportManager::processJavaScriptContent(const std::string& content, const std::string& alias) {
    std::string processed = sanitizeContent(content, ImportType::JavaScript);
    return applyAlias(processed, alias, ImportType::JavaScript);
}

std::string CHTLImportManager::processCHTLContent(const std::string& content, const std::string& alias, ImportMode /* mode */) {
    std::string processed = sanitizeContent(content, ImportType::CHTL);
    return applyAlias(processed, alias, ImportType::CHTL);
}

std::string CHTLImportManager::processCHTLJSContent(const std::string& content, const std::string& alias, ImportMode /* mode */) {
    std::string processed = sanitizeContent(content, ImportType::CHTLJS);
    return applyAlias(processed, alias, ImportType::CHTLJS);
}

std::string CHTLImportManager::getFileExtension(const std::string& path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos != std::string::npos) {
        return path.substr(dot_pos);
    }
    return "";
}

ImportType CHTLImportManager::getImportTypeFromExtension(const std::string& extension) {
    if (extension == ".html" || extension == ".htm") {
        return ImportType::HTML;
    } else if (extension == ".css") {
        return ImportType::CSS;
    } else if (extension == ".js") {
        return ImportType::JavaScript;
    } else if (extension == ".chtl") {
        return ImportType::CHTL;
    } else if (extension == ".cjjs") {
        return ImportType::CHTLJS;
    }
    return ImportType::HTML; // Default
}

std::string CHTLImportManager::generateImportKey(const ImportDefinition& definition) {
    std::stringstream ss;
    ss << static_cast<int>(definition.type) << ":" 
       << static_cast<int>(definition.mode) << ":" 
       << definition.path << ":" 
       << definition.alias;
    return ss.str();
}

bool CHTLImportManager::isWildcardPattern(const std::string& pattern) {
    return pattern.find('*') != std::string::npos;
}

std::vector<std::string> CHTLImportManager::expandWildcardPattern(const std::string& pattern, ImportType type) {
    return findMatchingFiles(pattern, type);
}

std::string CHTLImportManager::sanitizeContent(const std::string& content, ImportType type) {
    // Basic content sanitization
    std::string sanitized = content;
    
    // Remove potential security issues
    // This is a simplified implementation
    switch (type) {
        case ImportType::HTML:
            // Basic HTML sanitization
            break;
        case ImportType::CSS:
            // Basic CSS sanitization
            break;
        case ImportType::JavaScript:
            // Basic JavaScript sanitization
            break;
        case ImportType::CHTL:
        case ImportType::CHTLJS:
            // CHTL content sanitization
            break;
        default:
            break;
    }
    
    return sanitized;
}

std::string CHTLImportManager::applyAlias(const std::string& content, const std::string& alias, ImportType type) {
    if (alias.empty()) {
        return content;
    }
    
    // Apply alias to content based on type
    // This is a simplified implementation
    switch (type) {
        case ImportType::HTML:
            // Apply alias to HTML content
            return content;
        case ImportType::CSS:
            // Apply alias to CSS content
            return content;
        case ImportType::JavaScript:
            // Apply alias to JavaScript content
            return content;
        case ImportType::CHTL:
        case ImportType::CHTLJS:
            // Apply alias to CHTL content
            return content;
        default:
            return content;
    }
}

} // namespace CHTL
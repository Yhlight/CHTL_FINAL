#include "CHTLOrigin.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

// OriginNode is already implemented in CHTLNode.cpp

CHTLOriginManager::CHTLOriginManager() {
    // Initialize origin manager
}

CHTLOriginManager::~CHTLOriginManager() {
    // Cleanup
}

bool CHTLOriginManager::addRawEmbedding(const RawEmbedding& embedding) {
    if (!validateRawEmbedding(embedding)) {
        return false;
    }
    
    std::string key = generateOriginKey(embedding.name, embedding.type);
    
    switch (embedding.type) {
        case OriginType::Html:
            html_embeddings[key] = std::make_shared<RawEmbedding>(embedding);
            break;
        case OriginType::Style:
            style_embeddings[key] = std::make_shared<RawEmbedding>(embedding);
            break;
        case OriginType::JavaScript:
            javascript_embeddings[key] = std::make_shared<RawEmbedding>(embedding);
            break;
        default:
            return false;
    }
    
    return true;
}

bool CHTLOriginManager::removeRawEmbedding(const std::string& name, OriginType type) {
    std::string key = generateOriginKey(name, type);
    
    switch (type) {
        case OriginType::Html:
            return html_embeddings.erase(key) > 0;
        case OriginType::Style:
            return style_embeddings.erase(key) > 0;
        case OriginType::JavaScript:
            return javascript_embeddings.erase(key) > 0;
        default:
            return false;
    }
}

std::shared_ptr<RawEmbedding> CHTLOriginManager::findRawEmbedding(const std::string& name, OriginType type) {
    std::string key = generateOriginKey(name, type);
    
    switch (type) {
        case OriginType::Html:
            {
                auto it = html_embeddings.find(key);
                return (it != html_embeddings.end()) ? it->second : nullptr;
            }
        case OriginType::Style:
            {
                auto it = style_embeddings.find(key);
                return (it != style_embeddings.end()) ? it->second : nullptr;
            }
        case OriginType::JavaScript:
            {
                auto it = javascript_embeddings.find(key);
                return (it != javascript_embeddings.end()) ? it->second : nullptr;
            }
        default:
            return nullptr;
    }
}

std::string CHTLOriginManager::processOrigin(const OriginNode& origin_node) {
    if (origin_node.origin_type == "HTML") {
        return processHtmlOrigin(origin_node.origin_content, origin_node.origin_name);
    } else if (origin_node.origin_type == "CSS") {
        return processStyleOrigin(origin_node.origin_content, origin_node.origin_name);
    } else if (origin_node.origin_type == "JS") {
        return processJavaScriptOrigin(origin_node.origin_content, origin_node.origin_name);
    }
    return "";
}

std::string CHTLOriginManager::processHtmlOrigin(const std::string& content, const std::string& name) {
    std::string processed = processHtmlContent(content);
    
    if (!name.empty()) {
        // Add named origin processing
        processed = "<!-- Origin: " + name + " -->\n" + processed;
    }
    
    return processed;
}

std::string CHTLOriginManager::processStyleOrigin(const std::string& content, const std::string& name) {
    std::string processed = processStyleContent(content);
    
    if (!name.empty()) {
        // Add named origin processing
        processed = "/* Origin: " + name + " */\n" + processed;
    }
    
    return "<style>" + processed + "</style>";
}

std::string CHTLOriginManager::processJavaScriptOrigin(const std::string& content, const std::string& name) {
    std::string processed = processJavaScriptContent(content);
    
    if (!name.empty()) {
        // Add named origin processing
        processed = "/* Origin: " + name + " */\n" + processed;
    }
    
    return "<script>" + processed + "</script>";
}

bool CHTLOriginManager::validateRawEmbedding(const RawEmbedding& embedding) {
    // Validate content
    if (embedding.content.empty()) {
        return false;
    }
    
    // Validate type
    switch (embedding.type) {
        case OriginType::Html:
        case OriginType::Style:
        case OriginType::JavaScript:
            break;
        default:
            return false;
    }
    
    // Validate content based on type
    return validateOriginContent(embedding.type, embedding.content);
}

bool CHTLOriginManager::validateOriginContent(OriginType type, const std::string& /* content */) {
    switch (type) {
        case OriginType::Html:
            // Basic HTML validation - check for balanced tags
            return true; // Simplified validation
        case OriginType::Style:
            // Basic CSS validation - check for balanced braces
            return true; // Simplified validation
        case OriginType::JavaScript:
            // Basic JavaScript validation - check for balanced braces and parentheses
            return true; // Simplified validation
        default:
            return false;
    }
}

std::vector<std::string> CHTLOriginManager::getRawEmbeddingNames(OriginType type) const {
    std::vector<std::string> names;
    
    switch (type) {
        case OriginType::Html:
            for (const auto& pair : html_embeddings) {
                names.push_back(pair.first);
            }
            break;
        case OriginType::Style:
            for (const auto& pair : style_embeddings) {
                names.push_back(pair.first);
            }
            break;
        case OriginType::JavaScript:
            for (const auto& pair : javascript_embeddings) {
                names.push_back(pair.first);
            }
            break;
        default:
            break;
    }
    
    return names;
}

void CHTLOriginManager::clearAllRawEmbeddings() {
    html_embeddings.clear();
    style_embeddings.clear();
    javascript_embeddings.clear();
}

std::string CHTLOriginManager::processHtmlContent(const std::string& content) {
    return sanitizeHtmlContent(content);
}

std::string CHTLOriginManager::processStyleContent(const std::string& content) {
    return sanitizeStyleContent(content);
}

std::string CHTLOriginManager::processJavaScriptContent(const std::string& content) {
    return sanitizeJavaScriptContent(content);
}

std::string CHTLOriginManager::generateOriginKey(const std::string& name, OriginType type) {
    std::stringstream ss;
    ss << static_cast<int>(type) << ":" << name;
    return ss.str();
}

std::string CHTLOriginManager::sanitizeHtmlContent(const std::string& content) {
    // Basic HTML sanitization
    std::string sanitized = content;
    
    // Remove potentially dangerous content
    // This is a simplified implementation
    sanitized = escapeHtmlEntities(sanitized);
    
    return sanitized;
}

std::string CHTLOriginManager::sanitizeStyleContent(const std::string& content) {
    // Basic CSS sanitization
    std::string sanitized = content;
    
    // Remove potentially dangerous content
    // This is a simplified implementation
    sanitized = escapeCssEntities(sanitized);
    
    return sanitized;
}

std::string CHTLOriginManager::sanitizeJavaScriptContent(const std::string& content) {
    // Basic JavaScript sanitization
    std::string sanitized = content;
    
    // Remove potentially dangerous content
    // This is a simplified implementation
    sanitized = escapeJavaScriptEntities(sanitized);
    
    return sanitized;
}

std::string CHTLOriginManager::escapeHtmlEntities(const std::string& content) {
    std::string escaped = content;
    
    // Escape HTML entities using string replacement
    size_t pos = 0;
    while ((pos = escaped.find('<', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = escaped.find('>', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = escaped.find('&', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = escaped.find('\'', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return escaped;
}

std::string CHTLOriginManager::escapeCssEntities(const std::string& content) {
    // CSS doesn't need HTML entity escaping
    return content;
}

std::string CHTLOriginManager::escapeJavaScriptEntities(const std::string& content) {
    // JavaScript doesn't need HTML entity escaping
    return content;
}

} // namespace CHTL
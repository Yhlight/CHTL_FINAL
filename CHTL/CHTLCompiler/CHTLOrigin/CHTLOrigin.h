#ifndef CHTL_ORIGIN_H
#define CHTL_ORIGIN_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

// Origin types for raw embedding
enum class OriginType {
    Html,
    Style,
    JavaScript
};

// Raw embedding definition
struct RawEmbedding {
    OriginType type;
    std::string content;
    std::string name;  // For named raw embedding
    std::map<std::string, std::string> parameters;
    bool is_named;
};

// Origin node for AST (using existing OriginNode from CHTLNode.h)
// The OriginNode class is already defined in CHTLNode.h

// Origin manager for handling raw embeddings
class CHTLOriginManager {
public:
    CHTLOriginManager();
    ~CHTLOriginManager();
    
    // Raw embedding management
    bool addRawEmbedding(const RawEmbedding& embedding);
    bool removeRawEmbedding(const std::string& name, OriginType type);
    std::shared_ptr<RawEmbedding> findRawEmbedding(const std::string& name, OriginType type);
    
    // Origin processing
    std::string processOrigin(const OriginNode& origin_node);
    std::string processHtmlOrigin(const std::string& content, const std::string& name = "");
    std::string processStyleOrigin(const std::string& content, const std::string& name = "");
    std::string processJavaScriptOrigin(const std::string& content, const std::string& name = "");
    
    // Validation
    bool validateRawEmbedding(const RawEmbedding& embedding);
    bool validateOriginContent(OriginType type, const std::string& content);
    
    // Utility methods
    std::vector<std::string> getRawEmbeddingNames(OriginType type) const;
    void clearAllRawEmbeddings();
    
    // Content processing
    std::string processHtmlContent(const std::string& content);
    std::string processStyleContent(const std::string& content);
    std::string processJavaScriptContent(const std::string& content);
    
private:
    std::map<std::string, std::shared_ptr<RawEmbedding>> html_embeddings;
    std::map<std::string, std::shared_ptr<RawEmbedding>> style_embeddings;
    std::map<std::string, std::shared_ptr<RawEmbedding>> javascript_embeddings;
    
    // Helper methods
    std::string generateOriginKey(const std::string& name, OriginType type);
    std::string sanitizeHtmlContent(const std::string& content);
    std::string sanitizeStyleContent(const std::string& content);
    std::string sanitizeJavaScriptContent(const std::string& content);
    std::string escapeHtmlEntities(const std::string& content);
    std::string escapeCssEntities(const std::string& content);
    std::string escapeJavaScriptEntities(const std::string& content);
};

} // namespace CHTL

#endif // CHTL_ORIGIN_H
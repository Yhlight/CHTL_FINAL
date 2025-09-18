#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <memory>
#include "CHTLNode/BaseNode.h"

namespace CHTL {

class CHTLLoader {
public:
    CHTLLoader();
    ~CHTLLoader() = default;
    
    // 加载文件
    NodePtr loadFile(const std::string& filename);
    NodePtr loadString(const std::string& content);
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&)> handler);
    
private:
    std::function<void(const std::string&)> errorHandler_;
    
    void reportError(const std::string& message);
};

} // namespace CHTL

#endif // CHTL_LOADER_H
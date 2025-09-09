#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CHTL JS IO流
 * 负责CHTL JS文件的读写操作
 */
class CHTLJSIOStream {
public:
    CHTLJSIOStream();
    ~CHTLJSIOStream();
    
    // 文件操作
    std::string readFile(const std::string& filePath);
    bool writeFile(const std::string& filePath, const std::string& content);

private:
};

} // namespace CHTL
#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL IO流
 * 负责文件读写操作
 */
class CHTLIOStream {
public:
    CHTLIOStream();
    ~CHTLIOStream();
    
    // 文件读写
    std::string readFile(const std::string& filePath);
    bool writeFile(const std::string& filePath, const std::string& content);
    
    // 行读写
    std::vector<std::string> readLines(const std::string& filePath);
    bool writeLines(const std::string& filePath, const std::vector<std::string>& lines);

private:
};

} // namespace CHTL
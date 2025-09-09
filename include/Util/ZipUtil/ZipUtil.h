#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief ZIP工具类
 * 提供ZIP文件操作功能
 */
class ZipUtil {
public:
    ZipUtil();
    ~ZipUtil();
    
    // ZIP操作
    bool compress(const std::string& inputPath, const std::string& outputPath);
    bool decompress(const std::string& inputPath, const std::string& outputPath);
    
    // ZIP文件操作
    std::vector<std::string> listFiles(const std::string& zipPath);
    bool extractFile(const std::string& zipPath, const std::string& filePath, const std::string& outputPath);

private:
};

} // namespace CHTL
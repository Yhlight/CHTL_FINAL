#pragma once

#include "CHTLContext.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief CHTL文件加载器
 * 负责加载和保存CHTL文件
 */
class CHTLLoader {
public:
    CHTLLoader(std::shared_ptr<CHTLContext> context);
    ~CHTLLoader();
    
    // 加载文件
    std::string loadFile(const std::string& filePath);
    
    // 保存文件
    bool saveFile(const std::string& filePath, const std::string& content);
    
    // 加载目录
    std::vector<std::string> loadDirectory(const std::string& dirPath);
    
    // 文件操作
    bool fileExists(const std::string& filePath);
    std::string getFileExtension(const std::string& filePath);

private:
    std::shared_ptr<CHTLContext> m_context;
};

} // namespace CHTL
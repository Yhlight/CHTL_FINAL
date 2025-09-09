#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CHTL JS加载器
 * 负责加载CHTL JS文件
 */
class CHTLJSLoader {
public:
    CHTLJSLoader();
    ~CHTLJSLoader();
    
    // 文件加载
    std::string loadFile(const std::string& filePath);

private:
};

} // namespace CHTL
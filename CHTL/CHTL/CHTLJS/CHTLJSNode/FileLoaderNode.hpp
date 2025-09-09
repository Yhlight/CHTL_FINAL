#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 文件加载器节点
 * 
 * 根据 CHTL.md 文档第 1210-1231 行实现
 * 支持 AMD 风格 JavaScript 文件加载器
 */
class FileLoaderNode : public CHTLJSBaseNode {
public:
    FileLoaderNode(size_t line = 0, size_t column = 0);
    
    // 添加要加载的文件
    void addLoadFile(const std::string& filePath);
    
    // 获取要加载的文件列表
    const std::vector<std::string>& getLoadFiles() const { return loadFiles_; }
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::vector<std::string> loadFiles_;
    
    // 生成 AMD 加载器代码
    std::string generateAMDLoader() const;
};

} // namespace CHTL
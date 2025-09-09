#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 文件载入节点
 * 
 * 表示 fileloader {} 语法
 */
class FileLoaderNode : public CHTLJSBaseNode {
public:
    FileLoaderNode(size_t line = 0, size_t column = 0);
    
    // 添加要载入的文件
    void addFile(const std::string& filePath);
    
    // 获取所有要载入的文件
    const std::vector<std::string>& getFiles() const { return files_; }
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::vector<std::string> files_;
};

} // namespace CHTL
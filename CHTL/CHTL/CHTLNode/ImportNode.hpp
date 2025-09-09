#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 导入节点
 * 
 * 表示导入语句
 */
class ImportNode : public BaseNode {
public:
    ImportNode(const std::string& importType, size_t line = 0, size_t column = 0);
    
    // 获取导入类型
    const std::string& getImportType() const { return importType_; }
    
    // 设置导入类型
    void setImportType(const std::string& type) { importType_ = type; }
    
    // 获取导入路径
    const std::string& getImportPath() const { return importPath_; }
    
    // 设置导入路径
    void setImportPath(const std::string& path) { importPath_ = path; }
    
    // 获取导入名称
    const std::string& getImportName() const { return importName_; }
    
    // 设置导入名称
    void setImportName(const std::string& name) { importName_ = name; }
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string importType_;
    std::string importPath_;
    std::string importName_;
};

} // namespace CHTL
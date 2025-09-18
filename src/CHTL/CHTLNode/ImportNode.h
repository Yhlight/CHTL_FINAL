#ifndef IMPORT_NODE_H
#define IMPORT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class ImportNode : public BaseNode {
public:
    ImportNode(const std::string& name = "import");
    ~ImportNode() = default;
    
    // 导入路径
    void setImportPath(const std::string& path);
    const std::string& getImportPath() const { return importPath_; }
    
    // 导入类型
    void setImportType(const std::string& type);
    const std::string& getImportType() const { return importType_; }
    
    // 导入别名
    void setImportAlias(const std::string& alias);
    const std::string& getImportAlias() const { return importAlias_; }
    
    // 重写基类方法
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    
private:
    std::string importPath_;
    std::string importType_;
    std::string importAlias_;
};

} // namespace CHTL

#endif // IMPORT_NODE_H
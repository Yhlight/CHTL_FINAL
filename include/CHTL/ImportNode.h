#pragma once

#include "BaseNode.h"

namespace CHTL {

// 导入节点类
class ImportNode : public BaseNode {
private:
    String importType_;
    String importPath_;
    String importName_;
    String alias_;
    
public:
    ImportNode(const String& type, const String& path, const SourceLocation& loc = {})
        : BaseNode(NodeType::Import, loc), importType_(type), importPath_(path) {}
    
    // 导入类型和路径
    const String& getImportType() const { return importType_; }
    void setImportType(const String& type) { importType_ = type; }
    
    const String& getImportPath() const { return importPath_; }
    void setImportPath(const String& path) { importPath_ = path; }
    
    const String& getImportName() const { return importName_; }
    void setImportName(const String& name) { importName_ = name; }
    
    const String& getAlias() const { return alias_; }
    void setAlias(const String& alias) { alias_ = alias; }
    
    // 实现基类方法
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    // 工厂方法
    static ImportNodePtr create(const String& type, const String& path, const SourceLocation& loc = {});
};

} // namespace CHTL
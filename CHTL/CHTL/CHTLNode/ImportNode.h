#pragma once

#include "BaseNode.h"

namespace CHTL {

enum class ImportType {
    FILE,           // 文件导入
    CHTL_FILE,      // CHTL文件导入
    MODULE,         // 模块导入
    PRECISE,        // 精确导入
    TYPE,           // 类型导入
    WILDCARD        // 通配导入
};

enum class ImportTargetType {
    HTML,           // HTML文件
    CSS,            // CSS文件
    JS,             // JavaScript文件
    CHTL,           // CHTL文件
    CJJS,           // CHTL JS文件
    MODULE,         // 模块
    UNKNOWN         // 未知类型
};

class ImportNode : public BaseNode {
private:
    ImportType importType;
    ImportTargetType targetType;
    std::string targetPath;
    std::string alias;
    std::vector<std::string> specificImports;
    std::map<std::string, std::string> importMap;
    
    // 导入配置
    bool isRelative;
    std::string basePath;
    std::vector<std::string> searchPaths;
    
public:
    ImportNode(ImportType type, const std::string& path);
    virtual ~ImportNode() = default;
    
    // 导入类型
    ImportType getImportType() const { return importType; }
    void setImportType(ImportType type) { importType = type; }
    
    // 目标类型
    ImportTargetType getTargetType() const { return targetType; }
    void setTargetType(ImportTargetType type) { targetType = type; }
    
    // 目标路径
    const std::string& getTargetPath() const { return targetPath; }
    void setTargetPath(const std::string& path) { targetPath = path; }
    
    // 别名
    const std::string& getAlias() const { return alias; }
    void setAlias(const std::string& alias) { this->alias = alias; }
    
    // 特定导入
    void addSpecificImport(const std::string& import);
    void removeSpecificImport(const std::string& import);
    const std::vector<std::string>& getSpecificImports() const { return specificImports; }
    
    // 导入映射
    void addImportMapping(const std::string& from, const std::string& to);
    void removeImportMapping(const std::string& from);
    const std::map<std::string, std::string>& getImportMap() const { return importMap; }
    
    // 路径配置
    void setRelative(bool relative) { isRelative = relative; }
    bool isRelativePath() const { return isRelative; }
    
    void setBasePath(const std::string& path) { basePath = path; }
    const std::string& getBasePath() const { return basePath; }
    
    void addSearchPath(const std::string& path);
    const std::vector<std::string>& getSearchPaths() const { return searchPaths; }
    
    // 路径解析
    std::string resolvePath() const;
    std::string getFullPath() const;
    
    // 验证
    bool isValidPath() const;
    bool isFileExists() const;
    
    // 克隆
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 序列化
    virtual std::string toString() const override;
    virtual std::string toHTML() const override;
    virtual std::string toCSS() const override;
    virtual std::string toJS() const override;
    
    // 验证
    virtual bool isValid() const override;
    
    // 调试
    virtual std::string debugString() const override;
    
    // 静态方法
    static ImportTargetType detectTargetType(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static bool isAbsolutePath(const std::string& path);
    static std::string normalizePath(const std::string& path);
    
private:
    std::string resolveRelativePath() const;
    std::string resolveAbsolutePath() const;
    std::string findInSearchPaths() const;
};

} // namespace CHTL
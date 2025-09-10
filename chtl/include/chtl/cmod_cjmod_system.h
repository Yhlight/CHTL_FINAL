#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <set>
#include <functional>

namespace chtl {
namespace cmod_cjmod {

// 模块类型
enum class ModuleType {
    CMOD,           // CHTL模块
    CJMOD,          // CHTL JS模块
    MIXED           // 混合模块
};

// 模块信息
struct ModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    std::vector<std::string> exports;
    
    ModuleInfo(const std::string& n = "") : name(n) {}
};

// 模块导出
struct ModuleExport {
    std::string name;
    std::string type;
    std::string value;
    bool isPublic;
    
    ModuleExport(const std::string& n = "", const std::string& t = "", const std::string& v = "", bool pub = true)
        : name(n), type(t), value(v), isPublic(pub) {}
};

// CJMOD API - 语法分析
class CJMODSyntax {
public:
    static bool analyze(const std::string& code);
    static bool isObject(const std::string& code);
    static bool isFunction(const std::string& code);
    static bool isArray(const std::string& code);
    static bool isCHTLJSFunction(const std::string& code);
};

// CJMOD API - 参数处理
class CJMODArg {
public:
    static std::string bind(const std::string& arg, const std::string& value);
    static std::string fillValue(const std::string& arg, const std::string& value);
    static std::string transform(const std::string& arg, const std::string& transform);
};

// CJMOD API - 扫描器
class CJMODScanner {
public:
    CJMODScanner(const std::string& input);
    
    std::vector<std::string> scan();
    std::string scanNext();
    bool hasNext() const;
    
private:
    std::string input_;
    size_t position_;
    
    void skipWhitespace();
    char currentChar() const;
    void advance();
};

// CJMOD API - 生成器
class CJMODGenerator {
public:
    CJMODGenerator();
    
    std::string exportResult(const std::string& result);
    std::string generateCode(const std::vector<std::string>& tokens);
    void addExport(const std::string& name, const std::string& value);
    
private:
    std::map<std::string, std::string> exports_;
};

// CJMOD API - 原子参数
class CJMODAtomArg {
public:
    static std::string bind(const std::string& placeholder, const std::string& value);
    static std::string fillValue(const std::string& placeholder, const std::string& value);
    
    // 占位符类型
    static const std::string PLACEHOLDER_DOLLAR;
    static const std::string PLACEHOLDER_QUESTION;
    static const std::string PLACEHOLDER_EXCLAMATION;
    static const std::string PLACEHOLDER_UNDERSCORE;
};

// CJMOD API - CHTL JS函数
class CJMODCHTLJSFunction {
public:
    static std::string CreateCHTLJSFunction(const std::string& name, const std::string& body);
    static std::string bindVirtualObject(const std::string& function, const std::string& object);
};

// 模块结构
struct ModuleStructure {
    std::string srcPath;
    std::string infoPath;
    std::vector<std::string> sourceFiles;
    std::vector<std::string> infoFiles;
    
    ModuleStructure(const std::string& src = "src", const std::string& info = "info")
        : srcPath(src), infoPath(info) {}
};

// CMOD模块
class CMODModule {
public:
    CMODModule(const std::string& name);
    
    void setInfo(const ModuleInfo& info);
    void addExport(const ModuleExport& exp);
    void addSourceFile(const std::string& filePath);
    void addInfoFile(const std::string& filePath);
    
    ModuleInfo getInfo() const;
    std::vector<ModuleExport> getExports() const;
    std::vector<std::string> getSourceFiles() const;
    std::vector<std::string> getInfoFiles() const;
    
    std::string generateModuleCode() const;
    std::string generateInfoCode() const;
    
private:
    std::string name_;
    ModuleInfo info_;
    std::vector<ModuleExport> exports_;
    std::vector<std::string> sourceFiles_;
    std::vector<std::string> infoFiles_;
};

// CJMOD模块
class CJMODModule {
public:
    CJMODModule(const std::string& name);
    
    void setInfo(const ModuleInfo& info);
    void addExport(const ModuleExport& exp);
    void addSourceFile(const std::string& filePath);
    void addInfoFile(const std::string& filePath);
    
    ModuleInfo getInfo() const;
    std::vector<ModuleExport> getExports() const;
    std::vector<std::string> getSourceFiles() const;
    std::vector<std::string> getInfoFiles() const;
    
    std::string generateModuleCode() const;
    std::string generateInfoCode() const;
    
    // CJMOD特定功能
    void addCHTLJSFunction(const std::string& name, const std::string& body);
    void addVirtualObject(const std::string& name, const std::string& object);
    
private:
    std::string name_;
    ModuleInfo info_;
    std::vector<ModuleExport> exports_;
    std::vector<std::string> sourceFiles_;
    std::vector<std::string> infoFiles_;
    std::map<std::string, std::string> chtljsFunctions_;
    std::map<std::string, std::string> virtualObjects_;
};

// 混合模块
class MixedModule {
public:
    MixedModule(const std::string& name);
    
    void setInfo(const ModuleInfo& info);
    void addCMODModule(std::shared_ptr<CMODModule> module);
    void addCJMODModule(std::shared_ptr<CJMODModule> module);
    
    ModuleInfo getInfo() const;
    std::vector<std::shared_ptr<CMODModule>> getCMODModules() const;
    std::vector<std::shared_ptr<CJMODModule>> getCJMODModules() const;
    
    std::string generateModuleCode() const;
    std::string generateInfoCode() const;
    
private:
    std::string name_;
    ModuleInfo info_;
    std::vector<std::shared_ptr<CMODModule>> cmodModules_;
    std::vector<std::shared_ptr<CJMODModule>> cjmodModules_;
};

// 官方模块
class OfficialModule {
public:
    static std::shared_ptr<CMODModule> createChthollyCMOD();
    static std::shared_ptr<CJMODModule> createChthollyCJMOD();
    static std::shared_ptr<CMODModule> createYuigahamaCMOD();
    
    static std::string getChthollyFeatures();
    static std::string getYuigahamaFeatures();
};

// 模块管理器
class ModuleManager {
public:
    ModuleManager();
    
    void addModule(std::shared_ptr<CMODModule> module);
    void addModule(std::shared_ptr<CJMODModule> module);
    void addModule(std::shared_ptr<MixedModule> module);
    
    std::shared_ptr<CMODModule> getCMODModule(const std::string& name);
    std::shared_ptr<CJMODModule> getCJMODModule(const std::string& name);
    std::shared_ptr<MixedModule> getMixedModule(const std::string& name);
    
    std::string generateAllModules() const;
    std::string generateModuleInfo() const;
    
    void clear();
    size_t getModuleCount() const;
    
private:
    std::map<std::string, std::shared_ptr<CMODModule>> cmodModules_;
    std::map<std::string, std::shared_ptr<CJMODModule>> cjmodModules_;
    std::map<std::string, std::shared_ptr<MixedModule>> mixedModules_;
    
    std::string processModuleDependencies(const std::string& content);
    std::string validateModuleStructure(const std::string& moduleName, ModuleType type);
};

} // namespace cmod_cjmod
} // namespace chtl
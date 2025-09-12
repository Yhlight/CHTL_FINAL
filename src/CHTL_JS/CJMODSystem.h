#ifndef CJMOD_SYSTEM_H
#define CJMOD_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

/**
 * 原子参数类型
 */
enum class AtomArgType {
    PLACEHOLDER,        // $ 占位符
    OPTIONAL,           // $? 可选占位符
    REQUIRED,           // $! 必须占位符
    UNORDERED,          // $_ 无序占位符
    VARIADIC,           // ... 不定参数占位符
    COMBINED            // 组合占位符 (如 $!_)
};

/**
 * 原子参数
 */
class AtomArg {
public:
    AtomArgType type;
    std::string value;
    std::function<std::string(const std::string&)> binder;
    bool filled;
    
    AtomArg(AtomArgType t, const std::string& v = "") 
        : type(t), value(v), filled(false) {}
    
    void bind(std::function<std::string(const std::string&)> func);
    void fillValue(const std::string& val);
    void fillValue(int val);
    std::string getValue() const;
};

/**
 * 参数列表
 */
class Arg {
public:
    std::vector<AtomArg> args;
    std::map<std::string, int> matchCount;
    
    Arg() = default;
    Arg(const std::vector<std::string>& values);
    
    void addArg(const AtomArg& arg);
    void bind(const std::string& pattern, std::function<std::string(const std::string&)> func);
    std::string match(const std::string& pattern, std::function<std::string(const std::string&)> func);
    void fillValue(const Arg& result);
    void transform(const std::string& template);
    void print() const;
    
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    size_t size() const;
};

/**
 * 语法分析类
 */
class Syntax {
public:
    /**
     * 分析语法，返回参数列表
     */
    static Arg analyze(const std::string& syntax);
    
    /**
     * 判断是否是JS对象
     */
    static bool isObject(const std::string& code);
    
    /**
     * 判断是否是JS函数
     */
    static bool isFunction(const std::string& code);
    
    /**
     * 判断是否是JS数组
     */
    static bool isArray(const std::string& code);
    
    /**
     * 判断是否是CHTL JS函数
     */
    static bool isCHTLJSFunction(const std::string& code);
};

/**
 * CJMOD扫描器
 */
class CJMODScanner {
public:
    /**
     * 扫描语法片段
     */
    static Arg scan(const Arg& args, const std::string& keyword);
    
    /**
     * 双指针扫描
     */
    static Arg dualPointerScan(const std::string& code, const std::string& keyword);
    
    /**
     * 前置截取
     */
    static std::string preprocessCode(const std::string& code, const std::string& keyword);
};

/**
 * CJMOD生成器
 */
class CJMODGenerator {
public:
    /**
     * 导出最终的JS代码
     */
    static std::string exportResult(const Arg& args);
    
    /**
     * 生成函数定义
     */
    static std::string generateFunction(const std::string& name, 
                                      const std::vector<std::string>& parameters, 
                                      const std::string& body);
    
    /**
     * 生成变量声明
     */
    static std::string generateVariable(const std::string& name, 
                                      const std::string& value, 
                                      bool isConst = false);
};

/**
 * CHTL JS函数
 */
class CHTLJSFunction {
public:
    std::string name;
    std::string syntax;
    std::vector<std::string> parameters;
    std::string body;
    bool supportsVirtualObject;
    
    CHTLJSFunction(const std::string& n, const std::string& s) 
        : name(n), syntax(s), supportsVirtualObject(false) {}
    
    /**
     * 创建CHTL JS函数
     */
    static CHTLJSFunction createCHTLJSFunction(const std::string& syntax);
    
    /**
     * 绑定虚对象支持
     */
    static void bindVirtualObject(const std::string& functionName);
    
    /**
     * 生成函数代码
     */
    std::string generateCode() const;
    
    /**
     * 验证语法
     */
    bool validateSyntax() const;
};

/**
 * CJMOD模块
 */
class CJMODModule {
public:
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
    
    std::vector<CHTLJSFunction> functions;
    std::map<std::string, std::string> exports;
    
    CJMODModule(const std::string& n) : name(n) {}
    
    /**
     * 添加函数
     */
    void addFunction(const CHTLJSFunction& func);
    
    /**
     * 添加导出
     */
    void addExport(const std::string& name, const std::string& value);
    
    /**
     * 生成模块代码
     */
    std::string generateModuleCode() const;
    
    /**
     * 验证模块
     */
    bool validateModule() const;
};

/**
 * CJMOD系统管理器
 */
class CJMODSystem {
public:
    CJMODSystem();
    ~CJMODSystem();
    
    /**
     * 注册CJMOD模块
     */
    void registerModule(std::unique_ptr<CJMODModule> module);
    
    /**
     * 获取CJMOD模块
     */
    CJMODModule* getModule(const std::string& name);
    
    /**
     * 获取所有模块
     */
    const std::map<std::string, std::unique_ptr<CJMODModule>>& getAllModules() const;
    
    /**
     * 处理CJMOD导入
     */
    std::string processImport(const std::string& moduleName);
    
    /**
     * 处理CJMOD函数调用
     */
    std::string processFunctionCall(const std::string& functionName, 
                                  const std::map<std::string, std::string>& parameters);
    
    /**
     * 验证CJMOD模块
     */
    bool validateModule(const std::string& moduleName);
    
    /**
     * 清空所有模块
     */
    void clear();

private:
    std::map<std::string, std::unique_ptr<CJMODModule>> modules;
    
    /**
     * 解析模块信息
     */
    std::map<std::string, std::string> parseModuleInfo(const std::string& info);
    
    /**
     * 处理模块依赖
     */
    std::vector<std::string> resolveDependencies(const std::string& moduleName);
    
    /**
     * 检测循环依赖
     */
    bool detectCircularDependency(const std::string& moduleName);
    
    /**
     * 检查版本兼容性
     */
    bool checkVersionCompatibility(const std::string& moduleName);
};

} // namespace CHTL

#endif // CJMOD_SYSTEM_H
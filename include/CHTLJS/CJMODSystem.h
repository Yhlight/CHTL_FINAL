#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#include <functional>

namespace CHTL {

/**
 * CJMOD API - 语法分析类
 */
class Syntax {
public:
    // 参数类型
    struct Arg {
        std::string value;
        std::string type;
        std::function<std::string(const std::string&)> binder;
        bool is_placeholder;
        bool is_optional;
        bool is_required;
        bool is_unordered;
        
        Arg();
        Arg(const std::string& val);
        
        void bind(std::function<std::string(const std::string&)> func);
        void fillValue(const std::string& val);
        std::string getValue() const;
    };
    
    // 语法分析
    static std::vector<Arg> analyze(const std::string& syntax);
    
    // 类型判断
    static bool isObject(const std::string& code);
    static bool isFunction(const std::string& code);
    static bool isArray(const std::string& code);
    static bool isCHTLJSFunction(const std::string& code);
    static std::vector<std::string> tokenize(const std::string& syntax);
    
private:
    static bool isPlaceholder(const std::string& token);
    static std::string extractPlaceholderType(const std::string& token);
};

/**
 * CJMOD API - 参数列表类
 */
class Arg {
public:
    Arg();
    Arg(const std::vector<Syntax::Arg>& args);
    
    // 参数操作
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    std::string match(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    void fillValue(const std::vector<Syntax::Arg>& values);
    void transform(const std::string& template_str);
    
    // 访问器
    Syntax::Arg& operator[](size_t index);
    const Syntax::Arg& operator[](size_t index) const;
    size_t size() const;
    
    // 输出
    void print() const;
    std::string toString() const;

private:
    std::vector<Syntax::Arg> args_;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> binders_;
};

/**
 * CJMOD API - 扫描器接口
 */
class CJMODScanner {
public:
    static Arg scan(const Arg& args, const std::string& keyword);
    static Arg scan(const Arg& args);
    
private:
    static std::string extractCodeFragment(const std::string& code, const std::string& keyword);
    static bool isKeywordMatch(const std::string& code, const std::string& keyword);
};

/**
 * CJMOD API - 生成器接口
 */
class CJMODGenerator {
public:
    static std::string exportResult(const Arg& args);
    static std::string generateJS(const Arg& args);
    
private:
    static std::string processTemplate(const std::string& template_str, const Arg& args);
    static std::string escapeJSString(const std::string& str);
};

/**
 * CJMOD API - 原子参数
 */
class AtomArg {
public:
    enum class Type {
        PLACEHOLDER,        // $
        OPTIONAL,           // $?
        REQUIRED,           // $!
        UNORDERED,          // $_
        VARIADIC,           // ...
        COMBINED            // 组合类型
    };
    
    AtomArg();
    AtomArg(const std::string& value, Type type);
    
    // 绑定和填充
    void bind(std::function<std::string(const std::string&)> func);
    void fillValue(const std::string& value);
    void fillValue(int value);
    
    // 访问器
    std::string getValue() const;
    Type getType() const;
    bool isPlaceholder() const;
    bool isOptional() const;
    bool isRequired() const;
    bool isUnordered() const;
    bool isVariadic() const;

private:
    std::string value_;
    Type type_;
    std::function<std::string(const std::string&)> binder_;
};

/**
 * CJMOD API - CHTL JS函数
 */
class CHTLJSFunction {
public:
    CHTLJSFunction();
    CHTLJSFunction(const std::string& name, const std::string& syntax);
    
    // 创建CHTL JS函数
    static CHTLJSFunction createCHTLJSFunction(const std::string& syntax);
    
    // 绑定虚对象
    static void bindVirtualObject(const std::string& functionName);
    
    // 函数操作
    void setName(const std::string& name);
    std::string getName() const;
    
    void setSyntax(const std::string& syntax);
    std::string getSyntax() const;
    
    void addParameter(const std::string& name, const std::string& type);
    std::vector<std::pair<std::string, std::string>> getParameters() const;
    
    void setBody(const std::string& body);
    std::string getBody() const;
    
    // 生成JS代码
    std::string generateJS() const;
    
    // 验证
    bool validate() const;

private:
    std::string name_;
    std::string syntax_;
    std::vector<std::pair<std::string, std::string>> parameters_;
    std::string body_;
    bool supports_virtual_object_;
    
    static std::unordered_map<std::string, CHTLJSFunction> function_registry_;
};

/**
 * CJMOD系统
 * 管理CJMOD模块的加载、解析和生成
 */
class CJMODSystem {
public:
    CJMODSystem();
    ~CJMODSystem();
    
    // 模块管理
    bool loadModule(const std::string& path);
    bool unloadModule(const std::string& name);
    bool hasModule(const std::string& name) const;
    
    // 函数注册
    void registerFunction(const std::string& name, const CHTLJSFunction& func);
    CHTLJSFunction* getFunction(const std::string& name);
    bool hasFunction(const std::string& name) const;
    
    // 语法扩展
    void addSyntaxExtension(const std::string& keyword, const std::string& handler);
    std::string getSyntaxExtension(const std::string& keyword) const;
    bool hasSyntaxExtension(const std::string& keyword) const;
    
    // 代码生成
    std::string generateJS(const std::string& chtl_js_code);
    
    // 配置
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    
    // 调试
    void setDebugMode(bool enabled);
    bool isDebugMode() const;

private:
    // 模块存储
    std::unordered_map<std::string, std::string> modules_;
    
    // 函数注册表
    std::unordered_map<std::string, CHTLJSFunction> functions_;
    
    // 语法扩展
    std::unordered_map<std::string, std::string> syntax_extensions_;
    
    // 配置
    std::unordered_map<std::string, std::string> configurations_;
    
    // 调试模式
    bool debug_mode_;
    
    // 扫描算法
    std::vector<std::string> doublePointerScan(const std::string& code, const std::string& keyword);
    std::string preemptiveCapture(const std::string& code, const std::string& keyword);
};

} // namespace CHTL
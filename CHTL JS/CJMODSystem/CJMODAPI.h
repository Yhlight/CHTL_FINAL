#ifndef CJMODAPI_H
#define CJMODAPI_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

namespace CHTLJS {

/**
 * 原子参数类
 */
class AtomArg {
public:
    enum class ArgType {
        PLACEHOLDER,        // $
        OPTIONAL_PLACEHOLDER, // $?
        REQUIRED_PLACEHOLDER, // $!
        UNORDERED_PLACEHOLDER, // $_
        VARIADIC_PLACEHOLDER,  // ...
        UNKNOWN
    };
    
    AtomArg(ArgType type, const std::string& value = "");
    ~AtomArg();
    
    ArgType getType() const;
    const std::string& getValue() const;
    void setValue(const std::string& value);
    
    void bind(std::function<std::string(const std::string&)> func);
    void fillValue(const std::string& value);
    void fillValue(int value);
    
    std::string toString() const;

private:
    ArgType type_;
    std::string value_;
    std::function<std::string(const std::string&)> binder_;
};

/**
 * 参数列表类
 */
class Arg {
public:
    Arg();
    Arg(const std::vector<std::string>& values);
    ~Arg();
    
    void add(const std::string& value);
    void add(AtomArg* arg);
    size_t size() const;
    
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    
    void bind(const std::string& key, std::function<std::string(const std::string&)> func);
    std::string match(const std::string& key, std::function<std::string(const std::string&)> func);
    void fillValue(const Arg& other);
    void transform(const std::string& template_str);
    
    void print() const;
    std::string toString() const;

private:
    std::vector<std::shared_ptr<AtomArg>> args_;
    std::map<std::string, std::function<std::string(const std::string&)>> binders_;
};

/**
 * 语法分析类
 */
class Syntax {
public:
    static Arg analyze(const std::string& syntax);
    static bool isObject(const std::string& str);
    static bool isFunction(const std::string& str);
    static bool isArray(const std::string& str);
    static bool isCHTLJSFunction(const std::string& str);

private:
    static std::vector<std::string> tokenize(const std::string& syntax);
    static bool isBalanced(const std::string& str);
};

/**
 * CJMOD扫描器
 */
class CJMODScanner {
public:
    static Arg scan(const Arg& args, const std::string& keyword);
    static Arg scan(const std::string& source, const std::string& keyword);

private:
    static std::vector<std::string> findKeywordPositions(const std::string& source, const std::string& keyword);
    static std::string extractContext(const std::string& source, size_t pos, size_t length);
};

/**
 * CJMOD生成器
 */
class CJMODGenerator {
public:
    static void exportResult(const Arg& args);
    static std::string generateCode(const Arg& args);

private:
    static std::string processTemplate(const std::string& template_str, const Arg& args);
};

/**
 * CHTL JS函数类
 */
class CHTLJSFunction {
public:
    static std::shared_ptr<CHTLJSFunction> CreateCHTLJSFunction(const std::string& syntax);
    static void bindVirtualObject(const std::string& functionName);
    
    std::string generateCode() const;
    void addParameter(const std::string& key, const std::string& value);
    void addParameter(const std::string& key, std::shared_ptr<CHTLJSBaseNode> value);
    
    const std::string& getFunctionName() const;
    const std::map<std::string, std::string>& getStringParameters() const;
    const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& getNodeParameters() const;

private:
    CHTLJSFunction(const std::string& functionName);
    
    std::string function_name_;
    std::map<std::string, std::string> string_parameters_;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> node_parameters_;
    std::string syntax_template_;
};

} // namespace CHTLJS

#endif // CJMODAPI_H
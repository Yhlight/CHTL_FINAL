#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>

namespace CHTL {

// CJMOD API - CHTL JS模块的扩展组件

class AtomArg {
public:
    enum class Type {
        PLACEHOLDER,        // $
        OPTIONAL_PLACEHOLDER, // $?
        REQUIRED_PLACEHOLDER, // $!
        UNORDERED_PLACEHOLDER, // $_
        VARIADIC_PLACEHOLDER,  // ...
        VALUE
    };
    
    Type type;
    std::string value;
    std::function<std::string(const std::string&)> binder;
    
    AtomArg() : type(Type::PLACEHOLDER) {}
    AtomArg(Type t, const std::string& v = "") : type(t), value(v) {}
    
    void bind(std::function<std::string(const std::string&)> func) {
        binder = func;
    }
    
    void fillValue(const std::string& val) {
        value = val;
    }
    
    std::string getValue() const {
        if (binder) {
            return binder(value);
        }
        return value;
    }
};

class Arg {
public:
    std::vector<AtomArg> args;
    
    Arg() = default;
    Arg(const std::vector<std::string>& values);
    
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    std::string match(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    void fillValue(const Arg& result);
    void transform(const std::string& template_str);
    void print() const;
    
    AtomArg& operator[](size_t index) { return args[index]; }
    const AtomArg& operator[](size_t index) const { return args[index]; }
    size_t size() const { return args.size(); }
};

class Syntax {
public:
    static Arg analyze(const std::string& syntax);
    static bool isObject(const std::string& content);
    static bool isFunction(const std::string& content);
    static bool isArray(const std::string& content);
    static bool isCHTLJSFunction(const std::string& content);
};

class CJMODScanner {
public:
    static Arg scan(const Arg& args, const std::string& keyword);
};

class CJMODGenerator {
public:
    static void exportResult(const Arg& args);
};

class CHTLJSFunction {
public:
    static CHTLJSFunction CreateCHTLJSFunction(const std::string& syntax);
    static void bindVirtualObject(const std::string& functionName);
    
    std::string generate(const std::vector<std::string>& args);
    
private:
    std::string syntax_;
    std::vector<std::string> parameters_;
    bool supportsVirtualObject_;
};

} // namespace CHTL
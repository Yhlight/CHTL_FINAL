#pragma once

#include "CHTL/Common.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace CHTL {

/**
 * CJMOD API - CHTL JS模块扩展API
 */
class CJMODAPI {
public:
    // 原子参数类型
    enum class AtomArgType {
        PLACEHOLDER,        // $
        OPTIONAL,           // $?
        REQUIRED,           // $!
        UNORDERED,          // $_
        VARIADIC,           // ...
        COMBINED            // 组合类型，如 $!_
    };

    // 原子参数
    class AtomArg {
    public:
        AtomArgType type;
        std::string value;
        std::function<std::string(const std::string&)> binder;
        bool isFilled = false;
        
        AtomArg(AtomArgType t, const std::string& v = "") : type(t), value(v) {}
        
        void bind(std::function<std::string(const std::string&)> binder);
        void fillValue(const std::string& value);
        void fillValue(int value);
        void fillValue(double value);
        std::string getValue() const;
        bool isPlaceholder() const;
        bool isOptional() const;
        bool isRequired() const;
        bool isUnordered() const;
        bool isVariadic() const;
    };

    // 参数列表
    class Arg {
    public:
        std::vector<AtomArg> args;
        
        Arg() = default;
        Arg(const std::vector<std::string>& values);
        
        void add(const AtomArg& arg);
        void add(const std::string& value);
        void bind(const std::string& pattern, std::function<std::string(const std::string&)> binder);
        std::string match(const std::string& pattern, std::function<std::string(const std::string&)> matcher);
        void fillValue(const Arg& other);
        void fillValue(const std::vector<std::string>& values);
        void transform(const std::string& template);
        void print() const;
        
        AtomArg& operator[](size_t index);
        const AtomArg& operator[](size_t index) const;
        size_t size() const;
        bool empty() const;
    };

    // 语法分析器
    class Syntax {
    public:
        /**
         * 分析语法
         * @param code 代码片段
         * @return 参数列表
         */
        static Arg analyze(const std::string& code);
        
        /**
         * 判断是否是JS对象
         * @param code 代码
         * @return 是否是JS对象
         */
        static bool isObject(const std::string& code);
        
        /**
         * 判断是否是JS函数
         * @param code 代码
         * @return 是否是JS函数
         */
        static bool isFunction(const std::string& code);
        
        /**
         * 判断是否是JS数组
         * @param code 代码
         * @return 是否是JS数组
         */
        static bool isArray(const std::string& code);
        
        /**
         * 判断是否是CHTL JS函数
         * @param code 代码
         * @return 是否是CHTL JS函数
         */
        static bool isCHTLJSFunction(const std::string& code);
        
        /**
         * 解析占位符
         * @param code 代码
         * @return 占位符列表
         */
        static std::vector<std::string> parsePlaceholders(const std::string& code);
        
        /**
         * 验证语法
         * @param code 代码
         * @return 验证结果
         */
        static bool validate(const std::string& code);
    };

    // CJMOD扫描器
    class CJMODScanner {
    public:
        /**
         * 扫描语法片段
         * @param args 参数列表
         * @param keyword 关键字
         * @return 扫描结果
         */
        static Arg scan(const Arg& args, const std::string& keyword);
        
        /**
         * 扫描代码片段
         * @param code 代码
         * @param keyword 关键字
         * @return 扫描结果
         */
        static Arg scan(const std::string& code, const std::string& keyword);
        
        /**
         * 双指针扫描
         * @param code 代码
         * @param keyword 关键字
         * @return 扫描结果
         */
        static Arg dualPointerScan(const std::string& code, const std::string& keyword);
        
        /**
         * 前置截取扫描
         * @param code 代码
         * @param keyword 关键字
         * @return 扫描结果
         */
        static Arg preprocessScan(const std::string& code, const std::string& keyword);
    };

    // CJMOD生成器
    class CJMODGenerator {
    public:
        /**
         * 导出结果
         * @param args 参数列表
         * @return 生成的代码
         */
        static std::string exportResult(const Arg& args);
        
        /**
         * 导出结果到文件
         * @param args 参数列表
         * @param filePath 文件路径
         * @return 是否成功
         */
        static bool exportResultToFile(const Arg& args, const std::string& filePath);
        
        /**
         * 生成函数定义
         * @param functionName 函数名
         * @param args 参数列表
         * @return 函数定义代码
         */
        static std::string generateFunction(const std::string& functionName, const Arg& args);
        
        /**
         * 生成类定义
         * @param className 类名
         * @param args 参数列表
         * @return 类定义代码
         */
        static std::string generateClass(const std::string& className, const Arg& args);
    };

    // CHTL JS函数
    class CHTLJSFunction {
    public:
        std::string name;
        std::string definition;
        std::map<std::string, std::string> parameters;
        bool supportsVirtualObject = false;
        
        CHTLJSFunction(const std::string& n, const std::string& def) : name(n), definition(def) {}
        
        /**
         * 创建CHTL JS函数
         * @param definition 函数定义
         * @return 函数对象
         */
        static CHTLJSFunction CreateCHTLJSFunction(const std::string& definition);
        
        /**
         * 绑定虚对象
         * @param functionName 函数名
         * @return 是否成功
         */
        static bool bindVirtualObject(const std::string& functionName);
        
        /**
         * 解析函数参数
         * @param functionCall 函数调用
         * @return 参数映射
         */
        std::map<std::string, std::string> parseParameters(const std::string& functionCall);
        
        /**
         * 生成函数调用代码
         * @param parameters 参数映射
         * @return 生成的代码
         */
        std::string generateCall(const std::map<std::string, std::string>& parameters);
        
        /**
         * 生成虚对象代码
         * @param objectName 对象名
         * @param parameters 参数映射
         * @return 生成的代码
         */
        std::string generateVirtualObject(const std::string& objectName, 
                                        const std::map<std::string, std::string>& parameters);
    };

    // CJMOD管理器
    class CJMODManager {
    public:
        static CJMODManager& getInstance();
        
        /**
         * 注册CJMOD函数
         * @param function 函数对象
         * @return 是否成功
         */
        bool registerFunction(const CHTLJSFunction& function);
        
        /**
         * 获取CJMOD函数
         * @param functionName 函数名
         * @return 函数对象
         */
        CHTLJSFunction getFunction(const std::string& functionName);
        
        /**
         * 检查函数是否存在
         * @param functionName 函数名
         * @return 是否存在
         */
        bool hasFunction(const std::string& functionName);
        
        /**
         * 列出所有函数
         * @return 函数名列表
         */
        std::vector<std::string> listFunctions();
        
        /**
         * 清除所有函数
         */
        void clearFunctions();
        
        /**
         * 加载CJMOD模块
         * @param modulePath 模块路径
         * @return 是否成功
         */
        bool loadModule(const std::string& modulePath);
        
        /**
         * 卸载CJMOD模块
         * @param moduleName 模块名
         * @return 是否成功
         */
        bool unloadModule(const std::string& moduleName);

    private:
        CJMODManager() = default;
        std::map<std::string, CHTLJSFunction> functions_;
        std::map<std::string, std::string> modulePaths_;
    };

    // 工具函数
    class Utils {
    public:
        /**
         * 创建占位符
         * @param type 类型
         * @param index 索引
         * @return 占位符
         */
        static std::string createPlaceholder(const std::string& type, size_t index);
        
        /**
         * 解析占位符
         * @param placeholder 占位符
         * @return 解析结果
         */
        static std::pair<std::string, size_t> parsePlaceholder(const std::string& placeholder);
        
        /**
         * 验证占位符
         * @param placeholder 占位符
         * @return 是否有效
         */
        static bool validatePlaceholder(const std::string& placeholder);
        
        /**
         * 转义字符串
         * @param str 字符串
         * @return 转义后的字符串
         */
        static std::string escapeString(const std::string& str);
        
        /**
         * 反转义字符串
         * @param str 字符串
         * @return 反转义后的字符串
         */
        static std::string unescapeString(const std::string& str);
    };

private:
    // 内部工具函数
    static std::string parseAtomArgType(const std::string& pattern);
    static bool isPlaceholderPattern(const std::string& pattern);
    static bool isOptionalPattern(const std::string& pattern);
    static bool isRequiredPattern(const std::string& pattern);
    static bool isUnorderedPattern(const std::string& pattern);
    static bool isVariadicPattern(const std::string& pattern);
};

} // namespace CHTL
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {

/**
 * @brief CJMOD API - CHTL JS模块扩展API
 * 提供强大的CHTL JS语法扩展能力
 */

/**
 * @brief 原子参数类型
 */
enum class AtomArgType {
    PLACEHOLDER,        // $ 占位符
    OPTIONAL,           // $? 可选占位符
    REQUIRED,           // $! 必须占位符
    UNORDERED,          // $_ 无序占位符
    VARIADIC            // ... 不定参数占位符
};

/**
 * @brief 原子参数类
 */
class AtomArg {
public:
    AtomArg(AtomArgType type, const std::string& value = "");
    ~AtomArg() = default;

    AtomArgType getType() const { return m_type; }
    const std::string& getValue() const { return m_value; }
    void setValue(const std::string& value) { m_value = value; }
    
    bool isPlaceholder() const;
    bool isOptional() const;
    bool isRequired() const;
    bool isUnordered() const;
    bool isVariadic() const;
    
    void bind(std::function<std::string(const std::string&)> binder);
    void fillValue(const std::string& value);
    void fillValue(int value);
    
    std::string getBoundValue() const;

private:
    AtomArgType m_type;
    std::string m_value;
    std::function<std::string(const std::string&)> m_binder;
    bool m_hasValue;
};

/**
 * @brief 参数列表类
 */
class Arg {
public:
    Arg();
    ~Arg() = default;

    void addAtom(const AtomArg& atom);
    void addAtom(AtomArgType type, const std::string& value = "");
    
    size_t size() const { return m_atoms.size(); }
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    
    void bind(const std::string& key, std::function<std::string(const std::string&)> binder);
    void fillValue(const Arg& result);
    void transform(const std::string& template_str);
    
    void print() const;
    
    std::string getTransformedResult() const { return m_transformedResult; }

private:
    std::vector<AtomArg> m_atoms;
    std::string m_transformedResult;
};

/**
 * @brief 语法分析类
 */
class Syntax {
public:
    /**
     * @brief 分析语法，返回参数列表
     * @param syntax 语法字符串
     * @return 解析出的参数列表
     */
    static Arg analyze(const std::string& syntax);
    
    /**
     * @brief 判断是否是JS对象
     * @param code 代码字符串
     * @return 是否是JS对象
     */
    static bool isObject(const std::string& code);
    
    /**
     * @brief 判断是否是JS函数
     * @param code 代码字符串
     * @return 是否是JS函数
     */
    static bool isFunction(const std::string& code);
    
    /**
     * @brief 判断是否是JS数组
     * @param code 代码字符串
     * @return 是否是JS数组
     */
    static bool isArray(const std::string& code);
    
    /**
     * @brief 判断是否是CHTL JS函数
     * @param code 代码字符串
     * @return 是否是CHTL JS函数
     */
    static bool isCHTLJSFunction(const std::string& code);

private:
    static bool isBalanced(const std::string& code, char open, char close);
    static bool hasValidObjectSyntax(const std::string& code);
    static bool hasValidFunctionSyntax(const std::string& code);
    static bool hasValidArraySyntax(const std::string& code);
    static bool hasValidCHTLJSFunctionSyntax(const std::string& code);
};

/**
 * @brief CJMOD扫描器
 */
class CJMODScanner {
public:
    /**
     * @brief 扫描语法片段
     * @param args 参数列表
     * @param keyword 扫描的关键字
     * @return 扫描结果
     */
    static Arg scan(const Arg& args, const std::string& keyword);
    
    /**
     * @brief 扫描代码片段
     * @param code 代码片段
     * @param keyword 关键字
     * @return 扫描结果
     */
    static Arg scanCode(const std::string& code, const std::string& keyword);

private:
    static std::vector<std::string> splitByKeyword(const std::string& code, const std::string& keyword);
    static bool isKeywordMatch(const std::string& text, const std::string& keyword);
};

/**
 * @brief CJMOD生成器
 */
class CJMODGenerator {
public:
    /**
     * @brief 导出最终结果
     * @param args 参数列表
     * @return 生成的代码
     */
    static std::string exportResult(const Arg& args);
    
    /**
     * @brief 导出到文件
     * @param args 参数列表
     * @param filePath 文件路径
     */
    static void exportToFile(const Arg& args, const std::string& filePath);

private:
    static std::string processTemplate(const std::string& template_str, const Arg& args);
};

/**
 * @brief CHTL JS函数类
 */
class CHTLJSFunction {
public:
    /**
     * @brief 创建CHTL JS函数
     * @param syntax 函数语法
     * @return 函数实例
     */
    static std::shared_ptr<CHTLJSFunction> CreateCHTLJSFunction(const std::string& syntax);
    
    /**
     * @brief 绑定虚对象
     * @param functionName 函数名称
     */
    static void bindVirtualObject(const std::string& functionName);
    
    /**
     * @brief 检查是否支持虚对象
     * @param functionName 函数名称
     * @return 是否支持
     */
    static bool supportsVirtualObject(const std::string& functionName);
    
    /**
     * @brief 获取函数语法
     * @return 语法字符串
     */
    const std::string& getSyntax() const { return m_syntax; }
    
    /**
     * @brief 设置函数语法
     * @param syntax 语法字符串
     */
    void setSyntax(const std::string& syntax) { m_syntax = syntax; }
    
    /**
     * @brief 生成JavaScript代码
     * @param args 参数
     * @return 生成的JS代码
     */
    std::string generateJS(const std::vector<std::string>& args) const;

private:
    std::string m_syntax;
    bool m_supportsVirtualObject;
    
    static std::unordered_map<std::string, std::shared_ptr<CHTLJSFunction>> s_functions;
    static std::unordered_map<std::string, bool> s_virtualObjectSupport;
    
    CHTLJSFunction(const std::string& syntax);
    std::string processArguments(const std::vector<std::string>& args) const;
};

/**
 * @brief 扫描算法辅助类
 */
class ScanAlgorithm {
public:
    /**
     * @brief 双指针扫描
     * @param code 代码
     * @param keyword 关键字
     * @return 扫描结果
     */
    static std::vector<std::string> doublePointerScan(const std::string& code, const std::string& keyword);
    
    /**
     * @brief 前置截取
     * @param code 代码
     * @param keyword 关键字
     * @return 截取结果
     */
    static std::string preExtract(const std::string& code, const std::string& keyword);

private:
    static bool isKeywordAtPosition(const std::string& code, size_t pos, const std::string& keyword);
    static size_t findKeywordStart(const std::string& code, const std::string& keyword);
    static size_t findKeywordEnd(const std::string& code, size_t start, const std::string& keyword);
};

} // namespace CHTL
#ifndef CJMOD_CHTLJSFUNCTION_H
#define CJMOD_CHTLJSFUNCTION_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace CJMOD {

/**
 * CHTL JS 函数类
 * 用于 CJMOD API 的接口，支持虚对象和语法扩展
 */
class CHTLJSFunction {
public:
    /**
     * 构造函数
     * @param name 函数名称
     * @param syntax 语法字符串
     */
    CHTLJSFunction(const std::string& name, const std::string& syntax);
    
    /**
     * 析构函数
     */
    ~CHTLJSFunction() = default;
    
    /**
     * 创建 CHTL JS 函数
     * @param syntax 语法字符串
     * @return CHTL JS 函数对象
     */
    static CHTLJSFunction CreateCHTLJSFunction(const std::string& syntax);
    
    /**
     * 绑定虚对象 vir
     * @param functionName 函数名称
     */
    static void bindVirtualObject(const std::string& functionName);
    
    /**
     * 获取函数名称
     * @return 函数名称
     */
    const std::string& getName() const { return m_name; }
    
    /**
     * 获取语法字符串
     * @return 语法字符串
     */
    const std::string& getSyntax() const { return m_syntax; }
    
    /**
     * 检查是否支持虚对象
     * @return 是否支持虚对象
     */
    bool supportsVirtualObject() const { return m_supportsVirtualObject; }
    
    /**
     * 检查是否支持无序键值对
     * @return 是否支持无序键值对
     */
    bool supportsUnorderedKeyValue() const { return m_supportsUnorderedKeyValue; }
    
    /**
     * 检查是否支持可选键值对
     * @return 是否支持可选键值对
     */
    bool supportsOptionalKeyValue() const { return m_supportsOptionalKeyValue; }
    
    /**
     * 检查是否支持无修饰字面量
     * @return 是否支持无修饰字面量
     */
    bool supportsUnquotedLiterals() const { return m_supportsUnquotedLiterals; }
    
    /**
     * 添加参数绑定
     * @param placeholder 占位符
     * @param handler 处理函数
     */
    void addParameterBinding(const std::string& placeholder, 
                           std::function<std::string(const std::string&)> handler);
    
    /**
     * 处理函数调用
     * @param parameters 参数映射
     * @return 生成的代码
     */
    std::string processCall(const std::map<std::string, std::string>& parameters) const;
    
    /**
     * 验证参数完整性
     * @param parameters 参数映射
     * @return 是否有效
     */
    bool validateParameters(const std::map<std::string, std::string>& parameters) const;
    
    /**
     * 获取必需的参数列表
     * @return 必需参数列表
     */
    std::vector<std::string> getRequiredParameters() const;
    
    /**
     * 获取可选的参数列表
     * @return 可选参数列表
     */
    std::vector<std::string> getOptionalParameters() const;
    
    /**
     * 生成函数定义代码
     * @return 函数定义代码
     */
    std::string generateFunctionDefinition() const;
    
    /**
     * 生成虚对象支持代码
     * @return 虚对象支持代码
     */
    std::string generateVirtualObjectSupport() const;

private:
    std::string m_name;
    std::string m_syntax;
    bool m_supportsVirtualObject;
    bool m_supportsUnorderedKeyValue;
    bool m_supportsOptionalKeyValue;
    bool m_supportsUnquotedLiterals;
    
    std::map<std::string, std::function<std::string(const std::string&)>> m_parameterBindings;
    std::vector<std::string> m_requiredParameters;
    std::vector<std::string> m_optionalParameters;
    
    /**
     * 解析语法字符串
     */
    void parseSyntax();
    
    /**
     * 提取参数信息
     */
    void extractParameters();
    
    /**
     * 检查语法特征
     */
    void analyzeSyntaxFeatures();
    
    /**
     * 生成参数处理代码
     * @param parameters 参数映射
     * @return 参数处理代码
     */
    std::string generateParameterProcessing(const std::map<std::string, std::string>& parameters) const;
    
    /**
     * 生成默认参数值
     * @param parameter 参数名
     * @return 默认值
     */
    std::string generateDefaultValue(const std::string& parameter) const;
    
    /**
     * 验证参数类型
     * @param parameter 参数名
     * @param value 参数值
     * @return 是否有效
     */
    bool validateParameterType(const std::string& parameter, const std::string& value) const;
};

} // namespace CJMOD

#endif // CJMOD_CHTLJSFUNCTION_H
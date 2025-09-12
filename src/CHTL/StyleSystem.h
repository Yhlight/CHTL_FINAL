#ifndef STYLE_SYSTEM_H
#define STYLE_SYSTEM_H

#include "CHTLParser.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

/**
 * 样式属性类型
 */
enum class StylePropertyType {
    INLINE,         // 内联样式
    CLASS,          // 类选择器
    ID,             // ID选择器
    PSEUDO_CLASS,   // 伪类选择器
    PSEUDO_ELEMENT  // 伪元素选择器
};

/**
 * 样式属性
 */
struct StyleProperty {
    std::string name;
    std::string value;
    StylePropertyType type;
    std::string selector;
    std::string unit;
    bool isComputed;
    
    StyleProperty(const std::string& n, const std::string& v, StylePropertyType t = StylePropertyType::INLINE)
        : name(n), value(v), type(t), isComputed(false) {}
};

/**
 * 属性运算器
 */
class PropertyCalculator {
public:
    /**
     * 计算属性值
     * @param expression 表达式
     * @param context 上下文
     * @return 计算结果
     */
    static std::string calculate(const std::string& expression, 
                                const std::map<std::string, std::string>& context = {});
    
    /**
     * 解析算术表达式
     * @param expression 表达式
     * @return 解析结果
     */
    static double parseArithmeticExpression(const std::string& expression);
    
    /**
     * 验证单位兼容性
     * @param unit1 单位1
     * @param unit2 单位2
     * @return 是否兼容
     */
    static bool validateUnitCompatibility(const std::string& unit1, const std::string& unit2);
    
    /**
     * 提取数值和单位
     * @param value 值
     * @return 数值和单位的对
     */
    static std::pair<double, std::string> extractValueAndUnit(const std::string& value);

private:
    static double evaluateExpression(const std::string& expression);
    static std::string normalizeUnit(const std::string& unit);
};

/**
 * 条件表达式解析器
 */
class ConditionalExpressionParser {
public:
    /**
     * 解析条件表达式
     * @param expression 表达式
     * @param context 上下文
     * @return 解析结果
     */
    static std::string parse(const std::string& expression, 
                           const std::map<std::string, std::string>& context = {});
    
    /**
     * 解析链式条件表达式
     * @param expressions 表达式列表
     * @param context 上下文
     * @return 解析结果
     */
    static std::string parseChain(const std::vector<std::string>& expressions, 
                                const std::map<std::string, std::string>& context = {});
    
    /**
     * 解析可选条件表达式
     * @param expressions 表达式列表
     * @param context 上下文
     * @return 解析结果
     */
    static std::string parseOptional(const std::vector<std::string>& expressions, 
                                   const std::map<std::string, std::string>& context = {});

private:
    static bool evaluateCondition(const std::string& condition, 
                                const std::map<std::string, std::string>& context);
    static std::string parseArithmeticInCondition(const std::string& condition);
    static std::string parseLogicalInCondition(const std::string& condition);
};

/**
 * 属性引用解析器
 */
class PropertyReferenceParser {
public:
    /**
     * 解析属性引用
     * @param reference 引用表达式
     * @param context 上下文
     * @return 解析结果
     */
    static std::string parse(const std::string& reference, 
                           const std::map<std::string, std::string>& context = {});
    
    /**
     * 解析选择器
     * @param selector 选择器
     * @return 解析结果
     */
    static std::string parseSelector(const std::string& selector);
    
    /**
     * 解析属性名
     * @param property 属性名
     * @return 解析结果
     */
    static std::string parseProperty(const std::string& property);

private:
    static std::string resolveSelector(const std::string& selector, 
                                     const std::map<std::string, std::string>& context);
    static std::string getPropertyValue(const std::string& selector, 
                                      const std::string& property,
                                      const std::map<std::string, std::string>& context);
};

/**
 * 响应式值处理器
 */
class ReactiveValueProcessor {
public:
    /**
     * 处理响应式值
     * @param value 值
     * @param context 上下文
     * @return 处理结果
     */
    static std::string process(const std::string& value, 
                             const std::map<std::string, std::string>& context = {});
    
    /**
     * 提取响应式变量
     * @param value 值
     * @return 变量名
     */
    static std::string extractVariableName(const std::string& value);
    
    /**
     * 生成响应式监听代码
     * @param variableName 变量名
     * @return 监听代码
     */
    static std::string generateReactiveListener(const std::string& variableName);

private:
    static bool isReactiveValue(const std::string& value);
    static std::string generateReactiveCode(const std::string& variableName);
};

/**
 * 局部样式块处理器
 */
class LocalStyleProcessor {
public:
    /**
     * 处理局部样式块
     * @param styleNode 样式节点
     * @param elementNode 元素节点
     * @return 处理结果
     */
    static std::string process(const StyleNode* styleNode, 
                             const ElementNode* elementNode);
    
    /**
     * 处理内联样式
     * @param properties 属性列表
     * @return 内联样式字符串
     */
    static std::string processInlineStyles(const std::vector<StyleProperty>& properties);
    
    /**
     * 处理类选择器
     * @param properties 属性列表
     * @param className 类名
     * @return 类选择器CSS
     */
    static std::string processClassSelector(const std::vector<StyleProperty>& properties, 
                                          const std::string& className);
    
    /**
     * 处理ID选择器
     * @param properties 属性列表
     * @param idName ID名
     * @return ID选择器CSS
     */
    static std::string processIdSelector(const std::vector<StyleProperty>& properties, 
                                       const std::string& idName);
    
    /**
     * 处理伪类选择器
     * @param properties 属性列表
     * @param pseudoClass 伪类
     * @param baseSelector 基础选择器
     * @return 伪类选择器CSS
     */
    static std::string processPseudoClassSelector(const std::vector<StyleProperty>& properties, 
                                                const std::string& pseudoClass,
                                                const std::string& baseSelector);
    
    /**
     * 处理伪元素选择器
     * @param properties 属性列表
     * @param pseudoElement 伪元素
     * @param baseSelector 基础选择器
     * @return 伪元素选择器CSS
     */
    static std::string processPseudoElementSelector(const std::vector<StyleProperty>& properties, 
                                                  const std::string& pseudoElement,
                                                  const std::string& baseSelector);
    
    /**
     * 处理上下文推导
     * @param properties 属性列表
     * @param context 上下文
     * @return 处理结果
     */
    static std::string processContextDerivation(const std::vector<StyleProperty>& properties, 
                                              const std::map<std::string, std::string>& context);

private:
    static std::string generateSelector(const std::string& type, const std::string& name);
    static std::string generatePseudoSelector(const std::string& base, const std::string& pseudo);
    static std::string resolveContextReference(const std::string& reference, 
                                             const std::map<std::string, std::string>& context);
};

/**
 * 全局样式块处理器
 */
class GlobalStyleProcessor {
public:
    /**
     * 处理全局样式块
     * @param styleNode 样式节点
     * @return 处理结果
     */
    static std::string process(const StyleNode* styleNode);
    
    /**
     * 处理模板变量
     * @param properties 属性列表
     * @param variables 变量映射
     * @return 处理结果
     */
    static std::string processTemplateVariables(const std::vector<StyleProperty>& properties, 
                                              const std::map<std::string, std::string>& variables);
    
    /**
     * 处理自定义变量
     * @param properties 属性列表
     * @param variables 变量映射
     * @return 处理结果
     */
    static std::string processCustomVariables(const std::vector<StyleProperty>& properties, 
                                            const std::map<std::string, std::string>& variables);

private:
    static std::string replaceVariables(const std::string& value, 
                                      const std::map<std::string, std::string>& variables);
};

/**
 * 样式系统管理器
 */
class StyleSystem {
public:
    StyleSystem();
    ~StyleSystem();
    
    /**
     * 处理样式节点
     * @param styleNode 样式节点
     * @param elementNode 元素节点（可选）
     * @return 处理结果
     */
    std::string processStyleNode(const StyleNode* styleNode, 
                               const ElementNode* elementNode = nullptr);
    
    /**
     * 处理属性运算
     * @param expression 表达式
     * @param context 上下文
     * @return 计算结果
     */
    std::string processPropertyCalculation(const std::string& expression, 
                                         const std::map<std::string, std::string>& context = {});
    
    /**
     * 处理条件表达式
     * @param expression 表达式
     * @param context 上下文
     * @return 处理结果
     */
    std::string processConditionalExpression(const std::string& expression, 
                                           const std::map<std::string, std::string>& context = {});
    
    /**
     * 处理属性引用
     * @param reference 引用表达式
     * @param context 上下文
     * @return 处理结果
     */
    std::string processPropertyReference(const std::string& reference, 
                                       const std::map<std::string, std::string>& context = {});
    
    /**
     * 处理响应式值
     * @param value 值
     * @param context 上下文
     * @return 处理结果
     */
    std::string processReactiveValue(const std::string& value, 
                                   const std::map<std::string, std::string>& context = {});
    
    /**
     * 添加全局样式
     * @param css CSS代码
     */
    void addGlobalStyle(const std::string& css);
    
    /**
     * 获取全局样式
     * @return 全局样式CSS
     */
    std::string getGlobalStyles() const;
    
    /**
     * 清空全局样式
     */
    void clearGlobalStyles();

private:
    std::string globalStyles;
    std::map<std::string, std::string> elementProperties;
    std::map<std::string, std::string> reactiveVariables;
    
    /**
     * 解析样式属性
     * @param styleNode 样式节点
     * @return 属性列表
     */
    std::vector<StyleProperty> parseStyleProperties(const StyleNode* styleNode);
    
    /**
     * 处理属性值
     * @param value 属性值
     * @param context 上下文
     * @return 处理后的值
     */
    std::string processPropertyValue(const std::string& value, 
                                   const std::map<std::string, std::string>& context = {});
    
    /**
     * 验证样式属性
     * @param property 属性
     * @return 是否有效
     */
    bool validateStyleProperty(const StyleProperty& property);
    
    /**
     * 生成CSS代码
     * @param properties 属性列表
     * @param selector 选择器
     * @return CSS代码
     */
    std::string generateCSS(const std::vector<StyleProperty>& properties, 
                          const std::string& selector = "");
};

} // namespace CHTL

#endif // STYLE_SYSTEM_H
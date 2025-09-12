#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "CHTLJSParser.h"
#include <string>
#include <map>
#include <vector>

namespace CHTL {

/**
 * CHTL JS代码生成器
 */
class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    ~CHTLJSGenerator();
    
    /**
     * 生成JavaScript代码
     * @param ast AST根节点
     * @return JavaScript代码
     */
    std::string generateJavaScript(const CHTLJSNode* ast);
    
    /**
     * 生成虚对象代码
     * @param node 虚对象节点
     * @return JavaScript代码
     */
    std::string generateVirtualObject(const VirtualObjectNode* node);
    
    /**
     * 生成监听器代码
     * @param node 监听器节点
     * @return JavaScript代码
     */
    std::string generateListen(const ListenNode* node);
    
    /**
     * 生成动画代码
     * @param node 动画节点
     * @return JavaScript代码
     */
    std::string generateAnimate(const AnimateNode* node);
    
    /**
     * 生成文件加载器代码
     * @param node 文件加载器节点
     * @return JavaScript代码
     */
    std::string generateFileLoader(const FileLoaderNode* node);
    
    /**
     * 生成路由代码
     * @param node 路由节点
     * @return JavaScript代码
     */
    std::string generateRouter(const RouterNode* node);
    
    /**
     * 生成iNeverAway代码
     * @param node iNeverAway节点
     * @return JavaScript代码
     */
    std::string generateINeverAway(const INeverAwayNode* node);
    
    /**
     * 生成printMylove代码
     * @param node printMylove节点
     * @return JavaScript代码
     */
    std::string generatePrintMylove(const PrintMyloveNode* node);
    
    /**
     * 生成util...then表达式代码
     * @param node util...then节点
     * @return JavaScript代码
     */
    std::string generateUtilThen(const UtilThenNode* node);
    
    /**
     * 生成增强选择器代码
     * @param node 增强选择器节点
     * @return JavaScript代码
     */
    std::string generateEnhancedSelector(const EnhancedSelectorNode* node);
    
    /**
     * 生成响应式值代码
     * @param node 响应式值节点
     * @return JavaScript代码
     */
    std::string generateReactiveValue(const ReactiveValueNode* node);
    
    /**
     * 生成表达式代码
     * @param node 表达式节点
     * @return JavaScript代码
     */
    std::string generateExpression(const CHTLJSNode* node);
    
    /**
     * 生成对象字面量代码
     * @param node 对象字面量节点
     * @return JavaScript代码
     */
    std::string generateObjectLiteral(const CHTLJSNode* node);
    
    /**
     * 生成数组字面量代码
     * @param node 数组字面量节点
     * @return JavaScript代码
     */
    std::string generateArrayLiteral(const CHTLJSNode* node);
    
    /**
     * 生成函数调用代码
     * @param node 函数调用节点
     * @return JavaScript代码
     */
    std::string generateFunctionCall(const CHTLJSNode* node);
    
    /**
     * 生成完整JavaScript代码
     * @param ast AST根节点
     * @return 完整JavaScript代码
     */
    std::string generateCompleteJavaScript(const CHTLJSNode* ast);

private:
    // 全局变量和函数映射
    std::map<std::string, std::string> globalVariables;
    std::map<std::string, std::string> globalFunctions;
    
    // 虚对象映射
    std::map<std::string, std::map<std::string, std::string>> virtualObjects;
    
    // 事件委托注册表
    std::map<std::string, std::vector<std::string>> eventDelegates;
    
    // 路由注册表
    std::vector<std::pair<std::string, std::string>> routes;
    
    /**
     * 生成选择器查询代码
     * @param selector 选择器
     * @return JavaScript代码
     */
    std::string generateSelectorQuery(const std::string& selector);
    
    /**
     * 生成事件委托代码
     * @param parentSelector 父选择器
     * @param childSelectors 子选择器列表
     * @param events 事件映射
     * @return JavaScript代码
     */
    std::string generateEventDelegate(const std::string& parentSelector, 
                                    const std::vector<std::string>& childSelectors,
                                    const std::map<std::string, std::string>& events);
    
    /**
     * 生成动画关键帧代码
     * @param whenStates 关键帧状态
     * @return JavaScript代码
     */
    std::string generateAnimationKeyframes(const std::vector<std::map<std::string, std::string>>& whenStates);
    
    /**
     * 生成路由处理代码
     * @return JavaScript代码
     */
    std::string generateRouteHandler();
    
    /**
     * 生成模块加载代码
     * @param files 文件列表
     * @return JavaScript代码
     */
    std::string generateModuleLoader(const std::vector<std::string>& files);
    
    /**
     * 生成响应式值监听代码
     * @param variableName 变量名
     * @return JavaScript代码
     */
    std::string generateReactiveListener(const std::string& variableName);
    
    /**
     * 生成条件表达式代码
     * @param condition 条件
     * @param trueValue 真值
     * @param falseValue 假值
     * @return JavaScript代码
     */
    std::string generateConditionalExpression(const std::string& condition, 
                                            const std::string& trueValue, 
                                            const std::string& falseValue);
    
    /**
     * 生成算术表达式代码
     * @param left 左操作数
     * @param operator 操作符
     * @param right 右操作数
     * @return JavaScript代码
     */
    std::string generateArithmeticExpression(const std::string& left, 
                                           const std::string& op, 
                                           const std::string& right);
    
    /**
     * 生成逻辑表达式代码
     * @param left 左操作数
     * @param operator 操作符
     * @param right 右操作数
     * @return JavaScript代码
     */
    std::string generateLogicalExpression(const std::string& left, 
                                        const std::string& op, 
                                        const std::string& right);
    
    /**
     * 生成比较表达式代码
     * @param left 左操作数
     * @param operator 操作符
     * @param right 右操作数
     * @return JavaScript代码
     */
    std::string generateComparisonExpression(const std::string& left, 
                                           const std::string& op, 
                                           const std::string& right);
    
    /**
     * 生成属性访问代码
     * @param object 对象
     * @param property 属性
     * @return JavaScript代码
     */
    std::string generatePropertyAccess(const std::string& object, const std::string& property);
    
    /**
     * 生成函数定义代码
     * @param name 函数名
     * @param parameters 参数列表
     * @param body 函数体
     * @return JavaScript代码
     */
    std::string generateFunctionDefinition(const std::string& name, 
                                         const std::vector<std::string>& parameters, 
                                         const std::string& body);
    
    /**
     * 生成变量声明代码
     * @param name 变量名
     * @param value 变量值
     * @param isConst 是否为常量
     * @return JavaScript代码
     */
    std::string generateVariableDeclaration(const std::string& name, 
                                          const std::string& value, 
                                          bool isConst = false);
    
    /**
     * 生成注释代码
     * @param comment 注释内容
     * @return JavaScript代码
     */
    std::string generateComment(const std::string& comment);
    
    /**
     * 生成缩进
     * @param level 缩进级别
     * @return 缩进字符串
     */
    std::string generateIndent(int level);
    
    /**
     * 转义JavaScript字符串
     * @param str 原始字符串
     * @return 转义后的字符串
     */
    std::string escapeJavaScriptString(const std::string& str);
    
    /**
     * 验证JavaScript标识符
     * @param identifier 标识符
     * @return 是否有效
     */
    bool isValidJavaScriptIdentifier(const std::string& identifier);
    
    /**
     * 生成唯一标识符
     * @param prefix 前缀
     * @return 唯一标识符
     */
    std::string generateUniqueIdentifier(const std::string& prefix = "chtl_");
};

} // namespace CHTL

#endif // CHTL_JS_GENERATOR_H
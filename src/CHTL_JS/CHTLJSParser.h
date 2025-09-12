#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "CHTLJSLexer.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace CHTL {

/**
 * CHTL JS AST节点基类
 */
class CHTLJSNode {
public:
    enum class NodeType {
        ROOT,
        VIRTUAL_OBJECT,     // 虚对象
        LISTEN,             // 监听器
        ANIMATE,            // 动画
        FILELOADER,         // 文件加载器
        ROUTER,             // 路由
        INEVERAWAY,         // iNeverAway
        PRINTMYLOVE,        // printMylove
        UTIL_THEN,          // util...then表达式
        ENHANCED_SELECTOR,  // 增强选择器
        REACTIVE_VALUE,     // 响应式值
        EXPRESSION,         // 表达式
        FUNCTION_CALL,      // 函数调用
        OBJECT_LITERAL,     // 对象字面量
        ARRAY_LITERAL,      // 数组字面量
        PROPERTY,           // 属性
        COMMENT             // 注释
    };
    
    NodeType type;
    std::string value;
    size_t line;
    size_t column;
    std::vector<std::unique_ptr<CHTLJSNode>> children;
    std::map<std::string, std::string> properties;
    
    CHTLJSNode(NodeType t, const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
    
    virtual ~CHTLJSNode() = default;
    
    void addChild(std::unique_ptr<CHTLJSNode> child);
    std::string toString(int indent = 0) const;
};

/**
 * 虚对象节点
 */
class VirtualObjectNode : public CHTLJSNode {
public:
    std::string name;
    std::map<std::string, std::unique_ptr<CHTLJSNode>> properties;
    
    VirtualObjectNode(const std::string& n, size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::VIRTUAL_OBJECT, n, l, c), name(n) {}
};

/**
 * 监听器节点
 */
class ListenNode : public CHTLJSNode {
public:
    std::string selector;
    std::map<std::string, std::string> events;
    
    ListenNode(const std::string& sel, size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::LISTEN, sel, l, c), selector(sel) {}
};

/**
 * 动画节点
 */
class AnimateNode : public CHTLJSNode {
public:
    std::string target;
    std::string duration;
    std::string easing;
    std::map<std::string, std::string> beginState;
    std::vector<std::map<std::string, std::string>> whenStates;
    std::map<std::string, std::string> endState;
    std::string loop;
    std::string direction;
    std::string delay;
    std::string callback;
    
    AnimateNode(size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::ANIMATE, "", l, c) {}
};

/**
 * 文件加载器节点
 */
class FileLoaderNode : public CHTLJSNode {
public:
    std::vector<std::string> files;
    
    FileLoaderNode(size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::FILELOADER, "", l, c) {}
};

/**
 * 路由节点
 */
class RouterNode : public CHTLJSNode {
public:
    std::vector<std::string> urls;
    std::vector<std::string> pages;
    std::string root;
    std::string mode;
    
    RouterNode(size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::ROUTER, "", l, c) {}
};

/**
 * iNeverAway节点
 */
class INeverAwayNode : public CHTLJSNode {
public:
    std::map<std::string, std::string> functions;
    
    INeverAwayNode(size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::INEVERAWAY, "", l, c) {}
};

/**
 * printMylove节点
 */
class PrintMyloveNode : public CHTLJSNode {
public:
    std::string url;
    std::string mode;
    std::string width;
    std::string height;
    std::string scale;
    
    PrintMyloveNode(size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::PRINTMYLOVE, "", l, c) {}
};

/**
 * util...then表达式节点
 */
class UtilThenNode : public CHTLJSNode {
public:
    std::string condition;
    std::string changeAction;
    std::string thenAction;
    
    UtilThenNode(const std::string& cond, size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::UTIL_THEN, cond, l, c), condition(cond) {}
};

/**
 * 增强选择器节点
 */
class EnhancedSelectorNode : public CHTLJSNode {
public:
    std::string selector;
    
    EnhancedSelectorNode(const std::string& sel, size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::ENHANCED_SELECTOR, sel, l, c), selector(sel) {}
};

/**
 * 响应式值节点
 */
class ReactiveValueNode : public CHTLJSNode {
public:
    std::string variableName;
    
    ReactiveValueNode(const std::string& var, size_t l = 0, size_t c = 0)
        : CHTLJSNode(NodeType::REACTIVE_VALUE, var, l, c), variableName(var) {}
};

/**
 * CHTL JS语法分析器
 */
class CHTLJSParser {
public:
    CHTLJSParser();
    ~CHTLJSParser();
    
    /**
     * 设置词法分析器
     * @param lexer 词法分析器
     */
    void setLexer(std::unique_ptr<CHTLJSLexer> lexer);
    
    /**
     * 解析源代码
     * @return AST根节点
     */
    std::unique_ptr<CHTLJSNode> parse();
    
    /**
     * 解析虚对象
     * @return 虚对象节点
     */
    std::unique_ptr<VirtualObjectNode> parseVirtualObject();
    
    /**
     * 解析监听器
     * @return 监听器节点
     */
    std::unique_ptr<ListenNode> parseListen();
    
    /**
     * 解析动画
     * @return 动画节点
     */
    std::unique_ptr<AnimateNode> parseAnimate();
    
    /**
     * 解析文件加载器
     * @return 文件加载器节点
     */
    std::unique_ptr<FileLoaderNode> parseFileLoader();
    
    /**
     * 解析路由
     * @return 路由节点
     */
    std::unique_ptr<RouterNode> parseRouter();
    
    /**
     * 解析iNeverAway
     * @return iNeverAway节点
     */
    std::unique_ptr<INeverAwayNode> parseINeverAway();
    
    /**
     * 解析printMylove
     * @return printMylove节点
     */
    std::unique_ptr<PrintMyloveNode> parsePrintMylove();
    
    /**
     * 解析util...then表达式
     * @return util...then节点
     */
    std::unique_ptr<UtilThenNode> parseUtilThen();
    
    /**
     * 解析增强选择器
     * @return 增强选择器节点
     */
    std::unique_ptr<EnhancedSelectorNode> parseEnhancedSelector();
    
    /**
     * 解析响应式值
     * @return 响应式值节点
     */
    std::unique_ptr<ReactiveValueNode> parseReactiveValue();
    
    /**
     * 解析表达式
     * @return 表达式节点
     */
    std::unique_ptr<CHTLJSNode> parseExpression();
    
    /**
     * 解析对象字面量
     * @return 对象字面量节点
     */
    std::unique_ptr<CHTLJSNode> parseObjectLiteral();
    
    /**
     * 解析数组字面量
     * @return 数组字面量节点
     */
    std::unique_ptr<CHTLJSNode> parseArrayLiteral();
    
    /**
     * 解析函数调用
     * @return 函数调用节点
     */
    std::unique_ptr<CHTLJSNode> parseFunctionCall();
    
    /**
     * 获取当前Token
     * @return 当前Token
     */
    const CHTLJSToken* currentToken() const;
    
    /**
     * 获取下一个Token
     * @return 下一个Token
     */
    const CHTLJSToken* nextToken();
    
    /**
     * 检查当前Token类型
     * @param type Token类型
     * @return 是否匹配
     */
    bool match(CHTLJSTokenType type);
    
    /**
     * 消费当前Token
     */
    void consume();
    
    /**
     * 检查是否到达文件末尾
     * @return 是否到达文件末尾
     */
    bool isEOF() const;
    
    /**
     * 获取错误信息
     * @return 错误信息
     */
    const std::string& getError() const;

private:
    std::unique_ptr<CHTLJSLexer> lexer;
    std::vector<CHTLJSToken> tokens;
    size_t currentTokenIndex;
    std::string errorMessage;
    
    /**
     * 报告错误
     * @param message 错误信息
     */
    void reportError(const std::string& message);
    
    /**
     * 解析属性列表
     * @return 属性映射
     */
    std::map<std::string, std::string> parseProperties();
    
    /**
     * 解析值
     * @return 值字符串
     */
    std::string parseValue();
    
    /**
     * 解析标识符
     * @return 标识符字符串
     */
    std::string parseIdentifier();
    
    /**
     * 解析字符串
     * @return 字符串内容
     */
    std::string parseString();
    
    /**
     * 解析数字
     * @return 数字字符串
     */
    std::string parseNumber();
    
    /**
     * 解析字面量
     * @return 字面量字符串
     */
    std::string parseLiteral();
    
    /**
     * 解析条件表达式
     * @return 条件表达式字符串
     */
    std::string parseConditionalExpression();
    
    /**
     * 解析算术表达式
     * @return 算术表达式字符串
     */
    std::string parseArithmeticExpression();
    
    /**
     * 解析逻辑表达式
     * @return 逻辑表达式字符串
     */
    std::string parseLogicalExpression();
    
    /**
     * 解析比较表达式
     * @return 比较表达式字符串
     */
    std::string parseComparisonExpression();
    
    /**
     * 解析主表达式
     * @return 主表达式节点
     */
    std::unique_ptr<CHTLJSNode> parsePrimaryExpression();
};

} // namespace CHTL

#endif // CHTL_JS_PARSER_H
#ifndef CHTLJS_TOKEN_H
#define CHTLJS_TOKEN_H

#include <string>

namespace CHTLJS {

/**
 * CHTL JS令牌类型
 * CHTL JS是独立的编程语言，不是JavaScript
 */
enum class CHTLJS_TokenType {
    // 基本类型
    IDENTIFIER,         // 标识符
    STRING_LITERAL,     // 字符串字面量
    NUMBER_LITERAL,     // 数字字面量
    BOOLEAN_LITERAL,    // 布尔字面量
    
    // CHTL JS关键字
    SCRIPT_LOADER,      // ScriptLoader - 模块加载器
    LISTEN,             // Listen - 事件监听
    ANIMATE,            // Animate - 动画系统
    ROUTER,             // Router - 路由系统
    VIR,                // Vir - 虚拟对象
    UTIL,               // util - 工具函数
    CHANGE,             // change - 变更操作
    THEN,               // then - 链式操作
    PRINTMYLOVE,        // printMylove - 图像转ASCII
    INEVERAWAY,         // iNeverAway - 函数重载
    
    // CHTL JS操作符
    BIND_OPERATOR,      // &-> - 绑定操作符
    ARROW_OPERATOR,     // -> - 箭头操作符
    RESPONSIVE_GET,     // $变量名$ - 响应式获取
    RESPONSIVE_SET,     // $变量名$ = - 响应式设置
    
    // CHTL JS特殊语法
    CHTL_SELECTOR,      // {{选择器}} - CHTL选择器
    CHTL_RESPONSIVE,    // $JS变量名$ - CHTL响应式值
    CHTL_FUNCTION,      // CHTL JS函数调用
    
    // 传统操作符
    ASSIGN,             // = - 赋值
    EQUAL,              // == - 相等
    NOT_EQUAL,          // != - 不等
    LESS,               // < - 小于
    GREATER,            // > - 大于
    LESS_EQUAL,         // <= - 小于等于
    GREATER_EQUAL,      // >= - 大于等于
    AND,                // && - 逻辑与
    OR,                 // || - 逻辑或
    NOT,                // ! - 逻辑非
    PLUS,               // + - 加法
    MINUS,              // - - 减法
    MULTIPLY,           // * - 乘法
    DIVIDE,             // / - 除法
    MODULO,             // % - 取模
    POWER,              // ** - 幂运算
    
    // 分隔符
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    COLON,              // :
    QUESTION,           // ?
    
    // 特殊
    NEWLINE,            // 换行
    EOF_TOKEN,          // 文件结束
    UNKNOWN             // 未知
};

/**
 * CHTL JS令牌
 */
struct CHTLJS_Token {
    CHTLJS_TokenType type;
    std::string value;
    int line;
    int column;

    CHTLJS_Token(CHTLJS_TokenType type, const std::string& value, int line, int column)
        : type(type), value(value), line(line), column(column) {}

    std::string toString() const;
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isCHTLJSSpecific() const;
};

} // namespace CHTLJS

#endif // CHTLJS_TOKEN_H
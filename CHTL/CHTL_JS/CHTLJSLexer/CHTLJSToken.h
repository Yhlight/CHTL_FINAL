#ifndef CHTL_JS_TOKEN_H
#define CHTL_JS_TOKEN_H

#include <string>
#include <map>
#include <vector>

namespace CHTL_JS {

enum class CHTLJSTokenType {
    // 基础类型
    IDENTIFIER,         // 标识符
    LITERAL,            // 字面量（无修饰字面量）
    STRING,             // 字符串
    NUMBER,             // 数字
    BOOLEAN,            // 布尔值
    
    // 运算符
    ASSIGN,             // = 赋值
    ARROW,              // -> 箭头运算符
    DOT,                // . 点运算符
    COLON,              // : 冒号
    SEMICOLON,          // ; 分号
    COMMA,              // , 逗号
    
    // 算术运算符
    PLUS,               // + 加
    MINUS,              // - 减
    MULTIPLY,           // * 乘
    DIVIDE,             // / 除
    MODULO,             // % 取模
    POWER,              // ** 幂
    
    // 比较运算符
    EQUAL,              // == 等于
    NOT_EQUAL,          // != 不等于
    LESS_THAN,          // < 小于
    GREATER_THAN,       // > 大于
    LESS_EQUAL,         // <= 小于等于
    GREATER_EQUAL,      // >= 大于等于
    
    // 逻辑运算符
    AND,                // && 逻辑与
    OR,                 // || 逻辑或
    NOT,                // ! 逻辑非
    
    // 位运算符
    BITWISE_AND,        // & 按位与
    BITWISE_OR,         // | 按位或
    BITWISE_XOR,        // ^ 按位异或
    BITWISE_NOT,        // ~ 按位非
    LEFT_SHIFT,         // << 左移
    RIGHT_SHIFT,        // >> 右移
    
    // 括号和分隔符
    LEFT_PAREN,         // ( 左括号
    RIGHT_PAREN,        // ) 右括号
    LEFT_BRACKET,       // [ 左方括号
    RIGHT_BRACKET,      // ] 右方括号
    LEFT_BRACE,         // { 左花括号
    RIGHT_BRACE,        // } 右花括号
    
    // 特殊符号
    QUESTION,           // ? 问号
    TERNARY,            // ? : 三元运算符
    NULLISH,            // ?? 空值合并
    OPTIONAL,           // ?. 可选链
    
    // CHTL JS特有语法
    ENHANCED_SELECTOR,  // {{选择器}} 增强选择器
    VIRTUAL_OBJECT,     // vir 虚对象
    LISTEN,             // listen 增强监听器
    DELEGATE,           // delegate 事件委托
    ANIMATE,            // animate 动画
    ROUTER,             // router 路由
    FILELOADER,         // fileloader 文件加载器
    
    // 关键字
    IF,                 // if 条件
    ELSE,               // else 否则
    FOR,                // for 循环
    WHILE,              // while 循环
    DO,                 // do 循环
    SWITCH,             // switch 开关
    CASE,               // case 情况
    DEFAULT,            // default 默认
    BREAK,              // break 跳出
    CONTINUE,           // continue 继续
    RETURN,             // return 返回
    FUNCTION,           // function 函数
    CONST,              // const 常量
    LET,                // let 变量
    VAR,                // var 变量
    TRUE,               // true 真
    FALSE,              // false 假
    NULL,               // null 空
    UNDEFINED,          // undefined 未定义
    THIS,               // this 当前对象
    NEW,                // new 新建
    TYPEOF,             // typeof 类型
    INSTANCEOF,         // instanceof 实例
    IN,                 // in 包含
    OF,                 // of 属于
    WITH,               // with 使用
    TRY,                // try 尝试
    CATCH,              // catch 捕获
    FINALLY,            // finally 最终
    THROW,              // throw 抛出
    
    // 注释
    SINGLE_COMMENT,     // // 单行注释
    MULTI_COMMENT,      // /* */ 多行注释
    GENERATOR_COMMENT,  // -- 生成器注释
    
    // 文件结束
    END_OF_FILE,        // 文件结束
    
    // 错误
    ERROR,              // 错误
    UNKNOWN             // 未知
};

class CHTLJSToken {
private:
    CHTLJSTokenType type;
    std::string value;
    int line;
    int column;
    int position;
    std::map<std::string, std::string> attributes;

public:
    CHTLJSToken();
    CHTLJSToken(CHTLJSTokenType type, const std::string& value, int line = 0, int column = 0, int position = 0);
    virtual ~CHTLJSToken() = default;
    
    // 基本属性
    CHTLJSTokenType getType() const { return type; }
    void setType(CHTLJSTokenType type) { this->type = type; }
    
    const std::string& getValue() const { return value; }
    void setValue(const std::string& value) { this->value = value; }
    
    int getLine() const { return line; }
    void setLine(int line) { this->line = line; }
    
    int getColumn() const { return column; }
    void setColumn(int column) { this->column = column; }
    
    int getPosition() const { return position; }
    void setPosition(int position) { this->position = position; }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const std::map<std::string, std::string>& getAttributes() const { return attributes; }
    
    // 类型判断
    bool isIdentifier() const { return type == CHTLJSTokenType::IDENTIFIER; }
    bool isLiteral() const { return type == CHTLJSTokenType::LITERAL; }
    bool isString() const { return type == CHTLJSTokenType::STRING; }
    bool isNumber() const { return type == CHTLJSTokenType::NUMBER; }
    bool isBoolean() const { return type == CHTLJSTokenType::BOOLEAN; }
    bool isOperator() const;
    bool isKeyword() const;
    bool isPunctuation() const;
    bool isComment() const;
    bool isEndOfFile() const { return type == CHTLJSTokenType::END_OF_FILE; }
    bool isError() const { return type == CHTLJSTokenType::ERROR; }
    
    // CHTL JS特有类型判断
    bool isEnhancedSelector() const { return type == CHTLJSTokenType::ENHANCED_SELECTOR; }
    bool isVirtualObject() const { return type == CHTLJSTokenType::VIRTUAL_OBJECT; }
    bool isListen() const { return type == CHTLJSTokenType::LISTEN; }
    bool isDelegate() const { return type == CHTLJSTokenType::DELEGATE; }
    bool isAnimate() const { return type == CHTLJSTokenType::ANIMATE; }
    bool isRouter() const { return type == CHTLJSTokenType::ROUTER; }
    bool isFileloader() const { return type == CHTLJSTokenType::FILELOADER; }
    
    // 字符串表示
    std::string getTypeName() const;
    std::string toString() const;
    std::string toDebugString() const;
    
    // 比较操作
    bool operator==(const CHTLJSToken& other) const;
    bool operator!=(const CHTLJSToken& other) const;
    bool operator==(CHTLJSTokenType type) const;
    bool operator!=(CHTLJSTokenType type) const;
    
    // 克隆
    CHTLJSToken clone() const;
    
    // 验证
    bool isValid() const;
    bool isComplete() const;
    
    // 转换
    std::string toJavaScript() const;
    std::string toCHTLJS() const;
    std::string toHTML() const;
    std::string toCSS() const;
    
    // 格式化
    std::string format() const;
    std::string minify() const;
    std::string beautify() const;
    
    // 压缩
    std::string compress() const;
    std::string decompress() const;
    
    // 编码
    std::string encode() const;
    std::string decode() const;

private:
    // 辅助方法
    bool isArithmeticOperator() const;
    bool isComparisonOperator() const;
    bool isLogicalOperator() const;
    bool isBitwiseOperator() const;
    bool isAssignmentOperator() const;
    bool isUnaryOperator() const;
    bool isBinaryOperator() const;
    bool isTernaryOperator() const;
    
    // 类型转换
    std::string convertToJavaScript() const;
    std::string convertToCHTLJS() const;
    std::string convertToHTML() const;
    std::string convertToCSS() const;
    
    // 格式化辅助
    std::string formatToken() const;
    std::string minifyToken() const;
    std::string beautifyToken() const;
    
    // 压缩辅助
    std::string compressToken() const;
    std::string decompressToken() const;
    
    // 编码辅助
    std::string encodeToken() const;
    std::string decodeToken() const;
};

} // namespace CHTL_JS

#endif // CHTL_JS_TOKEN_H
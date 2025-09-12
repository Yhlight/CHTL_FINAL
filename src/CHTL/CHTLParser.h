#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTLLexer.h"
#include <memory>
#include <vector>
#include <string>

namespace CHTL {

/**
 * AST节点基类
 */
class ASTNode {
public:
    enum class NodeType {
        ROOT,
        ELEMENT,
        TEXT,
        ATTRIBUTE,
        STYLE,
        SCRIPT,
        TEMPLATE,
        CUSTOM,
        IMPORT,
        NAMESPACE,
        ORIGIN,
        CONFIGURATION,
        COMMENT
    };
    
    NodeType type;
    std::string value;
    size_t line;
    size_t column;
    std::vector<std::unique_ptr<ASTNode>> children;
    std::map<std::string, std::string> attributes;
    
    ASTNode(NodeType t, const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
    
    virtual ~ASTNode() = default;
    
    void addChild(std::unique_ptr<ASTNode> child);
    std::string toString(int indent = 0) const;
};

/**
 * 元素节点
 */
class ElementNode : public ASTNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<ASTNode>> children;
    
    ElementNode(const std::string& tag, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::ELEMENT, tag, l, c), tagName(tag) {}
};

/**
 * 文本节点
 */
class TextNode : public ASTNode {
public:
    std::string content;
    
    TextNode(const std::string& text, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::TEXT, text, l, c), content(text) {}
};

/**
 * 样式节点
 */
class StyleNode : public ASTNode {
public:
    std::map<std::string, std::string> properties;
    bool isLocal;
    
    StyleNode(bool local = false, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::STYLE, "", l, c), isLocal(local) {}
};

/**
 * 脚本节点
 */
class ScriptNode : public ASTNode {
public:
    std::string content;
    bool isLocal;
    
    ScriptNode(const std::string& script = "", bool local = false, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::SCRIPT, script, l, c), content(script), isLocal(local) {}
};

/**
 * 模板节点
 */
class TemplateNode : public ASTNode {
public:
    enum class TemplateType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    TemplateType templateType;
    std::string name;
    std::map<std::string, std::string> properties;
    std::vector<std::unique_ptr<ASTNode>> children;
    
    TemplateNode(TemplateType type, const std::string& n, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::TEMPLATE, n, l, c), templateType(type), name(n) {}
};

/**
 * 自定义节点
 */
class CustomNode : public ASTNode {
public:
    enum class CustomType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    CustomType customType;
    std::string name;
    std::map<std::string, std::string> properties;
    std::vector<std::unique_ptr<ASTNode>> children;
    
    CustomNode(CustomType type, const std::string& n, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::CUSTOM, n, l, c), customType(type), name(n) {}
};

/**
 * 导入节点
 */
class ImportNode : public ASTNode {
public:
    enum class ImportType {
        HTML,
        STYLE,
        JAVASCRIPT,
        CHTL,
        CJMOD
    };
    
    ImportType importType;
    std::string path;
    std::string alias;
    
    ImportNode(ImportType type, const std::string& p, const std::string& a = "", size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::IMPORT, p, l, c), importType(type), path(p), alias(a) {}
};

/**
 * 命名空间节点
 */
class NamespaceNode : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> children;
    
    NamespaceNode(const std::string& n, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::NAMESPACE, n, l, c), name(n) {}
};

/**
 * 原始嵌入节点
 */
class OriginNode : public ASTNode {
public:
    enum class OriginType {
        HTML,
        STYLE,
        JAVASCRIPT
    };
    
    OriginType originType;
    std::string name;
    std::string content;
    
    OriginNode(OriginType type, const std::string& n, const std::string& c = "", size_t l = 0, size_t c_pos = 0)
        : ASTNode(NodeType::ORIGIN, n, l, c_pos), originType(type), name(n), content(c) {}
};

/**
 * 配置节点
 */
class ConfigurationNode : public ASTNode {
public:
    std::string name;
    std::map<std::string, std::string> settings;
    std::map<std::string, std::vector<std::string>> nameGroup;
    
    ConfigurationNode(const std::string& n = "", size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::CONFIGURATION, n, l, c), name(n) {}
};

/**
 * CHTL语法分析器
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();
    
    /**
     * 设置词法分析器
     * @param lexer 词法分析器
     */
    void setLexer(std::unique_ptr<CHTLLexer> lexer);
    
    /**
     * 解析源代码
     * @return AST根节点
     */
    std::unique_ptr<ASTNode> parse();
    
    /**
     * 解析元素
     * @return 元素节点
     */
    std::unique_ptr<ElementNode> parseElement();
    
    /**
     * 解析文本节点
     * @return 文本节点
     */
    std::unique_ptr<TextNode> parseText();
    
    /**
     * 解析样式块
     * @return 样式节点
     */
    std::unique_ptr<StyleNode> parseStyle();
    
    /**
     * 解析脚本块
     * @return 脚本节点
     */
    std::unique_ptr<ScriptNode> parseScript();
    
    /**
     * 解析模板
     * @return 模板节点
     */
    std::unique_ptr<TemplateNode> parseTemplate();
    
    /**
     * 解析自定义
     * @return 自定义节点
     */
    std::unique_ptr<CustomNode> parseCustom();
    
    /**
     * 解析导入
     * @return 导入节点
     */
    std::unique_ptr<ImportNode> parseImport();
    
    /**
     * 解析命名空间
     * @return 命名空间节点
     */
    std::unique_ptr<NamespaceNode> parseNamespace();
    
    /**
     * 解析原始嵌入
     * @return 原始嵌入节点
     */
    std::unique_ptr<OriginNode> parseOrigin();
    
    /**
     * 解析配置
     * @return 配置节点
     */
    std::unique_ptr<ConfigurationNode> parseConfiguration();
    
    /**
     * 解析属性
     * @return 属性映射
     */
    std::map<std::string, std::string> parseAttributes();
    
    /**
     * 解析样式属性
     * @return 样式属性映射
     */
    std::map<std::string, std::string> parseStyleProperties();
    
    /**
     * 解析条件表达式
     * @return 条件表达式字符串
     */
    std::string parseConditionalExpression();
    
    /**
     * 解析属性引用
     * @return 属性引用字符串
     */
    std::string parsePropertyReference();
    
    /**
     * 解析算术表达式
     * @return 算术表达式字符串
     */
    std::string parseArithmeticExpression();
    
    /**
     * 获取当前Token
     * @return 当前Token
     */
    const Token* currentToken() const;
    
    /**
     * 获取下一个Token
     * @return 下一个Token
     */
    const Token* nextToken();
    
    /**
     * 检查当前Token类型
     * @param type Token类型
     * @return 是否匹配
     */
    bool match(TokenType type);
    
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
    std::unique_ptr<CHTLLexer> lexer;
    std::vector<Token> tokens;
    size_t currentTokenIndex;
    std::string errorMessage;
    
    /**
     * 报告错误
     * @param message 错误信息
     */
    void reportError(const std::string& message);
    
    /**
     * 跳过到匹配的右括号
     */
    void skipToMatchingBrace();
    
    /**
     * 解析表达式
     * @return 表达式字符串
     */
    std::string parseExpression();
    
    /**
     * 解析选择器
     * @return 选择器字符串
     */
    std::string parseSelector();
    
    /**
     * 解析值
     * @return 值字符串
     */
    std::string parseValue();
    
    /**
     * 解析字面量
     * @return 字面量字符串
     */
    std::string parseLiteral();
    
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
     * 解析标识符
     * @return 标识符字符串
     */
    std::string parseIdentifier();
    
    /**
     * 解析路径
     * @return 路径字符串
     */
    std::string parsePath();
    
    /**
     * 解析命名空间引用
     * @return 命名空间引用字符串
     */
    std::string parseNamespaceReference();
    
    /**
     * 解析模板引用
     * @return 模板引用字符串
     */
    std::string parseTemplateReference();
    
    /**
     * 解析自定义引用
     * @return 自定义引用字符串
     */
    std::string parseCustomReference();
    
    /**
     * 解析变量引用
     * @return 变量引用字符串
     */
    std::string parseVariableReference();
    
    /**
     * 解析响应式值
     * @return 响应式值字符串
     */
    std::string parseReactiveValue();
    
    /**
     * 解析CHTL JS语法
     * @return CHTL JS代码
     */
    std::string parseCHTLJSSyntax();
    
    /**
     * 解析增强选择器
     * @return 增强选择器字符串
     */
    std::string parseEnhancedSelector();
    
    /**
     * 解析虚对象
     * @return 虚对象字符串
     */
    std::string parseVirtualObject();
    
    /**
     * 解析路由
     * @return 路由字符串
     */
    std::string parseRouter();
    
    /**
     * 解析动画
     * @return 动画字符串
     */
    std::string parseAnimation();
    
    /**
     * 解析监听器
     * @return 监听器字符串
     */
    std::string parseListener();
    
    /**
     * 解析文件加载器
     * @return 文件加载器字符串
     */
    std::string parseFileLoader();
    
    /**
     * 解析iNeverAway
     * @return iNeverAway字符串
     */
    std::string parseINeverAway();
    
    /**
     * 解析printMylove
     * @return printMylove字符串
     */
    std::string parsePrintMylove();
    
    /**
     * 解析util...then表达式
     * @return util...then表达式字符串
     */
    std::string parseUtilThenExpression();
};

} // namespace CHTL

#endif // CHTL_PARSER_H
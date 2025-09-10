#ifndef JS_COMPILER_H
#define JS_COMPILER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <any>

namespace CHTL {

// JavaScript AST节点类型
enum class JSNodeType {
    PROGRAM,
    STATEMENT,
    EXPRESSION,
    DECLARATION,
    FUNCTION,
    VARIABLE,
    LITERAL,
    IDENTIFIER,
    BINARY_OPERATION,
    UNARY_OPERATION,
    CALL_EXPRESSION,
    MEMBER_EXPRESSION,
    ARRAY_EXPRESSION,
    OBJECT_EXPRESSION,
    CONDITIONAL_EXPRESSION,
    ASSIGNMENT_EXPRESSION,
    SEQUENCE_EXPRESSION,
    BLOCK_STATEMENT,
    EXPRESSION_STATEMENT,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    TRY_STATEMENT,
    CATCH_STATEMENT,
    THROW_STATEMENT,
    UNKNOWN
};

// JavaScript AST节点
struct JSNode {
    JSNodeType type;
    std::string value;
    std::vector<std::shared_ptr<JSNode>> children;
    std::map<std::string, std::any> properties;
    size_t line;
    size_t column;
    std::map<std::string, std::string> metadata;
    
    JSNode() : type(JSNodeType::UNKNOWN), line(0), column(0) {}
    JSNode(JSNodeType type) : type(type), line(0), column(0) {}
};

// JavaScript编译结果
struct JSCompileResult {
    std::string javascript;         // 编译后的JavaScript
    std::vector<std::string> errors;  // 错误列表
    std::vector<std::string> warnings;  // 警告列表
    std::map<std::string, std::string> metadata;  // 元数据
    bool success;                   // 是否成功
};

// JavaScript编译器类
class JSCompiler {
public:
    // 构造函数和析构函数
    JSCompiler();
    ~JSCompiler();
    
    // 基本编译功能
    JSCompileResult compile(const std::string& jsCode);
    JSCompileResult compileFile(const std::string& filePath);
    
    // AST解析
    std::shared_ptr<JSNode> parseAST(const std::string& jsCode);
    std::shared_ptr<JSNode> parseStatement(const std::string& statement);
    std::shared_ptr<JSNode> parseExpression(const std::string& expression);
    
    // JavaScript生成
    std::string generateJS(const std::shared_ptr<JSNode>& ast);
    std::string generateStatement(const std::shared_ptr<JSNode>& node);
    std::string generateExpression(const std::shared_ptr<JSNode>& node);
    
    // JavaScript优化
    std::string optimizeJS(const std::string& js);
    std::string minifyJS(const std::string& js);
    std::string beautifyJS(const std::string& js);
    
    // JavaScript验证
    bool validateJS(const std::string& js);
    bool validateAST(const std::shared_ptr<JSNode>& ast);
    bool validateStatement(const std::string& statement);
    bool validateExpression(const std::string& expression);
    
    // JavaScript转换
    std::string convertToES5(const std::string& js);
    std::string convertToES6(const std::string& js);
    std::string convertToTypeScript(const std::string& js);
    std::string convertFromTypeScript(const std::string& ts);
    
    // JavaScript合并
    std::string mergeJS(const std::vector<std::string>& jsFiles);
    std::string mergeAST(const std::vector<std::shared_ptr<JSNode>>& asts);
    
    // JavaScript分割
    std::vector<std::string> splitJS(const std::string& js);
    std::vector<std::shared_ptr<JSNode>> splitAST(const std::shared_ptr<JSNode>& ast);
    
    // JavaScript过滤
    std::string filterJS(const std::string& js, const std::string& pattern);
    std::vector<std::shared_ptr<JSNode>> filterAST(const std::shared_ptr<JSNode>& ast, JSNodeType type);
    
    // JavaScript排序
    std::string sortJS(const std::string& js);
    std::vector<std::shared_ptr<JSNode>> sortAST(const std::vector<std::shared_ptr<JSNode>>& nodes);
    
    // JavaScript去重
    std::string uniqueJS(const std::string& js);
    std::vector<std::shared_ptr<JSNode>> uniqueAST(const std::vector<std::shared_ptr<JSNode>>& nodes);
    
    // JavaScript统计
    std::map<std::string, int> getStatistics(const std::string& js);
    std::map<std::string, int> getASTStatistics(const std::shared_ptr<JSNode>& ast);
    
    // JavaScript调试
    std::string debugJS(const std::string& js);
    std::string debugAST(const std::shared_ptr<JSNode>& ast);
    
    // 配置管理
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setMinifyOutput(bool minify) { minifyOutput = minify; }
    void setBeautifyOutput(bool beautify) { beautifyOutput = beautify; }
    void setTargetVersion(const std::string& version) { targetVersion = version; }
    
    bool getDebugMode() const { return debugMode; }
    bool getStrictMode() const { return strictMode; }
    bool getMinifyOutput() const { return minifyOutput; }
    bool getBeautifyOutput() const { return beautifyOutput; }
    std::string getTargetVersion() const { return targetVersion; }
    
    // 错误处理
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    void clearErrors();
    void clearWarnings();
    void clearMessages();
    
    // 工具函数
    static std::string nodeTypeToString(JSNodeType type);
    static JSNodeType stringToNodeType(const std::string& type);
    static bool isValidNodeType(JSNodeType type);
    static bool isValidJS(const std::string& js);
    
    // 序列化
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    std::string toString() const;
    std::string toDebugString() const;
    
    // 反序列化
    bool fromJSON(const std::string& json);
    bool fromXML(const std::string& xml);
    bool fromYAML(const std::string& yaml);
    bool fromString(const std::string& str);
    
    // 克隆
    std::unique_ptr<JSCompiler> clone() const;
    
    // 比较
    bool equals(const JSCompiler& other) const;
    bool equals(const std::string& js) const;
    
    // 格式化
    std::string format() const;
    std::string minify() const;
    std::string beautify() const;

private:
    // 内部状态
    bool debugMode;
    bool strictMode;
    bool minifyOutput;
    bool beautifyOutput;
    std::string targetVersion;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 内部方法
    void initialize();
    void cleanup();
    bool validateInput(const std::string& js) const;
    
    // AST解析内部方法
    std::shared_ptr<JSNode> parseASTInternal(const std::string& jsCode);
    std::shared_ptr<JSNode> parseStatementInternal(const std::string& statement);
    std::shared_ptr<JSNode> parseExpressionInternal(const std::string& expression);
    
    // JavaScript生成内部方法
    std::string generateJSInternal(const std::shared_ptr<JSNode>& ast);
    std::string generateStatementInternal(const std::shared_ptr<JSNode>& node);
    std::string generateExpressionInternal(const std::shared_ptr<JSNode>& node);
    
    // JavaScript优化内部方法
    std::string optimizeJSInternal(const std::string& js);
    std::string minifyJSInternal(const std::string& js);
    std::string beautifyJSInternal(const std::string& js);
    
    // JavaScript优化辅助方法
    std::string removeUnusedVariables(const std::string& js);
    std::string mergeStringLiterals(const std::string& js);
    
    // JavaScript验证内部方法
    bool validateJSInternal(const std::string& js);
    bool validateASTInternal(const std::shared_ptr<JSNode>& ast);
    bool validateStatementInternal(const std::string& statement);
    bool validateExpressionInternal(const std::string& expression);
    
    // JavaScript转换内部方法
    std::string convertToES5Internal(const std::string& js);
    std::string convertToES6Internal(const std::string& js);
    std::string convertToTypeScriptInternal(const std::string& js);
    std::string convertFromTypeScriptInternal(const std::string& ts);
    
    // JavaScript合并内部方法
    std::string mergeJSInternal(const std::vector<std::string>& jsFiles);
    std::string mergeASTInternal(const std::vector<std::shared_ptr<JSNode>>& asts);
    
    // JavaScript分割内部方法
    std::vector<std::string> splitJSInternal(const std::string& js);
    std::vector<std::shared_ptr<JSNode>> splitASTInternal(const std::shared_ptr<JSNode>& ast);
    
    // JavaScript过滤内部方法
    std::string filterJSInternal(const std::string& js, const std::string& pattern);
    std::vector<std::shared_ptr<JSNode>> filterASTInternal(const std::shared_ptr<JSNode>& ast, JSNodeType type);
    
    // JavaScript排序内部方法
    std::string sortJSInternal(const std::string& js);
    std::vector<std::shared_ptr<JSNode>> sortASTInternal(const std::vector<std::shared_ptr<JSNode>>& nodes);
    
    // JavaScript去重内部方法
    std::string uniqueJSInternal(const std::string& js);
    std::vector<std::shared_ptr<JSNode>> uniqueASTInternal(const std::vector<std::shared_ptr<JSNode>>& nodes);
    
    // JavaScript统计内部方法
    std::map<std::string, int> getStatisticsInternal(const std::string& js);
    std::map<std::string, int> getASTStatisticsInternal(const std::shared_ptr<JSNode>& ast);
    
    // JavaScript调试内部方法
    std::string debugJSInternal(const std::string& js);
    std::string debugASTInternal(const std::shared_ptr<JSNode>& ast);
    
    // 工具函数内部方法
    std::string nodeTypeToStringInternal(JSNodeType type);
    JSNodeType stringToNodeTypeInternal(const std::string& type);
    bool isValidNodeTypeInternal(JSNodeType type);
    bool isValidJSInternal(const std::string& js);
    
    // 序列化内部方法
    std::string toJSONInternal() const;
    std::string toXMLInternal() const;
    std::string toYAMLInternal() const;
    std::string toStringInternal() const;
    std::string toDebugStringInternal() const;
    
    // 反序列化内部方法
    bool fromJSONInternal(const std::string& json);
    bool fromXMLInternal(const std::string& xml);
    bool fromYAMLInternal(const std::string& yaml);
    bool fromStringInternal(const std::string& str);
    
    // 克隆内部方法
    std::unique_ptr<JSCompiler> cloneInternal() const;
    
    // 比较内部方法
    bool equalsInternal(const JSCompiler& other) const;
    bool equalsInternal(const std::string& js) const;
    
    // 格式化内部方法
    std::string formatInternal() const;
    std::string minifyInternal() const;
    std::string beautifyInternal() const;
};

} // namespace CHTL

#endif // JS_COMPILER_H
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <variant>

namespace chtl {
namespace use_syntax {

// use语句类型
enum class UseType {
    HTML5_TYPE,     // HTML5类型
    CONFIG_GROUP    // 配置组使用
};

// HTML5类型
enum class HTML5Type {
    HTML5,          // 标准HTML5
    HTML5_STRICT,   // 严格HTML5
    HTML5_TRANSITIONAL, // 过渡HTML5
    XHTML,          // XHTML
    XHTML_STRICT,   // 严格XHTML
    XHTML_TRANSITIONAL // 过渡XHTML
};

// 配置组使用
struct ConfigGroupUse {
    std::string groupName;
    std::map<std::string, std::string> parameters;
    bool isActive;
    
    ConfigGroupUse(const std::string& name = "") : groupName(name), isActive(true) {}
};

// use语句
struct UseStatement {
    UseType type;
    std::string value;
    std::map<std::string, std::string> parameters;
    bool isActive;
    
    UseStatement(UseType t = UseType::HTML5_TYPE, const std::string& v = "") 
        : type(t), value(v), isActive(true) {}
};

// use解析器
class UseParser {
public:
    UseParser(const std::string& input);
    
    std::vector<std::shared_ptr<UseStatement>> parse();
    
private:
    std::string input_;
    size_t position_;
    
    void skipWhitespace();
    char currentChar() const;
    char peekChar() const;
    void advance();
    
    std::shared_ptr<UseStatement> parseUseStatement();
    std::string parseString();
    std::string parseIdentifier();
    std::map<std::string, std::string> parseParameters();
    
    bool isKeyword(const std::string& word) const;
    bool isUseKeyword(const std::string& word) const;
};

// use编译器
class UseCompiler {
public:
    UseCompiler();
    
    std::string compile(const std::vector<std::shared_ptr<UseStatement>>& statements);
    std::string compileUseStatement(const UseStatement& statement);
    std::string compileHTML5Type(const std::string& type);
    std::string compileConfigGroupUse(const ConfigGroupUse& config);
    
    void addUseStatement(std::shared_ptr<UseStatement> statement);
    std::shared_ptr<UseStatement> getUseStatement(const std::string& value);
    
    bool validateUseStatement(const UseStatement& statement);
    bool validateHTML5Type(const std::string& type);
    bool validateConfigGroupUse(const ConfigGroupUse& config);
    
public:
    std::map<std::string, std::shared_ptr<UseStatement>> statements_;
    
    std::string generateUseCode(const UseStatement& statement);
    std::string generateHTML5Code(const std::string& type);
    std::string generateConfigGroupCode(const ConfigGroupUse& config);
};

// use管理器
class UseManager {
public:
    UseManager();
    
    void addUseStatement(std::shared_ptr<UseStatement> statement);
    std::shared_ptr<UseStatement> getUseStatement(const std::string& value);
    
    std::string generateCode(const std::vector<std::shared_ptr<UseStatement>>& statements);
    std::string generateUseCode();
    
    bool validateUseStatement(const UseStatement& statement);
    bool validateHTML5Type(const std::string& type);
    bool validateConfigGroupUse(const ConfigGroupUse& config);
    
    void setHTML5Type(HTML5Type type);
    HTML5Type getHTML5Type() const;
    
    void addConfigGroupUse(const ConfigGroupUse& config);
    std::vector<ConfigGroupUse> getConfigGroupUses() const;
    
    void clear();
    size_t getUseStatementCount() const;
    
private:
    UseCompiler compiler_;
    HTML5Type html5Type_;
    std::vector<ConfigGroupUse> configGroupUses_;
    
    std::string processUseDependencies(const std::string& content);
    std::string resolveUseReferences(const std::string& content);
    std::string validateUseParameters(const std::string& useValue, const std::map<std::string, std::string>& parameters);
};

// use验证器
class UseValidator {
public:
    UseValidator();
    
    void setUseStatements(const std::vector<std::shared_ptr<UseStatement>>& statements);
    
    bool validate(const UseStatement& statement);
    std::vector<std::string> getValidationErrors();
    std::vector<std::string> getValidationWarnings();
    
    void clearErrors();
    void clearWarnings();
    
private:
    std::vector<std::shared_ptr<UseStatement>> statements_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    bool checkUseStatement(const UseStatement& statement);
    bool checkHTML5Type(const std::string& type);
    bool checkConfigGroupUse(const ConfigGroupUse& config);
};

// HTML5类型支持
class HTML5TypeSupport {
public:
    static std::vector<std::string> getSupportedTypes();
    static bool isSupportedType(const std::string& type);
    static std::string getTypeDescription(const std::string& type);
    static std::vector<std::string> getTypeFeatures(const std::string& type);
    
    static std::string generateHTML5Doctype(const std::string& type);
    static std::string generateXHTMLDoctype(const std::string& type);
    static std::string generateMetaTags(const std::string& type);
};

// 配置组支持
class ConfigGroupSupport {
public:
    static std::vector<std::string> getAvailableConfigGroups();
    static bool isAvailableConfigGroup(const std::string& groupName);
    static std::string getConfigGroupDescription(const std::string& groupName);
    static std::vector<std::string> getConfigGroupParameters(const std::string& groupName);
    
    static std::string generateConfigGroupCode(const std::string& groupName, const std::map<std::string, std::string>& parameters);
    static std::string generateConfigGroupImport(const std::string& groupName);
    static std::string generateConfigGroupUsage(const std::string& groupName, const std::map<std::string, std::string>& parameters);
};

} // namespace use_syntax
} // namespace chtl
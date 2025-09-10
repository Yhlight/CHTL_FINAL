#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <variant>

namespace chtl {
namespace origin_embed {

// 原始嵌入类型
enum class OriginType {
    HTML,           // HTML原始嵌入
    CSS,            // CSS原始嵌入
    JS,             // JavaScript原始嵌入
    VUE,            // Vue.js原始嵌入
    REACT,          // React原始嵌入
    ANGULAR,        // Angular原始嵌入
    CUSTOM          // 自定义类型原始嵌入
};

// 原始嵌入内容
struct OriginContent {
    OriginType type;
    std::string content;
    std::string language;
    std::map<std::string, std::string> attributes;
    bool isProcessed;
    
    OriginContent(OriginType t = OriginType::HTML, const std::string& c = "") 
        : type(t), content(c), isProcessed(false) {}
};

// 原始嵌入块
struct OriginBlock {
    std::string name;
    OriginType type;
    std::string content;
    std::map<std::string, std::string> attributes;
    bool isActive;
    
    OriginBlock(const std::string& n = "", OriginType t = OriginType::HTML) 
        : name(n), type(t), isActive(true) {}
};

// 原始嵌入解析器
class OriginEmbedParser {
public:
    OriginEmbedParser(const std::string& input);
    
    std::vector<std::shared_ptr<OriginBlock>> parse();
    
private:
    std::string input_;
    size_t position_;
    
    void skipWhitespace();
    char currentChar() const;
    char peekChar() const;
    void advance();
    
    std::shared_ptr<OriginBlock> parseOriginBlock();
    OriginType parseOriginType();
    std::string parseString();
    std::string parseIdentifier();
    std::map<std::string, std::string> parseAttributes();
    
    bool isKeyword(const std::string& word) const;
    bool isOriginKeyword(const std::string& word) const;
};

// 原始嵌入编译器
class OriginEmbedCompiler {
public:
    OriginEmbedCompiler();
    
    std::string compile(const std::vector<std::shared_ptr<OriginBlock>>& blocks);
    std::string compileOriginBlock(const OriginBlock& block);
    std::string compileHTML(const std::string& content);
    std::string compileCSS(const std::string& content);
    std::string compileJS(const std::string& content);
    std::string compileVue(const std::string& content);
    std::string compileReact(const std::string& content);
    std::string compileAngular(const std::string& content);
    std::string compileCustom(const std::string& content, const std::string& type);
    
    void addOriginBlock(std::shared_ptr<OriginBlock> block);
    std::shared_ptr<OriginBlock> getOriginBlock(const std::string& name);
    
    bool validateOriginBlock(const OriginBlock& block);
    bool validateHTML(const std::string& content);
    bool validateCSS(const std::string& content);
    bool validateJS(const std::string& content);
    bool validateVue(const std::string& content);
    bool validateReact(const std::string& content);
    bool validateAngular(const std::string& content);
    bool validateCustom(const std::string& content, const std::string& type);
    
private:
    std::map<std::string, std::shared_ptr<OriginBlock>> blocks_;
    
    std::string generateOriginCode(const OriginBlock& block);
    std::string generateHTMLCode(const std::string& content);
    std::string generateCSSCode(const std::string& content);
    std::string generateJSCode(const std::string& content);
    std::string generateVueCode(const std::string& content);
    std::string generateReactCode(const std::string& content);
    std::string generateAngularCode(const std::string& content);
    std::string generateCustomCode(const std::string& content, const std::string& type);
};

// 原始嵌入管理器
class OriginEmbedManager {
public:
    OriginEmbedManager();
    
    void addOriginBlock(std::shared_ptr<OriginBlock> block);
    std::shared_ptr<OriginBlock> getOriginBlock(const std::string& name);
    
    std::string generateCode(const std::vector<std::shared_ptr<OriginBlock>>& blocks);
    std::string generateOriginCode();
    
    bool validateOriginBlock(const OriginBlock& block);
    bool validateHTML(const std::string& content);
    bool validateCSS(const std::string& content);
    bool validateJS(const std::string& content);
    bool validateVue(const std::string& content);
    bool validateReact(const std::string& content);
    bool validateAngular(const std::string& content);
    bool validateCustom(const std::string& content, const std::string& type);
    
    void registerCustomType(const std::string& type, const std::string& processor);
    std::string getCustomTypeProcessor(const std::string& type);
    std::vector<std::string> getRegisteredCustomTypes() const;
    
    void clear();
    size_t getOriginBlockCount() const;
    
private:
    OriginEmbedCompiler compiler_;
    std::map<std::string, std::string> customTypeProcessors_;
    
    std::string processOriginDependencies(const std::string& content);
    std::string resolveOriginReferences(const std::string& content);
    std::string validateOriginParameters(const std::string& blockName, const std::map<std::string, std::string>& parameters);
};

// 原始嵌入验证器
class OriginEmbedValidator {
public:
    OriginEmbedValidator();
    
    void setOriginBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks);
    
    bool validate(const OriginBlock& block);
    std::vector<std::string> getValidationErrors();
    std::vector<std::string> getValidationWarnings();
    
    void clearErrors();
    void clearWarnings();
    
private:
    std::vector<std::shared_ptr<OriginBlock>> blocks_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    bool checkOriginBlock(const OriginBlock& block);
    bool checkHTML(const std::string& content);
    bool checkCSS(const std::string& content);
    bool checkJS(const std::string& content);
    bool checkVue(const std::string& content);
    bool checkReact(const std::string& content);
    bool checkAngular(const std::string& content);
    bool checkCustom(const std::string& content, const std::string& type);
};

// 原始嵌入处理器
class OriginEmbedProcessor {
public:
    static std::string processHTML(const std::string& content);
    static std::string processCSS(const std::string& content);
    static std::string processJS(const std::string& content);
    static std::string processVue(const std::string& content);
    static std::string processReact(const std::string& content);
    static std::string processAngular(const std::string& content);
    static std::string processCustom(const std::string& content, const std::string& type);
    
    static std::string extractHTML(const std::string& content);
    static std::string extractCSS(const std::string& content);
    static std::string extractJS(const std::string& content);
    static std::string extractVue(const std::string& content);
    static std::string extractReact(const std::string& content);
    static std::string extractAngular(const std::string& content);
    static std::string extractCustom(const std::string& content, const std::string& type);
    
    static std::string mergeOriginBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks);
    static std::string mergeHTMLBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks);
    static std::string mergeCSSBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks);
    static std::string mergeJSBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks);
};

// 原始嵌入支持
class OriginEmbedSupport {
public:
    static std::vector<std::string> getSupportedTypes();
    static bool isSupportedType(const std::string& type);
    static std::string getTypeDescription(const std::string& type);
    static std::vector<std::string> getTypeFeatures(const std::string& type);
    
    static std::string generateOriginBlock(const std::string& type, const std::string& content);
    static std::string generateOriginImport(const std::string& type);
    static std::string generateOriginUsage(const std::string& type, const std::string& content);
};

} // namespace origin_embed
} // namespace chtl
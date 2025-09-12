#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <regex>

namespace CHTL {

/**
 * 语法参数
 */
class Arg {
public:
    Arg();
    Arg(const std::string& value);
    ~Arg();
    
    // 基本信息
    const std::string& getValue() const;
    void setValue(const std::string& value);
    
    // 绑定处理
    void bind(const std::string& pattern, std::function<std::string(const std::string&)> handler);
    std::string match(const std::string& pattern, std::function<std::string(const std::string&)> handler) const;
    
    // 值填充
    void fillValue(const Arg& other);
    void fillValue(const std::vector<Arg>& others);
    
    // 转换
    std::string transform(const std::string& template_str) const;
    
    // 输出
    void print() const;
    std::string toString() const;
    
    // 验证
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    std::string value_;
    std::map<std::string, std::function<std::string(const std::string&)>> handlers_;
    
    // 辅助函数
    std::string processTemplate(const std::string& template_str) const;
    std::string replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) const;
};

/**
 * 原子参数
 */
class AtomArg : public Arg {
public:
    AtomArg();
    AtomArg(const std::string& value, const std::string& type = "");
    ~AtomArg();
    
    // 类型信息
    const std::string& getType() const;
    void setType(const std::string& type);
    
    // 原子操作
    bool isAtom() const;
    std::string getAtomValue() const;
    void setAtomValue(const std::string& value);
    
    // 验证
    bool validateAtom() const;
    std::vector<std::string> getAtomValidationErrors() const;
    
private:
    std::string type_;
    bool is_atom_;
    
    // 辅助函数
    bool isAtomicValue(const std::string& value) const;
    std::string extractAtomValue(const std::string& value) const;
};

/**
 * 语法分析器
 */
class Syntax {
public:
    Syntax();
    ~Syntax();
    
    // 语法分析
    static Arg analyze(const std::string& pattern);
    static std::vector<Arg> analyzeMultiple(const std::string& pattern);
    
    // 模式匹配
    static bool matches(const std::string& input, const std::string& pattern);
    static std::vector<std::string> extractMatches(const std::string& input, const std::string& pattern);
    
    // 语法验证
    static bool isValidPattern(const std::string& pattern);
    static std::vector<std::string> getPatternValidationErrors(const std::string& pattern);
    
private:
    // 分析辅助函数
    static std::vector<std::string> tokenizePattern(const std::string& pattern);
    static bool isSpecialCharacter(char c);
    static std::string escapeSpecialCharacters(const std::string& str);
    static std::string unescapeSpecialCharacters(const std::string& str);
};

/**
 * CJMOD扫描器
 */
class CJMODScanner {
public:
    CJMODScanner();
    ~CJMODScanner();
    
    // 扫描操作
    static Arg scan(const Arg& args, const std::string& keyword);
    static std::vector<Arg> scanMultiple(const std::vector<Arg>& args, const std::string& keyword);
    
    // 双指针扫描
    static Arg dualPointerScan(const std::string& content, const std::string& keyword);
    static std::vector<Arg> dualPointerScanMultiple(const std::string& content, const std::vector<std::string>& keywords);
    
    // 前置截取
    static Arg preTruncateScan(const std::string& content, const std::string& keyword);
    static std::vector<Arg> preTruncateScanMultiple(const std::string& content, const std::vector<std::string>& keywords);
    
    // 扫描配置
    void setScanMode(const std::string& mode);
    void setScanOptions(const std::map<std::string, std::string>& options);
    
    const std::string& getScanMode() const;
    const std::map<std::string, std::string>& getScanOptions() const;
    
private:
    std::string scan_mode_;
    std::map<std::string, std::string> scan_options_;
    
    // 扫描辅助函数
    static std::pair<size_t, size_t> findKeywordBounds(const std::string& content, const std::string& keyword);
    static std::string extractContentRange(const std::string& content, size_t start, size_t end);
    static bool isValidContentRange(const std::string& content, size_t start, size_t end);
    static std::vector<std::pair<size_t, size_t>> findAllKeywordBounds(const std::string& content, const std::string& keyword);
};

/**
 * CJMOD生成器
 */
class CJMODGenerator {
public:
    CJMODGenerator();
    ~CJMODGenerator();
    
    // 生成操作
    static std::string exportResult(const Arg& args);
    static std::string exportResult(const std::vector<Arg>& args);
    static std::string generateCode(const Arg& args, const std::string& template_str);
    
    // 代码生成
    static std::string generateFunction(const Arg& args, const std::string& functionName);
    static std::string generateClass(const Arg& args, const std::string& className);
    static std::string generateModule(const Arg& args, const std::string& moduleName);
    
    // 生成配置
    void setGenerationMode(const std::string& mode);
    void setGenerationOptions(const std::map<std::string, std::string>& options);
    
    const std::string& getGenerationMode() const;
    const std::map<std::string, std::string>& getGenerationOptions() const;
    
private:
    std::string generation_mode_;
    std::map<std::string, std::string> generation_options_;
    
    // 生成辅助函数
    static std::string processTemplate(const std::string& template_str, const Arg& args);
    static std::string processTemplate(const std::string& template_str, const std::vector<Arg>& args);
    static std::string replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements);
    static std::string formatCode(const std::string& code);
};

/**
 * CHTL JS函数
 */
class CHTLJSFunction {
public:
    CHTLJSFunction();
    CHTLJSFunction(const std::string& name, const std::string& body);
    ~CHTLJSFunction();
    
    // 基本信息
    const std::string& getName() const;
    void setName(const std::string& name);
    
    const std::string& getBody() const;
    void setBody(const std::string& body);
    
    // 函数操作
    std::string generateFunction() const;
    std::string generateFunctionCall(const std::vector<std::string>& args) const;
    
    // 参数处理
    void addParameter(const std::string& param);
    void removeParameter(const std::string& param);
    std::vector<std::string> getParameters() const;
    
    // 验证
    bool validateFunction() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    std::string name_;
    std::string body_;
    std::vector<std::string> parameters_;
    
    // 辅助函数
    std::string formatFunctionName(const std::string& name) const;
    std::string formatFunctionBody(const std::string& body) const;
    std::string formatParameters(const std::vector<std::string>& params) const;
    bool isValidFunctionName(const std::string& name) const;
    bool isValidFunctionBody(const std::string& body) const;
};

/**
 * CJMOD API管理器
 */
class CJMODAPIManager {
public:
    CJMODAPIManager();
    ~CJMODAPIManager();
    
    // API管理
    void registerFunction(const std::string& name, std::shared_ptr<CHTLJSFunction> function);
    void unregisterFunction(const std::string& name);
    std::shared_ptr<CHTLJSFunction> getFunction(const std::string& name) const;
    
    // 函数查询
    std::vector<std::string> getFunctionNames() const;
    std::vector<std::shared_ptr<CHTLJSFunction>> getAllFunctions() const;
    bool hasFunction(const std::string& name) const;
    
    // 扫描和生成
    Arg scanAndGenerate(const std::string& content, const std::string& pattern);
    std::string generateCode(const std::string& content, const std::string& template_str);
    
    // 配置
    void setScanner(std::shared_ptr<CJMODScanner> scanner);
    void setGenerator(std::shared_ptr<CJMODGenerator> generator);
    
    std::shared_ptr<CJMODScanner> getScanner() const;
    std::shared_ptr<CJMODGenerator> getGenerator() const;
    
    // 清理
    void clear();
    void clearFunctions();
    
private:
    std::map<std::string, std::shared_ptr<CHTLJSFunction>> functions_;
    std::shared_ptr<CJMODScanner> scanner_;
    std::shared_ptr<CJMODGenerator> generator_;
    
    // 辅助函数
    std::string processContent(const std::string& content, const std::string& pattern) const;
    std::string applyFunctions(const std::string& content) const;
    bool isFunctionCall(const std::string& content, size_t position) const;
    std::string extractFunctionName(const std::string& content, size_t position) const;
};

} // namespace CHTL
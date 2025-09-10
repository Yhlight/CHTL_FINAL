#ifndef CSS_COMPILER_H
#define CSS_COMPILER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// CSS规则结构
struct CSSRule {
    std::string selector;           // 选择器
    std::map<std::string, std::string> properties;  // 属性-值对
    std::vector<CSSRule> nestedRules;  // 嵌套规则
    size_t line;                    // 行号
    size_t column;                  // 列号
    std::map<std::string, std::string> metadata;  // 元数据
};

// CSS编译结果
struct CSSCompileResult {
    std::string css;                // 编译后的CSS
    std::vector<std::string> errors;  // 错误列表
    std::vector<std::string> warnings;  // 警告列表
    std::map<std::string, std::string> metadata;  // 元数据
    bool success;                   // 是否成功
};

// CSS编译器类
class CSSCompiler {
public:
    // 构造函数和析构函数
    CSSCompiler();
    ~CSSCompiler();
    
    // 基本编译功能
    CSSCompileResult compile(const std::string& cssCode);
    CSSCompileResult compileFile(const std::string& filePath);
    
    // 规则解析
    std::vector<CSSRule> parseRules(const std::string& cssCode);
    CSSRule parseRule(const std::string& ruleCode);
    
    // 选择器解析
    std::string parseSelector(const std::string& selector);
    std::vector<std::string> parseComplexSelector(const std::string& selector);
    
    // 属性解析
    std::map<std::string, std::string> parseProperties(const std::string& propertiesCode);
    std::pair<std::string, std::string> parseProperty(const std::string& propertyCode);
    
    // CSS生成
    std::string generateCSS(const std::vector<CSSRule>& rules);
    std::string generateRule(const CSSRule& rule);
    std::string generateSelector(const std::string& selector);
    std::string generateProperties(const std::map<std::string, std::string>& properties);
    
    // CSS优化
    std::string optimizeCSS(const std::string& css);
    std::string minifyCSS(const std::string& css);
    std::string beautifyCSS(const std::string& css);
    
    // CSS验证
    bool validateCSS(const std::string& css);
    bool validateRule(const CSSRule& rule);
    bool validateSelector(const std::string& selector);
    bool validateProperty(const std::string& property, const std::string& value);
    
    // CSS转换
    std::string convertToSCSS(const std::string& css);
    std::string convertToLESS(const std::string& css);
    std::string convertToSASS(const std::string& css);
    std::string convertFromSCSS(const std::string& scss);
    std::string convertFromLESS(const std::string& less);
    std::string convertFromSASS(const std::string& sass);
    
    // CSS合并
    std::string mergeCSS(const std::vector<std::string>& cssFiles);
    std::string mergeRules(const std::vector<CSSRule>& rules);
    
    // CSS分割
    std::vector<std::string> splitCSS(const std::string& css);
    std::vector<CSSRule> splitRules(const std::string& css);
    
    // CSS过滤
    std::string filterCSS(const std::string& css, const std::string& selector);
    std::vector<CSSRule> filterRules(const std::vector<CSSRule>& rules, const std::string& selector);
    
    // CSS排序
    std::string sortCSS(const std::string& css);
    std::vector<CSSRule> sortRules(const std::vector<CSSRule>& rules);
    
    // CSS去重
    std::string uniqueCSS(const std::string& css);
    std::vector<CSSRule> uniqueRules(const std::vector<CSSRule>& rules);
    
    // CSS统计
    std::map<std::string, int> getStatistics(const std::string& css);
    std::map<std::string, int> getRuleStatistics(const std::vector<CSSRule>& rules);
    
    // CSS调试
    std::string debugCSS(const std::string& css);
    std::string debugRule(const CSSRule& rule);
    
    // 配置管理
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setMinifyOutput(bool minify) { minifyOutput = minify; }
    void setBeautifyOutput(bool beautify) { beautifyOutput = beautify; }
    
    bool getDebugMode() const { return debugMode; }
    bool getStrictMode() const { return strictMode; }
    bool getMinifyOutput() const { return minifyOutput; }
    bool getBeautifyOutput() const { return beautifyOutput; }
    
    // 错误处理
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    void clearErrors();
    void clearWarnings();
    void clearMessages();
    
    // 工具函数
    static std::string normalizeSelector(const std::string& selector);
    static std::string normalizeProperty(const std::string& property);
    static std::string normalizeValue(const std::string& value);
    static bool isValidSelector(const std::string& selector);
    static bool isValidProperty(const std::string& property);
    static bool isValidValue(const std::string& value);
    
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
    std::unique_ptr<CSSCompiler> clone() const;
    
    // 比较
    bool equals(const CSSCompiler& other) const;
    bool equals(const std::string& css) const;
    
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
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 内部方法
    void initialize();
    void cleanup();
    bool validateInput(const std::string& css) const;
    
    // 规则解析内部方法
    std::vector<CSSRule> parseRulesInternal(const std::string& cssCode);
    CSSRule parseRuleInternal(const std::string& ruleCode);
    
    // 选择器解析内部方法
    std::string parseSelectorInternal(const std::string& selector);
    std::vector<std::string> parseComplexSelectorInternal(const std::string& selector);
    
    // 属性解析内部方法
    std::map<std::string, std::string> parsePropertiesInternal(const std::string& propertiesCode);
    std::pair<std::string, std::string> parsePropertyInternal(const std::string& propertyCode);
    
    // CSS生成内部方法
    std::string generateCSSInternal(const std::vector<CSSRule>& rules);
    std::string generateRuleInternal(const CSSRule& rule);
    std::string generateSelectorInternal(const std::string& selector);
    std::string generatePropertiesInternal(const std::map<std::string, std::string>& properties);
    
    // CSS优化内部方法
    std::string optimizeCSSInternal(const std::string& css);
    std::string minifyCSSInternal(const std::string& css);
    std::string beautifyCSSInternal(const std::string& css);
    
    // CSS验证内部方法
    bool validateCSSInternal(const std::string& css);
    bool validateRuleInternal(const CSSRule& rule);
    bool validateSelectorInternal(const std::string& selector);
    bool validatePropertyInternal(const std::string& property, const std::string& value);
    
    // CSS转换内部方法
    std::string convertToSCSSInternal(const std::string& css);
    std::string convertToLESSInternal(const std::string& css);
    std::string convertToSASSInternal(const std::string& css);
    std::string convertFromSCSSInternal(const std::string& scss);
    std::string convertFromLESSInternal(const std::string& less);
    std::string convertFromSASSInternal(const std::string& sass);
    
    // CSS合并内部方法
    std::string mergeCSSInternal(const std::vector<std::string>& cssFiles);
    std::string mergeRulesInternal(const std::vector<CSSRule>& rules);
    
    // CSS分割内部方法
    std::vector<std::string> splitCSSInternal(const std::string& css);
    std::vector<CSSRule> splitRulesInternal(const std::string& css);
    
    // CSS过滤内部方法
    std::string filterCSSInternal(const std::string& css, const std::string& selector);
    std::vector<CSSRule> filterRulesInternal(const std::vector<CSSRule>& rules, const std::string& selector);
    
    // CSS排序内部方法
    std::string sortCSSInternal(const std::string& css);
    std::vector<CSSRule> sortRulesInternal(const std::vector<CSSRule>& rules);
    
    // CSS去重内部方法
    std::string uniqueCSSInternal(const std::string& css);
    std::vector<CSSRule> uniqueRulesInternal(const std::vector<CSSRule>& rules);
    
    // CSS统计内部方法
    std::map<std::string, int> getStatisticsInternal(const std::string& css);
    std::map<std::string, int> getRuleStatisticsInternal(const std::vector<CSSRule>& rules);
    
    // CSS调试内部方法
    std::string debugCSSInternal(const std::string& css);
    std::string debugRuleInternal(const CSSRule& rule);
    
    // 工具函数内部方法
    std::string normalizeSelectorInternal(const std::string& selector);
    std::string normalizePropertyInternal(const std::string& property);
    std::string normalizeValueInternal(const std::string& value);
    bool isValidSelectorInternal(const std::string& selector);
    bool isValidPropertyInternal(const std::string& property);
    bool isValidValueInternal(const std::string& value);
    
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
    std::unique_ptr<CSSCompiler> cloneInternal() const;
    
    // 比较内部方法
    bool equalsInternal(const CSSCompiler& other) const;
    bool equalsInternal(const std::string& css) const;
    
    // 格式化内部方法
    std::string formatInternal() const;
    std::string minifyInternal() const;
    std::string beautifyInternal() const;
};

} // namespace CHTL

#endif // CSS_COMPILER_H
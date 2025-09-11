#ifndef CHTL_JS_GLOBAL_MAP_H
#define CHTL_JS_GLOBAL_MAP_H

#include "CHTLJSToken.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL_JS {

class CHTLJSGlobalMap {
private:
    // 关键字映射
    std::map<std::string, CHTLJSTokenType> keywordMap;
    std::map<CHTLJSTokenType, std::string> reverseKeywordMap;
    
    // 运算符映射
    std::map<std::string, CHTLJSTokenType> operatorMap;
    std::map<CHTLJSTokenType, std::string> reverseOperatorMap;
    
    // 标点符号映射
    std::map<char, CHTLJSTokenType> punctuationMap;
    std::map<CHTLJSTokenType, char> reversePunctuationMap;
    
    // 语法边界映射
    std::map<std::string, std::string> syntaxBoundaryMap;
    std::map<std::string, std::vector<std::string>> syntaxPatternMap;
    
    // CHTL JS特有关键字
    std::vector<std::string> chtlJSKeywords;
    std::map<std::string, std::string> chtlJSFunctionMap;
    std::map<std::string, std::string> chtlJSSyntaxMap;
    
    // 统一扫描器相关
    std::map<std::string, std::string> placeholderMap;
    std::map<std::string, std::string> reversePlaceholderMap;
    std::vector<std::string> activePlaceholders;
    
    // 配置选项
    bool caseSensitive;
    bool strictMode;
    bool debugMode;
    std::string languageVersion;
    
    // 统计信息
    std::map<std::string, int> usageCount;
    std::map<CHTLJSTokenType, int> tokenTypeCount;

public:
    CHTLJSGlobalMap();
    virtual ~CHTLJSGlobalMap() = default;
    
    // 初始化
    void initialize();
    void initializeKeywords();
    void initializeOperators();
    void initializePunctuation();
    void initializeSyntaxBoundaries();
    void initializeCHTLJSSyntax();
    
    // 关键字管理
    void addKeyword(const std::string& keyword, CHTLJSTokenType type);
    void removeKeyword(const std::string& keyword);
    bool hasKeyword(const std::string& keyword) const;
    CHTLJSTokenType getKeywordType(const std::string& keyword) const;
    std::string getKeywordName(CHTLJSTokenType type) const;
    std::vector<std::string> getAllKeywords() const;
    
    // 运算符管理
    void addOperator(const std::string& op, CHTLJSTokenType type);
    void removeOperator(const std::string& op);
    bool hasOperator(const std::string& op) const;
    CHTLJSTokenType getOperatorType(const std::string& op) const;
    std::string getOperatorName(CHTLJSTokenType type) const;
    std::vector<std::string> getAllOperators() const;
    
    // 标点符号管理
    void addPunctuation(char punct, CHTLJSTokenType type);
    void removePunctuation(char punct);
    bool hasPunctuation(char punct) const;
    CHTLJSTokenType getPunctuationType(char punct) const;
    char getPunctuationChar(CHTLJSTokenType type) const;
    std::vector<char> getAllPunctuation() const;
    
    // 语法边界管理
    void addSyntaxBoundary(const std::string& start, const std::string& end);
    void removeSyntaxBoundary(const std::string& start);
    bool hasSyntaxBoundary(const std::string& start) const;
    std::string getSyntaxBoundaryEnd(const std::string& start) const;
    std::vector<std::string> getAllSyntaxBoundaries() const;
    
    // CHTL JS语法管理
    void addCHTLJSKeyword(const std::string& keyword);
    void removeCHTLJSKeyword(const std::string& keyword);
    bool hasCHTLJSKeyword(const std::string& keyword) const;
    std::vector<std::string> getAllCHTLJSKeywords() const;
    
    void addCHTLJSFunction(const std::string& name, const std::string& signature);
    void removeCHTLJSFunction(const std::string& name);
    bool hasCHTLJSFunction(const std::string& name) const;
    std::string getCHTLJSFunctionSignature(const std::string& name) const;
    std::vector<std::string> getAllCHTLJSFunctions() const;
    
    void addCHTLJSSyntax(const std::string& syntax, const std::string& description);
    void removeCHTLJSSyntax(const std::string& syntax);
    bool hasCHTLJSSyntax(const std::string& syntax) const;
    std::string getCHTLJSSyntaxDescription(const std::string& syntax) const;
    std::vector<std::string> getAllCHTLJSSyntax() const;
    
    // 占位符管理
    void addPlaceholder(const std::string& placeholder, const std::string& content);
    void removePlaceholder(const std::string& placeholder);
    bool hasPlaceholder(const std::string& placeholder) const;
    std::string getPlaceholderContent(const std::string& placeholder) const;
    std::string getPlaceholderByContent(const std::string& content) const;
    void clearPlaceholders();
    std::vector<std::string> getAllPlaceholders() const;
    
    // 配置管理
    void setCaseSensitive(bool sensitive) { caseSensitive = sensitive; }
    bool isCaseSensitive() const { return caseSensitive; }
    
    void setStrictMode(bool strict) { strictMode = strict; }
    bool isStrictMode() const { return strictMode; }
    
    void setDebugMode(bool debug) { debugMode = debug; }
    bool isDebugMode() const { return debugMode; }
    
    void setLanguageVersion(const std::string& version) { languageVersion = version; }
    const std::string& getLanguageVersion() const { return languageVersion; }
    
    // 统计管理
    void incrementUsage(const std::string& item);
    void incrementTokenTypeUsage(CHTLJSTokenType type);
    int getUsageCount(const std::string& item) const;
    int getTokenTypeUsageCount(CHTLJSTokenType type) const;
    void resetStatistics();
    std::map<std::string, int> getUsageStatistics() const;
    std::map<CHTLJSTokenType, int> getTokenTypeStatistics() const;
    
    // 查找和匹配
    CHTLJSTokenType findTokenType(const std::string& text) const;
    std::string findTokenName(CHTLJSTokenType type) const;
    bool isReservedWord(const std::string& word) const;
    bool isBuiltinFunction(const std::string& function) const;
    bool isCHTLJSFunction(const std::string& function) const;
    
    // 验证
    bool validateKeyword(const std::string& keyword) const;
    bool validateOperator(const std::string& op) const;
    bool validatePunctuation(char punct) const;
    bool validateSyntax(const std::string& syntax) const;
    bool validateIdentifier(const std::string& identifier) const;
    
    // 转换
    std::string toJavaScript(const std::string& chtljs) const;
    std::string toCHTLJS(const std::string& javascript) const;
    std::string toHTML(const std::string& chtljs) const;
    std::string toCSS(const std::string& chtljs) const;
    
    // 调试和诊断
    std::string getDebugInfo() const;
    std::string getStatisticsInfo() const;
    void printDebugInfo() const;
    void printStatistics() const;
    
    // 导入导出
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& json);
    
    // 重置和清理
    void reset();
    void clear();
    void clearKeywords();
    void clearOperators();
    void clearPunctuation();
    void clearSyntaxBoundaries();
    void clearCHTLJSSyntax();
    
    // 优化
    void optimize();
    void compress();
    void decompress();
    
    // 扩展
    void addCustomKeyword(const std::string& keyword, CHTLJSTokenType type);
    void addCustomOperator(const std::string& op, CHTLJSTokenType type);
    void addCustomSyntax(const std::string& syntax, const std::string& description);
    
    // 兼容性
    bool isCompatible(const std::string& version) const;
    std::vector<std::string> getCompatibilityIssues() const;
    void fixCompatibilityIssues();

private:
    // 辅助方法
    std::string normalizeText(const std::string& text) const;
    bool matchesPattern(const std::string& text, const std::string& pattern) const;
    std::vector<std::string> splitPattern(const std::string& pattern) const;
    bool isPatternMatch(const std::string& text, const std::vector<std::string>& patterns) const;
    
    // 内部验证
    bool isValidKeyword(const std::string& keyword) const;
    bool isValidOperator(const std::string& op) const;
    bool isValidPunctuation(char punct) const;
    bool isValidSyntax(const std::string& syntax) const;
    
    // 转换辅助
    std::string convertToJavaScript(const std::string& chtljs) const;
    std::string convertToCHTLJS(const std::string& javascript) const;
    std::string convertToHTML(const std::string& chtljs) const;
    std::string convertToCSS(const std::string& chtljs) const;
    
    // 统计辅助
    void updateUsageCount(const std::string& item);
    void updateTokenTypeCount(CHTLJSTokenType type);
    
    // 调试辅助
    std::string formatDebugInfo() const;
    std::string formatStatistics() const;
    
    // 文件操作辅助
    bool writeToFile(const std::string& filename, const std::string& content) const;
    std::string readFromFile(const std::string& filename) const;
    
    // JSON操作辅助
    std::string toJSON() const;
    bool fromJSON(const std::string& json);
    
    // 优化辅助
    void optimizeKeywords();
    void optimizeOperators();
    void optimizePunctuation();
    void optimizeSyntaxBoundaries();
    
    // 压缩辅助
    void compressData();
    void decompressData();
    
    // 兼容性辅助
    bool checkVersionCompatibility(const std::string& version) const;
    std::vector<std::string> findCompatibilityIssues() const;
    void applyCompatibilityFixes();
};

} // namespace CHTL_JS

#endif // CHTL_JS_GLOBAL_MAP_H
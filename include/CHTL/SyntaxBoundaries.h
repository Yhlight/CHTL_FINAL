#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <regex>

namespace CHTL {

/**
 * 语法边界类型
 */
enum class SyntaxBoundaryType {
    GLOBAL_STYLE,    // 全局样式块
    LOCAL_STYLE,     // 局部样式块
    GLOBAL_SCRIPT,   // 全局脚本块
    LOCAL_SCRIPT,    // 局部脚本块
    CHTL_JS,         // CHTL JS代码
    JS,              // JavaScript代码
    CSS,             // CSS代码
    HTML             // HTML代码
};

/**
 * 语法边界规则
 */
struct SyntaxBoundaryRule {
    SyntaxBoundaryType type;
    std::string start_pattern;
    std::string end_pattern;
    std::set<std::string> allowed_syntax;
    std::set<std::string> forbidden_syntax;
    bool strict_mode;
    
    SyntaxBoundaryRule() : strict_mode(false) {}
};

/**
 * 语法边界检测器
 */
class SyntaxBoundaryDetector {
public:
    SyntaxBoundaryDetector();
    ~SyntaxBoundaryDetector();
    
    // 边界检测
    SyntaxBoundaryType detectBoundaryType(const std::string& content, size_t position) const;
    std::vector<std::pair<size_t, size_t>> findBoundaries(const std::string& content) const;
    std::vector<std::pair<size_t, size_t>> findBoundariesByType(const std::string& content, SyntaxBoundaryType type) const;
    
    // 语法验证
    bool validateSyntax(const std::string& content, SyntaxBoundaryType type) const;
    std::vector<std::string> getValidationErrors(const std::string& content, SyntaxBoundaryType type) const;
    
    // 规则管理
    void addRule(const SyntaxBoundaryRule& rule);
    void removeRule(SyntaxBoundaryType type);
    const SyntaxBoundaryRule& getRule(SyntaxBoundaryType type) const;
    const std::map<SyntaxBoundaryType, SyntaxBoundaryRule>& getRules() const;
    
    // 清理
    void clear();
    
private:
    std::map<SyntaxBoundaryType, SyntaxBoundaryRule> rules_;
    
    // 检测辅助函数
    bool matchesPattern(const std::string& content, size_t position, const std::string& pattern) const;
    size_t findPatternEnd(const std::string& content, size_t start, const std::string& pattern) const;
    bool isAllowedSyntax(const std::string& content, size_t position, const std::set<std::string>& allowed) const;
    bool isForbiddenSyntax(const std::string& content, size_t position, const std::set<std::string>& forbidden) const;
    
    // 初始化默认规则
    void initializeDefaultRules();
};

/**
 * 宽判严判处理器
 */
class WideStrictProcessor {
public:
    WideStrictProcessor();
    ~WideStrictProcessor();
    
    // 宽判处理
    std::string processWide(const std::string& content) const;
    std::vector<std::string> extractWideBlocks(const std::string& content) const;
    
    // 严判处理
    std::string processStrict(const std::string& content) const;
    std::vector<std::string> extractStrictBlocks(const std::string& content) const;
    
    // 混合处理
    std::string processMixed(const std::string& content) const;
    std::map<SyntaxBoundaryType, std::vector<std::string>> extractMixedBlocks(const std::string& content) const;
    
    // 配置
    void setWideMode(bool enabled);
    void setStrictMode(bool enabled);
    void setMixedMode(bool enabled);
    
    bool isWideMode() const;
    bool isStrictMode() const;
    bool isMixedMode() const;
    
private:
    bool wide_mode_;
    bool strict_mode_;
    bool mixed_mode_;
    
    SyntaxBoundaryDetector detector_;
    
    // 处理辅助函数
    std::string processWideBlock(const std::string& block) const;
    std::string processStrictBlock(const std::string& block) const;
    std::string processMixedBlock(const std::string& block, SyntaxBoundaryType type) const;
    
    // 块提取辅助函数
    std::vector<std::string> extractBlocksByPattern(const std::string& content, const std::string& startPattern, const std::string& endPattern) const;
    std::vector<std::string> extractBlocksByType(const std::string& content, SyntaxBoundaryType type) const;
};

/**
 * 语法边界管理器
 */
class SyntaxBoundaryManager {
public:
    SyntaxBoundaryManager();
    ~SyntaxBoundaryManager();
    
    // 边界管理
    void addBoundary(SyntaxBoundaryType type, size_t start, size_t end);
    void removeBoundary(SyntaxBoundaryType type, size_t start);
    void clearBoundaries();
    void clearBoundariesByType(SyntaxBoundaryType type);
    
    // 边界查询
    std::vector<std::pair<size_t, size_t>> getBoundaries(SyntaxBoundaryType type) const;
    std::vector<std::pair<size_t, size_t>> getAllBoundaries() const;
    bool hasBoundary(SyntaxBoundaryType type, size_t start) const;
    
    // 边界处理
    std::string extractBoundaryContent(const std::string& content, SyntaxBoundaryType type, size_t start) const;
    std::string replaceBoundaryContent(const std::string& content, SyntaxBoundaryType type, size_t start, const std::string& newContent) const;
    
    // 边界验证
    bool validateBoundaries(const std::string& content) const;
    std::vector<std::string> getBoundaryValidationErrors(const std::string& content) const;
    
private:
    std::map<SyntaxBoundaryType, std::vector<std::pair<size_t, size_t>>> boundaries_;
    
    // 边界处理辅助函数
    bool isBoundaryValid(const std::string& content, size_t start, size_t end) const;
    bool doBoundariesOverlap(const std::pair<size_t, size_t>& boundary1, const std::pair<size_t, size_t>& boundary2) const;
    std::string extractContentRange(const std::string& content, size_t start, size_t end) const;
    std::string replaceContentRange(const std::string& content, size_t start, size_t end, const std::string& newContent) const;
};

/**
 * 语法边界解析器
 */
class SyntaxBoundaryParser {
public:
    SyntaxBoundaryParser();
    ~SyntaxBoundaryParser();
    
    // 解析边界
    std::vector<std::pair<SyntaxBoundaryType, std::pair<size_t, size_t>>> parseBoundaries(const std::string& content) const;
    std::vector<std::pair<SyntaxBoundaryType, std::string>> parseBoundaryContents(const std::string& content) const;
    
    // 解析特定类型边界
    std::vector<std::pair<size_t, size_t>> parseBoundariesByType(const std::string& content, SyntaxBoundaryType type) const;
    std::vector<std::string> parseBoundaryContentsByType(const std::string& content, SyntaxBoundaryType type) const;
    
    // 解析配置
    void setDetector(std::shared_ptr<SyntaxBoundaryDetector> detector);
    std::shared_ptr<SyntaxBoundaryDetector> getDetector() const;
    
private:
    std::shared_ptr<SyntaxBoundaryDetector> detector_;
    
    // 解析辅助函数
    std::vector<std::pair<size_t, size_t>> findPatternBoundaries(const std::string& content, const std::string& startPattern, const std::string& endPattern) const;
    std::string extractBoundaryContent(const std::string& content, size_t start, size_t end) const;
    bool isBoundaryComplete(const std::string& content, size_t start, size_t end) const;
};

/**
 * 语法边界验证器
 */
class SyntaxBoundaryValidator {
public:
    SyntaxBoundaryValidator();
    ~SyntaxBoundaryValidator();
    
    // 验证边界
    bool validateBoundary(const std::string& content, SyntaxBoundaryType type, size_t start, size_t end) const;
    bool validateAllBoundaries(const std::string& content) const;
    
    // 验证语法
    bool validateSyntaxInBoundary(const std::string& content, SyntaxBoundaryType type, size_t start, size_t end) const;
    bool validateSyntaxInAllBoundaries(const std::string& content) const;
    
    // 获取验证错误
    std::vector<std::string> getBoundaryValidationErrors(const std::string& content, SyntaxBoundaryType type, size_t start, size_t end) const;
    std::vector<std::string> getAllBoundaryValidationErrors(const std::string& content) const;
    
    // 配置
    void setDetector(std::shared_ptr<SyntaxBoundaryDetector> detector);
    std::shared_ptr<SyntaxBoundaryDetector> getDetector() const;
    
private:
    std::shared_ptr<SyntaxBoundaryDetector> detector_;
    
    // 验证辅助函数
    bool isSyntaxAllowed(const std::string& content, size_t position, const std::set<std::string>& allowed) const;
    bool isSyntaxForbidden(const std::string& content, size_t position, const std::set<std::string>& forbidden) const;
    bool isPositionInBoundary(size_t position, size_t start, size_t end) const;
    std::string getSyntaxAtPosition(const std::string& content, size_t position) const;
};

} // namespace CHTL
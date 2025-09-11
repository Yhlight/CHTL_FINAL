#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <any>
#include <optional>
#include <set>
#include <queue>
#include <stack>
#include <regex>

namespace CHTL {

// 代码类型枚举
enum class CodeType {
    CHTL,           // CHTL代码
    CHTL_JS,        // CHTL JS代码
    JAVASCRIPT,     // JavaScript代码
    CSS,            // CSS代码
    HTML,           // HTML代码
    UNKNOWN         // 未知类型
};

// 语法边界类型
enum class SyntaxBoundaryType {
    CHTL_START,         // CHTL开始标记
    CHTL_END,           // CHTL结束标记
    CHTL_JS_START,      // CHTL JS开始标记
    CHTL_JS_END,        // CHTL JS结束标记
    JAVASCRIPT_START,   // JavaScript开始标记
    JAVASCRIPT_END,     // JavaScript结束标记
    CSS_START,          // CSS开始标记
    CSS_END,            // CSS结束标记
    HTML_START,         // HTML开始标记
    HTML_END,           // HTML结束标记
    COMMENT_START,      // 注释开始
    COMMENT_END,        // 注释结束
    STRING_START,       // 字符串开始
    STRING_END,         // 字符串结束
    BRACKET_START,      // 括号开始
    BRACKET_END,        // 括号结束
    UNKNOWN             // 未知边界
};

// 代码片段结构
struct CodeFragment {
    std::string content;        // 代码内容
    CodeType type;              // 代码类型
    size_t startPos;            // 开始位置
    size_t endPos;              // 结束位置
    size_t line;                // 行号
    size_t column;              // 列号
    std::string source;         // 源文件
    std::map<std::string, std::any> metadata;  // 元数据
    
    CodeFragment() : type(CodeType::UNKNOWN), startPos(0), endPos(0), line(0), column(0) {}
    
    CodeFragment(const std::string& content, CodeType type, size_t startPos, size_t endPos, 
                 size_t line = 0, size_t column = 0, const std::string& source = "")
        : content(content), type(type), startPos(startPos), endPos(endPos), 
          line(line), column(column), source(source) {}
};

// 语法边界结构
struct SyntaxBoundary {
    SyntaxBoundaryType type;    // 边界类型
    size_t position;            // 位置
    size_t length;              // 长度
    std::string content;        // 内容
    size_t line;                // 行号
    size_t column;              // 列号
    std::map<std::string, std::any> metadata;  // 元数据
    
    SyntaxBoundary() : type(SyntaxBoundaryType::UNKNOWN), position(0), length(0), line(0), column(0) {}
    
    SyntaxBoundary(SyntaxBoundaryType type, size_t position, size_t length, 
                   const std::string& content = "", size_t line = 0, size_t column = 0)
        : type(type), position(position), length(length), content(content), line(line), column(column) {}
};

// 占位符结构
struct Placeholder {
    std::string id;             // 占位符ID
    std::string content;        // 原始内容
    CodeType type;              // 代码类型
    size_t position;            // 位置
    size_t length;              // 长度
    std::map<std::string, std::any> metadata;  // 元数据
    
    Placeholder() : type(CodeType::UNKNOWN), position(0), length(0) {}
    
    Placeholder(const std::string& id, const std::string& content, CodeType type, 
                size_t position, size_t length)
        : id(id), content(content), type(type), position(position), length(length) {}
};

// 扫描结果结构
struct ScanResult {
    std::vector<CodeFragment> fragments;        // 代码片段
    std::vector<SyntaxBoundary> boundaries;     // 语法边界
    std::vector<Placeholder> placeholders;      // 占位符
    std::map<CodeType, std::vector<CodeFragment>> fragmentsByType;  // 按类型分组的片段
    std::string processedCode;                  // 处理后的代码
    std::map<std::string, std::any> metadata;   // 元数据
    bool success;                               // 是否成功
    std::string error;                          // 错误信息
    
    ScanResult() : success(false) {}
};

// 扫描配置结构
struct ScanConfig {
    bool enableCHTL;            // 启用CHTL扫描
    bool enableCHTLJS;          // 启用CHTL JS扫描
    bool enableJavaScript;      // 启用JavaScript扫描
    bool enableCSS;             // 启用CSS扫描
    bool enableHTML;            // 启用HTML扫描
    bool enableComments;        // 启用注释处理
    bool enableStrings;         // 启用字符串处理
    bool enableBrackets;        // 启用括号处理
    bool enablePlaceholders;    // 启用占位符机制
    bool enableBoundaryDetection;  // 启用边界检测
    bool enableWideMode;        // 启用宽泛模式
    bool enableStrictMode;      // 启用严格模式
    bool enableTargetedProcessing;  // 启用针对性处理
    std::set<std::string> customBoundaries;  // 自定义边界
    std::map<std::string, std::any> metadata;  // 元数据
    
    ScanConfig() : enableCHTL(true), enableCHTLJS(true), enableJavaScript(true), 
                   enableCSS(true), enableHTML(true), enableComments(true), 
                   enableStrings(true), enableBrackets(true), enablePlaceholders(true),
                   enableBoundaryDetection(true), enableWideMode(false), 
                   enableStrictMode(true), enableTargetedProcessing(true) {}
};

// 统一扫描器类
class UnifiedScanner {
public:
    // 构造函数和析构函数
    UnifiedScanner();
    ~UnifiedScanner();
    
    // 基本扫描功能
    ScanResult scan(const std::string& code, const ScanConfig& config = ScanConfig());
    ScanResult scanFile(const std::string& filePath, const ScanConfig& config = ScanConfig());
    
    // 代码分离功能
    std::vector<CodeFragment> separateCode(const std::string& code, const ScanConfig& config = ScanConfig());
    std::map<CodeType, std::vector<CodeFragment>> separateCodeByType(const std::string& code, const ScanConfig& config = ScanConfig());
    
    // 语法边界检测
    std::vector<SyntaxBoundary> detectBoundaries(const std::string& code, const ScanConfig& config = ScanConfig());
    std::vector<SyntaxBoundary> detectBoundariesByType(const std::string& code, CodeType type, const ScanConfig& config = ScanConfig());
    
    // 占位符管理
    std::vector<Placeholder> createPlaceholders(const std::string& code, const ScanConfig& config = ScanConfig());
    std::string replacePlaceholders(const std::string& code, const std::vector<Placeholder>& placeholders);
    std::string restorePlaceholders(const std::string& code, const std::vector<Placeholder>& placeholders);
    
    // 变长切片功能
    std::vector<CodeFragment> variableLengthSlicing(const std::string& code, const ScanConfig& config = ScanConfig());
    std::vector<CodeFragment> smartExpansion(const std::string& code, size_t startPos, size_t endPos, const ScanConfig& config = ScanConfig());
    std::vector<CodeFragment> smartRollback(const std::string& code, size_t startPos, size_t endPos, const ScanConfig& config = ScanConfig());
    
    // 智能展开和回滚
    std::vector<CodeFragment> expandFragment(const CodeFragment& fragment, const ScanConfig& config = ScanConfig());
    std::vector<CodeFragment> rollbackFragment(const CodeFragment& fragment, const ScanConfig& config = ScanConfig());
    
    // 语法边界识别
    bool isSyntaxBoundary(const std::string& code, size_t position, const ScanConfig& config = ScanConfig());
    SyntaxBoundaryType getBoundaryType(const std::string& code, size_t position, const ScanConfig& config = ScanConfig());
    std::vector<SyntaxBoundary> findBoundaries(const std::string& code, SyntaxBoundaryType type, const ScanConfig& config = ScanConfig());
    
    // 宽泛和严格判断
    bool isWideMode(const std::string& code, size_t position, const ScanConfig& config = ScanConfig());
    bool isStrictMode(const std::string& code, size_t position, const ScanConfig& config = ScanConfig());
    bool shouldUseWideMode(const std::string& code, size_t position, const ScanConfig& config = ScanConfig());
    bool shouldUseStrictMode(const std::string& code, size_t position, const ScanConfig& config = ScanConfig());
    
    // 针对性处理
    std::vector<CodeFragment> processTargeted(const std::string& code, const std::vector<CodeFragment>& fragments, const ScanConfig& config = ScanConfig());
    std::vector<CodeFragment> processByType(const std::string& code, CodeType type, const ScanConfig& config = ScanConfig());
    std::vector<CodeFragment> processByBoundary(const std::string& code, const std::vector<SyntaxBoundary>& boundaries, const ScanConfig& config = ScanConfig());
    
    // 代码类型检测
    CodeType detectCodeType(const std::string& code, size_t position = 0, const ScanConfig& config = ScanConfig());
    CodeType detectCodeTypeByContent(const std::string& content, const ScanConfig& config = ScanConfig());
    CodeType detectCodeTypeByBoundary(const std::string& code, const SyntaxBoundary& boundary, const ScanConfig& config = ScanConfig());
    
    // 代码验证
    bool validateCode(const std::string& code, const ScanConfig& config = ScanConfig());
    bool validateFragment(const CodeFragment& fragment, const ScanConfig& config = ScanConfig());
    bool validateBoundary(const SyntaxBoundary& boundary, const ScanConfig& config = ScanConfig());
    
    // 代码转换
    std::string convertCode(const std::string& code, CodeType fromType, CodeType toType, const ScanConfig& config = ScanConfig());
    std::string convertFragment(const CodeFragment& fragment, CodeType toType, const ScanConfig& config = ScanConfig());
    std::string convertBoundary(const SyntaxBoundary& boundary, CodeType toType, const ScanConfig& config = ScanConfig());
    
    // 代码合并
    std::string mergeFragments(const std::vector<CodeFragment>& fragments, const ScanConfig& config = ScanConfig());
    std::string mergeByType(const std::map<CodeType, std::vector<CodeFragment>>& fragmentsByType, const ScanConfig& config = ScanConfig());
    std::string mergeWithBoundaries(const std::vector<CodeFragment>& fragments, const std::vector<SyntaxBoundary>& boundaries, const ScanConfig& config = ScanConfig());
    
    // 代码格式化
    std::string formatCode(const std::string& code, const ScanConfig& config = ScanConfig());
    std::string formatFragment(const CodeFragment& fragment, const ScanConfig& config = ScanConfig());
    std::string formatBoundary(const SyntaxBoundary& boundary, const ScanConfig& config = ScanConfig());
    
    // 代码压缩
    std::string minifyCode(const std::string& code, const ScanConfig& config = ScanConfig());
    std::string minifyFragment(const CodeFragment& fragment, const ScanConfig& config = ScanConfig());
    std::string minifyBoundary(const SyntaxBoundary& boundary, const ScanConfig& config = ScanConfig());
    
    // 代码美化
    std::string beautifyCode(const std::string& code, const ScanConfig& config = ScanConfig());
    std::string beautifyFragment(const CodeFragment& fragment, const ScanConfig& config = ScanConfig());
    std::string beautifyBoundary(const SyntaxBoundary& boundary, const ScanConfig& config = ScanConfig());
    
    // 统计信息
    std::map<std::string, int> getStatistics(const std::string& code, const ScanConfig& config = ScanConfig());
    std::map<std::string, int> getFragmentStatistics(const std::vector<CodeFragment>& fragments, const ScanConfig& config = ScanConfig());
    std::map<std::string, int> getBoundaryStatistics(const std::vector<SyntaxBoundary>& boundaries, const ScanConfig& config = ScanConfig());
    
    // 调试功能
    std::string debugCode(const std::string& code, const ScanConfig& config = ScanConfig());
    std::string debugFragment(const CodeFragment& fragment, const ScanConfig& config = ScanConfig());
    std::string debugBoundary(const SyntaxBoundary& boundary, const ScanConfig& config = ScanConfig());
    std::string debugScanResult(const ScanResult& result, const ScanConfig& config = ScanConfig());
    
    // 配置管理
    void setConfig(const ScanConfig& config);
    ScanConfig getConfig() const;
    void resetConfig();
    
    // 状态管理
    void reset();
    void clear();
    bool isEmpty() const;
    size_t getFragmentCount() const;
    size_t getBoundaryCount() const;
    size_t getPlaceholderCount() const;
    
    // 错误处理
    std::string getLastError() const;
    void clearError();
    bool hasError() const;
    
    // 工具函数
    static std::string codeTypeToString(CodeType type);
    static CodeType stringToCodeType(const std::string& type);
    static std::string boundaryTypeToString(SyntaxBoundaryType type);
    static SyntaxBoundaryType stringToBoundaryType(const std::string& type);
    static bool isValidCodeType(CodeType type);
    static bool isValidBoundaryType(SyntaxBoundaryType type);
    
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
    std::unique_ptr<UnifiedScanner> clone() const;
    
    // 比较
    bool equals(const UnifiedScanner& other) const;
    bool equals(const std::string& code) const;
    
    // 格式化
    std::string format() const;
    std::string minify() const;
    std::string beautify() const;

private:
    // 内部状态
    ScanConfig m_config;
    std::vector<CodeFragment> m_fragments;
    std::vector<SyntaxBoundary> m_boundaries;
    std::vector<Placeholder> m_placeholders;
    std::string m_lastError;
    bool m_hasError;
    
    // 内部方法
    void initialize();
    void cleanup();
    bool validateInput(const std::string& code) const;
    bool validateConfig(const ScanConfig& config) const;
    
    // 边界检测内部方法
    std::vector<SyntaxBoundary> detectCHTLBoundaries(const std::string& code);
    std::vector<SyntaxBoundary> detectCHTLJSBoundaries(const std::string& code);
    std::vector<SyntaxBoundary> detectJavaScriptBoundaries(const std::string& code);
    std::vector<SyntaxBoundary> detectCSSBoundaries(const std::string& code);
    std::vector<SyntaxBoundary> detectHTMLBoundaries(const std::string& code);
    std::vector<SyntaxBoundary> detectCommentBoundaries(const std::string& code);
    std::vector<SyntaxBoundary> detectStringBoundaries(const std::string& code);
    std::vector<SyntaxBoundary> detectBracketBoundaries(const std::string& code);
    
    // 代码分离内部方法
    std::vector<CodeFragment> separateCHTLCode(const std::string& code);
    std::vector<CodeFragment> separateCHTLJSCode(const std::string& code);
    std::vector<CodeFragment> separateJavaScriptCode(const std::string& code);
    std::vector<CodeFragment> separateCSSCode(const std::string& code);
    std::vector<CodeFragment> separateHTMLCode(const std::string& code);
    
    // 占位符管理内部方法
    std::string generatePlaceholderId(size_t index, CodeType type);
    std::string createPlaceholder(const std::string& content, CodeType type, size_t position);
    std::string restorePlaceholder(const std::string& placeholderId, const std::vector<Placeholder>& placeholders);
    
    // 变长切片内部方法
    std::vector<CodeFragment> sliceByBoundaries(const std::string& code, const std::vector<SyntaxBoundary>& boundaries);
    std::vector<CodeFragment> sliceByType(const std::string& code, CodeType type);
    std::vector<CodeFragment> sliceByPosition(const std::string& code, size_t startPos, size_t endPos);
    
    // 智能展开和回滚内部方法
    std::vector<CodeFragment> expandByBoundaries(const CodeFragment& fragment, const std::vector<SyntaxBoundary>& boundaries);
    std::vector<CodeFragment> expandByType(const CodeFragment& fragment, CodeType type);
    std::vector<CodeFragment> rollbackByBoundaries(const CodeFragment& fragment, const std::vector<SyntaxBoundary>& boundaries);
    std::vector<CodeFragment> rollbackByType(const CodeFragment& fragment, CodeType type);
    
    // 语法边界识别内部方法
    bool isCHTLBoundary(const std::string& code, size_t position);
    bool isCHTLJSBoundary(const std::string& code, size_t position);
    bool isJavaScriptBoundary(const std::string& code, size_t position);
    bool isCSSBoundary(const std::string& code, size_t position);
    bool isHTMLBoundary(const std::string& code, size_t position);
    bool isCommentBoundary(const std::string& code, size_t position);
    bool isStringBoundary(const std::string& code, size_t position);
    bool isBracketBoundary(const std::string& code, size_t position);
    
    // 宽泛和严格判断内部方法
    bool isWideModeCHTL(const std::string& code, size_t position);
    bool isWideModeCHTLJS(const std::string& code, size_t position);
    bool isWideModeJavaScript(const std::string& code, size_t position);
    bool isWideModeCSS(const std::string& code, size_t position);
    bool isWideModeHTML(const std::string& code, size_t position);
    
    bool isStrictModeCHTL(const std::string& code, size_t position);
    bool isStrictModeCHTLJS(const std::string& code, size_t position);
    bool isStrictModeJavaScript(const std::string& code, size_t position);
    bool isStrictModeCSS(const std::string& code, size_t position);
    bool isStrictModeHTML(const std::string& code, size_t position);
    
    // 针对性处理内部方法
    std::vector<CodeFragment> processCHTL(const std::string& code, const std::vector<CodeFragment>& fragments);
    std::vector<CodeFragment> processCHTLJS(const std::string& code, const std::vector<CodeFragment>& fragments);
    std::vector<CodeFragment> processJavaScript(const std::string& code, const std::vector<CodeFragment>& fragments);
    std::vector<CodeFragment> processCSS(const std::string& code, const std::vector<CodeFragment>& fragments);
    std::vector<CodeFragment> processHTML(const std::string& code, const std::vector<CodeFragment>& fragments);
    
    // 代码类型检测内部方法
    CodeType detectCHTLType(const std::string& code, size_t position);
    CodeType detectCHTLJSType(const std::string& code, size_t position);
    CodeType detectJavaScriptType(const std::string& code, size_t position);
    CodeType detectCSSType(const std::string& code, size_t position);
    CodeType detectHTMLType(const std::string& code, size_t position);
    
    // 代码验证内部方法
    bool validateCHTLCode(const std::string& code);
    bool validateCHTLJSCode(const std::string& code);
    bool validateJavaScriptCode(const std::string& code);
    bool validateCSSCode(const std::string& code);
    bool validateHTMLCode(const std::string& code);
    
    // 代码转换内部方法
    std::string convertCHTLToHTML(const std::string& code);
    std::string convertCHTLJSToJavaScript(const std::string& code);
    std::string convertJavaScriptToCHTLJS(const std::string& code);
    std::string convertCSSToCHTL(const std::string& code);
    std::string convertHTMLToCHTL(const std::string& code);
    
    // 代码合并内部方法
    std::string mergeCHTLFragments(const std::vector<CodeFragment>& fragments);
    std::string mergeCHTLJSFragments(const std::vector<CodeFragment>& fragments);
    std::string mergeJavaScriptFragments(const std::vector<CodeFragment>& fragments);
    std::string mergeCSSFragments(const std::vector<CodeFragment>& fragments);
    std::string mergeHTMLFragments(const std::vector<CodeFragment>& fragments);
    
    // 代码格式化内部方法
    std::string formatCHTLCode(const std::string& code);
    std::string formatCHTLJSCode(const std::string& code);
    std::string formatJavaScriptCode(const std::string& code);
    std::string formatCSSCode(const std::string& code);
    std::string formatHTMLCode(const std::string& code);
    
    // 代码压缩内部方法
    std::string minifyCHTLCode(const std::string& code);
    std::string minifyCHTLJSCode(const std::string& code);
    std::string minifyJavaScriptCode(const std::string& code);
    std::string minifyCSSCode(const std::string& code);
    std::string minifyHTMLCode(const std::string& code);
    
    // 代码美化内部方法
    std::string beautifyCHTLCode(const std::string& code);
    std::string beautifyCHTLJSCode(const std::string& code);
    std::string beautifyJavaScriptCode(const std::string& code);
    std::string beautifyCSSCode(const std::string& code);
    std::string beautifyHTMLCode(const std::string& code);
    
    // 统计信息内部方法
    std::map<std::string, int> getCHTLStatistics(const std::string& code);
    std::map<std::string, int> getCHTLJSStatistics(const std::string& code);
    std::map<std::string, int> getJavaScriptStatistics(const std::string& code);
    std::map<std::string, int> getCSSStatistics(const std::string& code);
    std::map<std::string, int> getHTMLStatistics(const std::string& code);
    
    // 调试功能内部方法
    std::string debugCHTLCode(const std::string& code);
    std::string debugCHTLJSCode(const std::string& code);
    std::string debugJavaScriptCode(const std::string& code);
    std::string debugCSSCode(const std::string& code);
    std::string debugHTMLCode(const std::string& code);
    
    // 工具函数内部方法
    std::string getLineContent(const std::string& code, size_t line) const;
    size_t getLineNumber(const std::string& code, size_t position) const;
    size_t getColumnNumber(const std::string& code, size_t position) const;
    std::string getPositionInfo(const std::string& code, size_t position) const;
    
    // 正则表达式模式
    std::regex m_chtlPattern;
    std::regex m_chtljsPattern;
    std::regex m_javascriptPattern;
    std::regex m_cssPattern;
    std::regex m_htmlPattern;
    std::regex m_commentPattern;
    std::regex m_stringPattern;
    std::regex m_bracketPattern;
    
    // 初始化正则表达式
    void initializePatterns();
    void cleanupPatterns();
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H
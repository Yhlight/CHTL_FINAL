#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief 代码片段类型
 */
enum class CodeFragmentType {
    CHTL,       // CHTL代码
    CHTL_JS,    // CHTL JS代码
    CSS,        // CSS代码
    JS,         // JavaScript代码
    HTML,       // HTML代码
    UNKNOWN     // 未知类型
};

/**
 * @brief 代码片段结构
 */
struct CodeFragment {
    CodeFragmentType type;
    std::string content;
    size_t startLine;
    size_t endLine;
    size_t startColumn;
    size_t endColumn;
    
    CodeFragment(CodeFragmentType t, const std::string& c, size_t sl, size_t el, size_t sc, size_t ec)
        : type(t), content(c), startLine(sl), endLine(el), startColumn(sc), endColumn(ec) {}
};

/**
 * @brief CHTL统一扫描器
 * 
 * 负责将混合的CHTL源代码分离为不同类型的代码片段
 */
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner() = default;

    /**
     * @brief 扫描源代码
     * @param source 源代码字符串
     * @return 代码片段列表
     */
    std::vector<CodeFragment> scan(const std::string& source);

    /**
     * @brief 从文件扫描
     * @param filename 文件名
     * @return 代码片段列表
     */
    std::vector<CodeFragment> scanFile(const std::string& filename);

    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);

    /**
     * @brief 获取错误信息
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const { return m_errors; }

    /**
     * @brief 清空错误信息
     */
    void clearErrors() { m_errors.clear(); }

private:
    std::string m_source;
    size_t m_position;
    size_t m_line;
    size_t m_column;
    bool m_debugMode;
    std::vector<std::string> m_errors;

    // 扫描方法
    CodeFragment scanCHTL();
    CodeFragment scanCHTLJS();
    CodeFragment scanCSS();
    CodeFragment scanJS();
    CodeFragment scanHTML();

    // 辅助方法
    char peek() const;
    char peek(size_t offset) const;
    char consume();
    void consume(size_t count);
    bool isAtEnd() const;
    void skipWhitespace();
    bool skipComment();
    bool skipSingleLineComment();
    bool skipMultiLineComment();
    bool skipGeneratorComment();
    bool isCHTLKeyword(const std::string& word);
    bool isCHTLJSKeyword(const std::string& word);
    bool isCSSKeyword(const std::string& word);
    bool isJSKeyword(const std::string& word);
    bool isHTMLTag(const std::string& word);
    void addError(const std::string& message);
};

} // namespace CHTL
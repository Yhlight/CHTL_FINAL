#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace CHTL {

enum class FragmentType {
    UNKNOWN,
    CHTL,
    CHTL_JS,
    CSS,
    HTML,
    JAVASCRIPT,
    TEXT
};

struct CodeFragment {
    FragmentType type;
    std::string content;
    int line;
    int column;
    int position;
    
    CodeFragment() : type(FragmentType::UNKNOWN), line(0), column(0), position(0) {}
    CodeFragment(FragmentType t, const std::string& c, int l, int col, int pos) 
        : type(t), content(c), line(l), column(col), position(pos) {}
};

class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner();
    
    void setInput(const std::string& input);
    void setInput(std::istream& input);
    void setInput(const std::string& file_path, bool is_file = true);
    
    std::vector<CodeFragment> scan();
    CodeFragment scanNextFragment();
    
    bool hasMoreTokens() const;
    char currentChar() const;
    char peekChar() const;
    void advance();
    void skipWhitespace();
    
    bool isCHTLStart(char c) const;
    bool isCHTLEnd(char c) const;
    bool isCHTLJSStart(char c) const;
    bool isCHTLJSEnd(char c) const;
    bool isCSSStart(char c) const;
    bool isCSSEnd(char c) const;
    bool isHTMLStart(char c) const;
    bool isHTMLEnd(char c) const;
    bool isJavaScriptStart(char c) const;
    bool isJavaScriptEnd(char c) const;
    
    void setDebugMode(bool enabled);
    bool isDebugMode() const;
    
    void reportError(const std::string& message);
    void clearErrors();
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;
    
    void reset();
    std::string getCurrentFilePath() const;
    int getCurrentLine() const;
    int getCurrentColumn() const;
    int getCurrentPosition() const;
    
    std::map<std::string, std::vector<std::string>> getBoundaries() const;
    void setBoundaries(const std::map<std::string, std::vector<std::string>>& boundaries);

private:
    std::string input_;
    std::string file_path_;
    int position_;
    int line_;
    int column_;
    bool debug_mode_;
    std::vector<std::string> errors_;
    std::map<std::string, std::vector<std::string>> boundaries_;
    
    void initializeBoundaries();
    
    CodeFragment scanCHTLFragment();
    CodeFragment scanCHTLJSFragment();
    CodeFragment scanCSSFragment();
    CodeFragment scanHTMLFragment();
    CodeFragment scanJavaScriptFragment();
    CodeFragment scanTextFragment();
};

} // namespace CHTL
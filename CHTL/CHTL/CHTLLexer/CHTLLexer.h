#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "CHTLToken.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

class CHTLLexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    size_t start;
    
    std::unordered_map<std::string, CHTLTokenType> keywords;
    std::unordered_map<std::string, CHTLTokenType> operators;
    std::unordered_map<std::string, CHTLTokenType> htmlElements;
    
    void initializeKeywords();
    void initializeOperators();
    void initializeHTMLElements();
    
    char current() const;
    char peek(size_t offset = 1) const;
    void advance();
    bool isAtEnd() const;
    
    void skipWhitespace();
    void skipComment();
    void skipLineComment();
    void skipBlockComment();
    void skipGeneratorComment();
    
    CHTLToken scanString();
    CHTLToken scanNumber();
    CHTLToken scanIdentifier();
    CHTLToken scanOperator();
    CHTLToken scanHTMLElement();
    CHTLToken scanTemplate();
    CHTLToken scanCustom();
    CHTLToken scanOrigin();
    CHTLToken scanImport();
    CHTLToken scanNamespace();
    CHTLToken scanConfiguration();
    CHTLToken scanInfo();
    CHTLToken scanExport();
    CHTLToken scanGeneratorComment();
    
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isHTMLChar(char c) const;
    
    CHTLToken makeToken(CHTLTokenType type, const std::string& value = "");
    CHTLToken makeToken(CHTLTokenType type, char value);
    
    void error(const std::string& message) const;
    
public:
    CHTLLexer(const std::string& source);
    ~CHTLLexer() = default;
    
    CHTLTokenList tokenize();
    CHTLTokenList tokenize(const std::string& source);
    
    void reset();
    void setSource(const std::string& source);
    
    std::string getSource() const;
    size_t getPosition() const;
    size_t getLine() const;
    size_t getColumn() const;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
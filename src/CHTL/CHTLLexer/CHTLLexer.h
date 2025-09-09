#pragma once

#include <string>
#include <vector>
#include <memory>
#include "CHTL.h"

namespace CHTL {

class CHTLLexer {
private:
    std::string input;
    size_t position;
    size_t line;
    size_t column;
    std::vector<Token> tokens;
    std::map<std::string, std::vector<std::string>> keywordMappings;
    
    // Character classification
    bool isWhitespace(char c) const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isQuote(char c) const;
    bool isOperator(char c) const;
    bool isBracket(char c) const;
    
    // Tokenization helpers
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    Token parseString(char quote);
    Token parseUnquotedLiteral();
    Token parseNumber();
    Token parseIdentifier();
    Token parseOperator();
    Token parseBracket();
    Token parseKeyword();
    
    // Keyword mapping
    void loadKeywordMappings();
    std::string mapKeyword(const std::string& keyword) const;
    
    // Error handling
    void addError(const std::string& message);
    std::vector<std::string> errors;

public:
    CHTLLexer();
    ~CHTLLexer();
    
    // Main lexing interface
    std::vector<Token> tokenize(const std::string& input);
    void setKeywordMappings(const std::map<std::string, std::vector<std::string>>& mappings);
    
    // Error handling
    const std::vector<std::string>& getErrors() const;
    void clearErrors();
    
    // Debug
    void printTokens() const;
};

} // namespace CHTL
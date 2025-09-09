#include "CHTLLexer.h"
#include <iostream>
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer() : position(0), line(1), column(1) {
    loadKeywordMappings();
}

CHTLLexer::~CHTLLexer() = default;

std::vector<Token> CHTLLexer::tokenize(const std::string& input) {
    this->input = input;
    position = 0;
    line = 1;
    column = 1;
    tokens.clear();
    errors.clear();
    
    while (position < input.length()) {
        skipWhitespace();
        
        if (position >= input.length()) break;
        
        char current = input[position];
        
        // Comments
        if (current == '/' && position + 1 < input.length()) {
            char next = input[position + 1];
            if (next == '/') {
                skipLineComment();
                continue;
            } else if (next == '*') {
                skipBlockComment();
                continue;
            }
        }
        
        // Generator comments
        if (current == '-' && position + 1 < input.length() && input[position + 1] == '-') {
            position += 2;
            column += 2;
            std::string comment = "--";
            while (position < input.length() && input[position] != '\n') {
                comment += input[position];
                position++;
                column++;
            }
            tokens.emplace_back(TokenType::GENERATOR_COMMENT, comment, line, column - comment.length());
            continue;
        }
        
        // Strings
        if (isQuote(current)) {
            tokens.push_back(parseString(current));
            continue;
        }
        
        // Numbers
        if (isDigit(current)) {
            tokens.push_back(parseNumber());
            continue;
        }
        
        // Identifiers and keywords
        if (isAlpha(current) || current == '_') {
            tokens.push_back(parseIdentifier());
            continue;
        }
        
        // Operators
        if (isOperator(current)) {
            tokens.push_back(parseOperator());
            continue;
        }
        
        // Brackets
        if (isBracket(current)) {
            tokens.push_back(parseBracket());
            continue;
        }
        
        // Unknown character
        addError("Unknown character: " + std::string(1, current) + " at line " + std::to_string(line) + ", column " + std::to_string(column));
        position++;
        column++;
    }
    
    // Add EOF token
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line, column);
    
    return tokens;
}

void CHTLLexer::setKeywordMappings(const std::map<std::string, std::vector<std::string>>& mappings) {
    keywordMappings = mappings;
}

const std::vector<std::string>& CHTLLexer::getErrors() const {
    return errors;
}

void CHTLLexer::clearErrors() {
    errors.clear();
}

void CHTLLexer::printTokens() const {
    for (const auto& token : tokens) {
        std::cout << "Token: " << static_cast<int>(token.type) << " = '" << token.value << "' at " << token.line << ":" << token.column << std::endl;
    }
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return std::isalnum(c) || c == '_';
}

bool CHTLLexer::isQuote(char c) const {
    return c == '\'' || c == '"';
}

bool CHTLLexer::isOperator(char c) const {
    return c == ':' || c == '=' || c == ';' || c == ',' || c == '.' || c == '#' || 
           c == '?' || c == '!' || c == '&' || c == '|' || c == '+' || c == '-' || 
           c == '*' || c == '/' || c == '%' || c == '<' || c == '>';
}

bool CHTLLexer::isBracket(char c) const {
    return c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')';
}

void CHTLLexer::skipWhitespace() {
    while (position < input.length() && isWhitespace(input[position])) {
        if (input[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void CHTLLexer::skipLineComment() {
    position += 2; // Skip //
    column += 2;
    
    std::string comment = "//";
    while (position < input.length() && input[position] != '\n') {
        comment += input[position];
        position++;
        column++;
    }
    
    tokens.emplace_back(TokenType::LINE_COMMENT, comment, line, column - comment.length());
}

void CHTLLexer::skipBlockComment() {
    position += 2; // Skip /*
    column += 2;
    
    std::string comment = "/*";
    while (position < input.length()) {
        if (input[position] == '*' && position + 1 < input.length() && input[position + 1] == '/') {
            comment += "*/";
            position += 2;
            column += 2;
            break;
        }
        comment += input[position];
        if (input[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
    
    tokens.emplace_back(TokenType::BLOCK_COMMENT, comment, line, column - comment.length());
}

Token CHTLLexer::parseString(char quote) {
    size_t startLine = line;
    size_t startColumn = column;
    
    position++; // Skip opening quote
    column++;
    
    std::string value;
    while (position < input.length() && input[position] != quote) {
        if (input[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        value += input[position];
        position++;
    }
    
    if (position < input.length()) {
        position++; // Skip closing quote
        column++;
    } else {
        addError("Unterminated string literal at line " + std::to_string(startLine) + ", column " + std::to_string(startColumn));
    }
    
    TokenType type = (quote == '\'') ? TokenType::SINGLE_QUOTED : TokenType::DOUBLE_QUOTED;
    return Token(type, value, startLine, startColumn);
}

Token CHTLLexer::parseUnquotedLiteral() {
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    while (position < input.length() && 
           !isWhitespace(input[position]) && 
           !isOperator(input[position]) && 
           !isBracket(input[position]) &&
           input[position] != ';') {
        value += input[position];
        position++;
        column++;
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startLine, startColumn);
}

Token CHTLLexer::parseNumber() {
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    while (position < input.length() && (isDigit(input[position]) || input[position] == '.')) {
        value += input[position];
        position++;
        column++;
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startLine, startColumn);
}

Token CHTLLexer::parseIdentifier() {
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    while (position < input.length() && isAlphaNumeric(input[position])) {
        value += input[position];
        position++;
        column++;
    }
    
    // Check if it's a keyword
    std::string mappedKeyword = mapKeyword(value);
    if (mappedKeyword != value) {
        // It's a keyword
        if (mappedKeyword == "text") return Token(TokenType::TEXT, value, startLine, startColumn);
        if (mappedKeyword == "style") return Token(TokenType::STYLE, value, startLine, startColumn);
        if (mappedKeyword == "script") return Token(TokenType::SCRIPT, value, startLine, startColumn);
        if (mappedKeyword == "template") return Token(TokenType::TEMPLATE, value, startLine, startColumn);
        if (mappedKeyword == "custom") return Token(TokenType::CUSTOM, value, startLine, startColumn);
        if (mappedKeyword == "origin") return Token(TokenType::ORIGIN, value, startLine, startColumn);
        if (mappedKeyword == "import") return Token(TokenType::IMPORT, value, startLine, startColumn);
        if (mappedKeyword == "namespace") return Token(TokenType::NAMESPACE, value, startLine, startColumn);
        if (mappedKeyword == "configuration") return Token(TokenType::CONFIGURATION, value, startLine, startColumn);
        if (mappedKeyword == "except") return Token(TokenType::EXCEPT, value, startLine, startColumn);
        if (mappedKeyword == "inherit") return Token(TokenType::INHERIT, value, startLine, startColumn);
        if (mappedKeyword == "delete") return Token(TokenType::DELETE, value, startLine, startColumn);
        if (mappedKeyword == "insert") return Token(TokenType::INSERT, value, startLine, startColumn);
        if (mappedKeyword == "after") return Token(TokenType::AFTER, value, startLine, startColumn);
        if (mappedKeyword == "before") return Token(TokenType::BEFORE, value, startLine, startColumn);
        if (mappedKeyword == "replace") return Token(TokenType::REPLACE, value, startLine, startColumn);
        if (mappedKeyword == "at top") return Token(TokenType::AT_TOP, value, startLine, startColumn);
        if (mappedKeyword == "at bottom") return Token(TokenType::AT_BOTTOM, value, startLine, startColumn);
        if (mappedKeyword == "from") return Token(TokenType::FROM, value, startLine, startColumn);
        if (mappedKeyword == "as") return Token(TokenType::AS, value, startLine, startColumn);
        if (mappedKeyword == "use") return Token(TokenType::USE, value, startLine, startColumn);
        if (mappedKeyword == "html5") return Token(TokenType::HTML5, value, startLine, startColumn);
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn);
}

Token CHTLLexer::parseOperator() {
    size_t startLine = line;
    size_t startColumn = column;
    
    char current = input[position];
    position++;
    column++;
    
    // Check for multi-character operators
    if (position < input.length()) {
        char next = input[position];
        if (current == '=' && next == '=') {
            position++;
            column++;
            return Token(TokenType::EQUAL_EQUAL, "==", startLine, startColumn);
        }
        if (current == '!' && next == '=') {
            position++;
            column++;
            return Token(TokenType::NOT_EQUAL, "!=", startLine, startColumn);
        }
        if (current == '&' && next == '&') {
            position++;
            column++;
            return Token(TokenType::AND_AND, "&&", startLine, startColumn);
        }
        if (current == '|' && next == '|') {
            position++;
            column++;
            return Token(TokenType::OR_OR, "||", startLine, startColumn);
        }
        if (current == '<' && next == '=') {
            position++;
            column++;
            return Token(TokenType::LESS_EQUAL, "<=", startLine, startColumn);
        }
        if (current == '>' && next == '=') {
            position++;
            column++;
            return Token(TokenType::GREATER_EQUAL, ">=", startLine, startColumn);
        }
    }
    
    // Single character operators
    switch (current) {
        case ':': return Token(TokenType::COLON, ":", startLine, startColumn);
        case '=': return Token(TokenType::EQUALS, "=", startLine, startColumn);
        case ';': return Token(TokenType::SEMICOLON, ";", startLine, startColumn);
        case ',': return Token(TokenType::COMMA, ",", startLine, startColumn);
        case '.': return Token(TokenType::DOT, ".", startLine, startColumn);
        case '#': return Token(TokenType::HASH, "#", startLine, startColumn);
        case '?': return Token(TokenType::QUESTION, "?", startLine, startColumn);
        case '!': return Token(TokenType::EXCLAMATION, "!", startLine, startColumn);
        case '&': return Token(TokenType::AMPERSAND, "&", startLine, startColumn);
        case '|': return Token(TokenType::PIPE, "|", startLine, startColumn);
        case '+': return Token(TokenType::PLUS, "+", startLine, startColumn);
        case '-': return Token(TokenType::MINUS, "-", startLine, startColumn);
        case '*': return Token(TokenType::MULTIPLY, "*", startLine, startColumn);
        case '/': return Token(TokenType::DIVIDE, "/", startLine, startColumn);
        case '%': return Token(TokenType::MODULO, "%", startLine, startColumn);
        case '<': return Token(TokenType::LESS, "<", startLine, startColumn);
        case '>': return Token(TokenType::GREATER, ">", startLine, startColumn);
        default: return Token(TokenType::IDENTIFIER, std::string(1, current), startLine, startColumn);
    }
}

Token CHTLLexer::parseBracket() {
    size_t startLine = line;
    size_t startColumn = column;
    
    char current = input[position];
    position++;
    column++;
    
    switch (current) {
        case '{': return Token(TokenType::LEFT_BRACE, "{", startLine, startColumn);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", startLine, startColumn);
        case '[': return Token(TokenType::LEFT_BRACKET, "[", startLine, startColumn);
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", startLine, startColumn);
        case '(': return Token(TokenType::LEFT_PAREN, "(", startLine, startColumn);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", startLine, startColumn);
        default: return Token(TokenType::IDENTIFIER, std::string(1, current), startLine, startColumn);
    }
}

Token CHTLLexer::parseKeyword() {
    // This is handled in parseIdentifier
    return Token(TokenType::IDENTIFIER, "", line, column);
}

void CHTLLexer::loadKeywordMappings() {
    // Default keyword mappings (would be loaded from configuration in real implementation)
    keywordMappings["CUSTOM_STYLE"] = {"@Style", "@style", "@CSS", "@Css", "@css"};
    keywordMappings["CUSTOM_ELEMENT"] = {"@Element"};
    keywordMappings["CUSTOM_VAR"] = {"@Var"};
    keywordMappings["TEMPLATE_STYLE"] = {"@Style"};
    keywordMappings["TEMPLATE_ELEMENT"] = {"@Element"};
    keywordMappings["TEMPLATE_VAR"] = {"@Var"};
    keywordMappings["ORIGIN_HTML"] = {"@Html"};
    keywordMappings["ORIGIN_STYLE"] = {"@Style"};
    keywordMappings["ORIGIN_JAVASCRIPT"] = {"@JavaScript"};
    keywordMappings["IMPORT_HTML"] = {"@Html"};
    keywordMappings["IMPORT_STYLE"] = {"@Style"};
    keywordMappings["IMPORT_JAVASCRIPT"] = {"@JavaScript"};
    keywordMappings["IMPORT_CHTL"] = {"@Chtl"};
    keywordMappings["IMPORT_CRMOD"] = {"@CJmod"};
    keywordMappings["KEYWORD_INHERIT"] = {"inherit"};
    keywordMappings["KEYWORD_DELETE"] = {"delete"};
    keywordMappings["KEYWORD_INSERT"] = {"insert"};
    keywordMappings["KEYWORD_AFTER"] = {"after"};
    keywordMappings["KEYWORD_BEFORE"] = {"before"};
    keywordMappings["KEYWORD_REPLACE"] = {"replace"};
    keywordMappings["KEYWORD_ATTOP"] = {"at top"};
    keywordMappings["KEYWORD_ATBOTTOM"] = {"at bottom"};
    keywordMappings["KEYWORD_FROM"] = {"from"};
    keywordMappings["KEYWORD_AS"] = {"as"};
    keywordMappings["KEYWORD_EXCEPT"] = {"except"};
    keywordMappings["KEYWORD_USE"] = {"use"};
    keywordMappings["KEYWORD_HTML5"] = {"html5"};
    keywordMappings["KEYWORD_TEXT"] = {"text"};
    keywordMappings["KEYWORD_STYLE"] = {"style"};
    keywordMappings["KEYWORD_SCRIPT"] = {"script"};
    keywordMappings["KEYWORD_CUSTOM"] = {"[Custom]"};
    keywordMappings["KEYWORD_TEMPLATE"] = {"[Template]"};
    keywordMappings["KEYWORD_ORIGIN"] = {"[Origin]"};
    keywordMappings["KEYWORD_IMPORT"] = {"[Import]"};
    keywordMappings["KEYWORD_NAMESPACE"] = {"[Namespace]"};
}

std::string CHTLLexer::mapKeyword(const std::string& keyword) const {
    // Simple keyword mapping (would be more sophisticated in real implementation)
    for (const auto& pair : keywordMappings) {
        for (const auto& option : pair.second) {
            if (option == keyword) {
                return keyword; // Return the keyword as-is for now
            }
        }
    }
    return keyword;
}

void CHTLLexer::addError(const std::string& message) {
    errors.push_back(message);
}

} // namespace CHTL
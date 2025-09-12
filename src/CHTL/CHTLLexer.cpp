#include "CHTL/CHTLLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLLexer::CHTLLexer() 
    : current_pos_(0), current_line_(1), current_column_(1), debug_mode_(false) {
    
    // 初始化关键字映射
    keywords_ = {
        {"[Template]", TokenType::TEMPLATE},
        {"[Custom]", TokenType::CUSTOM},
        {"[Origin]", TokenType::ORIGIN},
        {"[Import]", TokenType::IMPORT},
        {"[Namespace]", TokenType::NAMESPACE},
        {"[Configuration]", TokenType::CONFIGURATION},
        {"[Info]", TokenType::INFO},
        {"[Export]", TokenType::EXPORT},
        {"use", TokenType::USE},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"text", TokenType::TEXT},
        {"@Style", TokenType::STYLE_TEMPLATE},
        {"@Element", TokenType::ELEMENT_TEMPLATE},
        {"@Var", TokenType::VAR_TEMPLATE},
        {"@Html", TokenType::HTML_ORIGIN},
        {"@JavaScript", TokenType::JS_ORIGIN},
        {"@Chtl", TokenType::CHTL_IMPORT},
        {"@CJmod", TokenType::CJMOD_IMPORT},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        {"html5", TokenType::HTML5}
    };
    
    // 初始化操作符映射
    operators_ = {
        {":", TokenType::COLON},
        {"=", TokenType::EQUAL},
        {";", TokenType::SEMICOLON},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"/", TokenType::SLASH},
        {"\\", TokenType::BACKSLASH},
        {"{", TokenType::LBRACE},
        {"}", TokenType::RBRACE},
        {"[", TokenType::LBRACKET},
        {"]", TokenType::RBRACKET},
        {"(", TokenType::LPAREN},
        {")", TokenType::RPAREN},
        {">", TokenType::GT},
        {"<", TokenType::LT},
        {">=", TokenType::GE},
        {"<=", TokenType::LE},
        {"==", TokenType::EQ},
        {"!=", TokenType::NE},
        {"&&", TokenType::AND},
        {"||", TokenType::OR},
        {"!", TokenType::NOT},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::MULTIPLY},
        {"/", TokenType::DIVIDE},
        {"%", TokenType::MODULO},
        {"**", TokenType::POWER},
        {"?", TokenType::QUESTION},
        {"&", TokenType::AMPERSAND},
        {"|", TokenType::PIPE},
        {"~", TokenType::TILDE},
        {"^", TokenType::CARET},
        {"$", TokenType::DOLLAR},
        {"@", TokenType::AT},
        {"#", TokenType::HASH}
    };
}

std::vector<Token> CHTLLexer::tokenize(const std::string& source_code) {
    source_code_ = source_code;
    current_pos_ = 0;
    current_line_ = 1;
    current_column_ = 1;
    
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) break;
        
        char ch = peek();
        
        if (isAlpha(ch) || ch == '_') {
            tokens.push_back(readIdentifier());
        } else if (ch == '"' || ch == '\'') {
            tokens.push_back(readString());
        } else if (isDigit(ch)) {
            tokens.push_back(readNumber());
        } else if (isOperator(ch)) {
            tokens.push_back(readOperator());
        } else if (ch == '\n') {
            tokens.push_back(Token(TokenType::NEWLINE, "\n", current_line_, current_column_, current_pos_));
            consume();
            current_line_++;
            current_column_ = 1;
        } else {
            // 未知字符，跳过
            consume();
        }
    }
    
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", current_line_, current_column_, current_pos_));
    return tokens;
}

void CHTLLexer::reset() {
    current_pos_ = 0;
    current_line_ = 1;
    current_column_ = 1;
    source_code_.clear();
}

void CHTLLexer::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
}

size_t CHTLLexer::getCurrentLine() const {
    return current_line_;
}

size_t CHTLLexer::getCurrentColumn() const {
    return current_column_;
}

size_t CHTLLexer::getCurrentPosition() const {
    return current_pos_;
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(peek()) && peek() != '\n') {
        consume();
    }
}

bool CHTLLexer::skipComment() {
    if (peek() == '/' && peek(1)[0] == '/') {
        // 单行注释
        while (!isAtEnd() && peek() != '\n') {
            consume();
        }
        return true;
    } else if (peek() == '/' && peek(1)[0] == '*') {
        // 多行注释
        consume(); // 消费第一个 '/'
        consume(); // 消费第二个 '*'
        
        while (!isAtEnd()) {
            if (peek() == '*' && peek(1)[0] == '/') {
                consume(); // 消费 '*'
                consume(); // 消费 '/'
                break;
            }
            consume();
        }
        return true;
    } else if (peek() == '-' && peek(1)[0] == '-') {
        // 生成器注释
        while (!isAtEnd() && peek() != '\n') {
            consume();
        }
        return true;
    }
    
    return false;
}

Token CHTLLexer::readIdentifier() {
    size_t start = current_pos_;
    
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '_')) {
        consume();
    }
    
    std::string word = source_code_.substr(start, current_pos_ - start);
    
    // 检查是否为关键字
    if (isKeyword(word)) {
        return readKeyword(word);
    }
    
    return Token(TokenType::IDENTIFIER, word, current_line_, current_column_ - word.length(), start);
}

Token CHTLLexer::readString() {
    char quote = consume();
    size_t start = current_pos_;
    
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\\') {
            consume(); // 消费转义字符
        }
        consume();
    }
    
    if (!isAtEnd()) {
        consume(); // 消费结束引号
    }
    
    std::string value = source_code_.substr(start, current_pos_ - start - 1);
    return Token(TokenType::STRING, value, current_line_, current_column_ - value.length() - 2, start - 1);
}

Token CHTLLexer::readNumber() {
    size_t start = current_pos_;
    
    while (!isAtEnd() && (isDigit(peek()) || peek() == '.')) {
        consume();
    }
    
    std::string value = source_code_.substr(start, current_pos_ - start);
    return Token(TokenType::NUMBER, value, current_line_, current_column_ - value.length(), start);
}

Token CHTLLexer::readOperator() {
    size_t start = current_pos_;
    
    // 尝试读取多字符操作符
    if (current_pos_ + 1 < source_code_.length()) {
        std::string two_char = source_code_.substr(current_pos_, 2);
        if (operators_.find(two_char) != operators_.end()) {
            consume();
            consume();
            return Token(operators_[two_char], two_char, current_line_, current_column_ - 2, start);
        }
    }
    
    // 单字符操作符
    char ch = consume();
    std::string op(1, ch);
    if (operators_.find(op) != operators_.end()) {
        return Token(operators_[op], op, current_line_, current_column_ - 1, start);
    }
    
    return Token(TokenType::ERROR, op, current_line_, current_column_ - 1, start);
}

Token CHTLLexer::readKeyword(const std::string& word) {
    auto it = keywords_.find(word);
    if (it != keywords_.end()) {
        return Token(it->second, word, current_line_, current_column_ - word.length(), current_pos_ - word.length());
    }
    
    return Token(TokenType::IDENTIFIER, word, current_line_, current_column_ - word.length(), current_pos_ - word.length());
}

bool CHTLLexer::isKeyword(const std::string& word) const {
    return keywords_.find(word) != keywords_.end();
}

bool CHTLLexer::isOperator(char ch) const {
    std::string op(1, ch);
    return operators_.find(op) != operators_.end();
}

bool CHTLLexer::isAlpha(char ch) const {
    return std::isalpha(ch) || ch == '_';
}

bool CHTLLexer::isDigit(char ch) const {
    return std::isdigit(ch);
}

bool CHTLLexer::isAlphaNumeric(char ch) const {
    return isAlpha(ch) || isDigit(ch);
}

char CHTLLexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_code_[current_pos_];
}

std::string CHTLLexer::peek(int n) const {
    if (current_pos_ + n > source_code_.length()) return "";
    return source_code_.substr(current_pos_, n);
}

char CHTLLexer::consume() {
    if (isAtEnd()) return '\0';
    
    char ch = source_code_[current_pos_++];
    if (ch == '\n') {
        current_line_++;
        current_column_ = 1;
    } else {
        current_column_++;
    }
    
    return ch;
}

std::string CHTLLexer::consume(int n) {
    std::string result;
    for (int i = 0; i < n && !isAtEnd(); i++) {
        result += consume();
    }
    return result;
}

bool CHTLLexer::isAtEnd() const {
    return current_pos_ >= source_code_.length();
}

} // namespace CHTL
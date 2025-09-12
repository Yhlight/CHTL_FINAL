#include "CHTL_JS/CHTLJSLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLJSLexer::CHTLJSLexer() 
    : current_pos_(0), current_line_(1), current_column_(1), debug_mode_(false) {
    
    // 初始化CHTL JS关键字映射
    keywords_ = {
        // CHTL JS特有关键字
        {"listen", CHTLJSTokenType::LISTEN},
        {"animate", CHTLJSTokenType::ANIMATE},
        {"delegate", CHTLJSTokenType::DELEGATE},
        {"router", CHTLJSTokenType::ROUTER},
        {"fileloader", CHTLJSTokenType::FILELOADER},
        {"vir", CHTLJSTokenType::VIR},
        {"iNeverAway", CHTLJSTokenType::INEVERAWAY},
        {"util", CHTLJSTokenType::UTIL},
        
        // JavaScript关键字
        {"function", CHTLJSTokenType::FUNCTION},
        {"const", CHTLJSTokenType::CONST},
        {"let", CHTLJSTokenType::LET},
        {"var", CHTLJSTokenType::VAR},
        {"return", CHTLJSTokenType::RETURN},
        {"if", CHTLJSTokenType::IF},
        {"else", CHTLJSTokenType::ELSE},
        {"for", CHTLJSTokenType::FOR},
        {"while", CHTLJSTokenType::WHILE},
        {"do", CHTLJSTokenType::DO},
        {"switch", CHTLJSTokenType::SWITCH},
        {"case", CHTLJSTokenType::CASE},
        {"default", CHTLJSTokenType::DEFAULT},
        {"break", CHTLJSTokenType::BREAK},
        {"continue", CHTLJSTokenType::CONTINUE},
        {"try", CHTLJSTokenType::TRY},
        {"catch", CHTLJSTokenType::CATCH},
        {"finally", CHTLJSTokenType::FINALLY},
        {"throw", CHTLJSTokenType::THROW}
    };
    
    // 初始化操作符映射
    operators_ = {
        {":", CHTLJSTokenType::COLON},
        {"=", CHTLJSTokenType::EQUAL},
        {";", CHTLJSTokenType::SEMICOLON},
        {",", CHTLJSTokenType::COMMA},
        {".", CHTLJSTokenType::DOT},
        {"->", CHTLJSTokenType::ARROW},
        {"/", CHTLJSTokenType::SLASH},
        {"\\", CHTLJSTokenType::BACKSLASH},
        {"{", CHTLJSTokenType::LBRACE},
        {"}", CHTLJSTokenType::RBRACE},
        {"[", CHTLJSTokenType::LBRACKET},
        {"]", CHTLJSTokenType::RBRACKET},
        {"(", CHTLJSTokenType::LPAREN},
        {")", CHTLJSTokenType::RPAREN},
        {">", CHTLJSTokenType::GT},
        {"<", CHTLJSTokenType::LT},
        {">=", CHTLJSTokenType::GE},
        {"<=", CHTLJSTokenType::LE},
        {"==", CHTLJSTokenType::EQ},
        {"!=", CHTLJSTokenType::NE},
        {"===", CHTLJSTokenType::STRICT_EQ},
        {"!==", CHTLJSTokenType::STRICT_NE},
        {"&&", CHTLJSTokenType::AND},
        {"||", CHTLJSTokenType::OR},
        {"!", CHTLJSTokenType::NOT},
        {"+", CHTLJSTokenType::PLUS},
        {"-", CHTLJSTokenType::MINUS},
        {"*", CHTLJSTokenType::MULTIPLY},
        {"/", CHTLJSTokenType::DIVIDE},
        {"%", CHTLJSTokenType::MODULO},
        {"**", CHTLJSTokenType::POWER},
        {"++", CHTLJSTokenType::INCREMENT},
        {"--", CHTLJSTokenType::DECREMENT},
        {"+=", CHTLJSTokenType::PLUS_EQ},
        {"-=", CHTLJSTokenType::MINUS_EQ},
        {"*=", CHTLJSTokenType::MULTIPLY_EQ},
        {"/=", CHTLJSTokenType::DIVIDE_EQ},
        {"%=", CHTLJSTokenType::MODULO_EQ},
        {"**=", CHTLJSTokenType::POWER_EQ},
        {"?", CHTLJSTokenType::QUESTION},
        {"&", CHTLJSTokenType::AMPERSAND},
        {"|", CHTLJSTokenType::PIPE},
        {"~", CHTLJSTokenType::TILDE},
        {"^", CHTLJSTokenType::CARET},
        {"$", CHTLJSTokenType::DOLLAR},
        {"@", CHTLJSTokenType::AT},
        {"#", CHTLJSTokenType::HASH},
        {"_", CHTLJSTokenType::UNDERSCORE}
    };
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize(const std::string& source_code) {
    source_code_ = source_code;
    current_pos_ = 0;
    current_line_ = 1;
    current_column_ = 1;
    
    std::vector<CHTLJSToken> tokens;
    
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
            tokens.push_back(CHTLJSToken(CHTLJSTokenType::NEWLINE, "\n", current_line_, current_column_, current_pos_));
            consume();
            current_line_++;
            current_column_ = 1;
        } else {
            // 未知字符，跳过
            consume();
        }
    }
    
    tokens.push_back(CHTLJSToken(CHTLJSTokenType::EOF_TOKEN, "", current_line_, current_column_, current_pos_));
    return tokens;
}

void CHTLJSLexer::reset() {
    current_pos_ = 0;
    current_line_ = 1;
    current_column_ = 1;
    source_code_.clear();
}

void CHTLJSLexer::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
}

size_t CHTLJSLexer::getCurrentLine() const {
    return current_line_;
}

size_t CHTLJSLexer::getCurrentColumn() const {
    return current_column_;
}

size_t CHTLJSLexer::getCurrentPosition() const {
    return current_pos_;
}

void CHTLJSLexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(peek()) && peek() != '\n') {
        consume();
    }
}

bool CHTLJSLexer::skipComment() {
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
    }
    
    return false;
}

CHTLJSToken CHTLJSLexer::readIdentifier() {
    size_t start = current_pos_;
    
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '_')) {
        consume();
    }
    
    std::string word = source_code_.substr(start, current_pos_ - start);
    
    // 检查是否为关键字
    if (isKeyword(word)) {
        return readKeyword(word);
    }
    
    return CHTLJSToken(CHTLJSTokenType::IDENTIFIER, word, current_line_, current_column_ - word.length(), start);
}

CHTLJSToken CHTLJSLexer::readString() {
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
    return CHTLJSToken(CHTLJSTokenType::STRING, value, current_line_, current_column_ - value.length() - 2, start - 1);
}

CHTLJSToken CHTLJSLexer::readNumber() {
    size_t start = current_pos_;
    
    while (!isAtEnd() && (isDigit(peek()) || peek() == '.')) {
        consume();
    }
    
    std::string value = source_code_.substr(start, current_pos_ - start);
    return CHTLJSToken(CHTLJSTokenType::NUMBER, value, current_line_, current_column_ - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readOperator() {
    size_t start = current_pos_;
    
    // 尝试读取多字符操作符
    if (current_pos_ + 2 < source_code_.length()) {
        std::string three_char = source_code_.substr(current_pos_, 3);
        if (operators_.find(three_char) != operators_.end()) {
            consume();
            consume();
            consume();
            return CHTLJSToken(operators_[three_char], three_char, current_line_, current_column_ - 3, start);
        }
    }
    
    if (current_pos_ + 1 < source_code_.length()) {
        std::string two_char = source_code_.substr(current_pos_, 2);
        if (operators_.find(two_char) != operators_.end()) {
            consume();
            consume();
            return CHTLJSToken(operators_[two_char], two_char, current_line_, current_column_ - 2, start);
        }
    }
    
    // 单字符操作符
    char ch = consume();
    std::string op(1, ch);
    if (operators_.find(op) != operators_.end()) {
        return CHTLJSToken(operators_[op], op, current_line_, current_column_ - 1, start);
    }
    
    return CHTLJSToken(CHTLJSTokenType::ERROR, op, current_line_, current_column_ - 1, start);
}

CHTLJSToken CHTLJSLexer::readKeyword(const std::string& word) {
    auto it = keywords_.find(word);
    if (it != keywords_.end()) {
        return CHTLJSToken(it->second, word, current_line_, current_column_ - word.length(), current_pos_ - word.length());
    }
    
    return CHTLJSToken(CHTLJSTokenType::IDENTIFIER, word, current_line_, current_column_ - word.length(), current_pos_ - word.length());
}

bool CHTLJSLexer::isKeyword(const std::string& word) const {
    return keywords_.find(word) != keywords_.end();
}

bool CHTLJSLexer::isOperator(char ch) const {
    std::string op(1, ch);
    return operators_.find(op) != operators_.end();
}

bool CHTLJSLexer::isAlpha(char ch) const {
    return std::isalpha(ch) || ch == '_';
}

bool CHTLJSLexer::isDigit(char ch) const {
    return std::isdigit(ch);
}

bool CHTLJSLexer::isAlphaNumeric(char ch) const {
    return isAlpha(ch) || isDigit(ch);
}

char CHTLJSLexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_code_[current_pos_];
}

std::string CHTLJSLexer::peek(int n) const {
    if (current_pos_ + n > source_code_.length()) return "";
    return source_code_.substr(current_pos_, n);
}

char CHTLJSLexer::consume() {
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

std::string CHTLJSLexer::consume(int n) {
    std::string result;
    for (int i = 0; i < n && !isAtEnd(); i++) {
        result += consume();
    }
    return result;
}

bool CHTLJSLexer::isAtEnd() const {
    return current_pos_ >= source_code_.length();
}

} // namespace CHTL
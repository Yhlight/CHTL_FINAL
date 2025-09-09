#include "lexer/lexer.h"
#include <cctype>
#include <algorithm>

namespace chtl {
namespace lexer {

Lexer::Lexer(const std::string& source) 
    : source_(source), position_(0), line_(1), column_(1), start_position_(0) {
    initialize_keywords();
    initialize_operators();
}

void Lexer::initialize_keywords() {
    keywords_ = {
        // Basic keywords
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        
        // Template keywords
        {"template", TokenType::TEMPLATE},
        {"@Style", TokenType::TEMPLATE_STYLE},
        {"@Element", TokenType::TEMPLATE_ELEMENT},
        {"@Var", TokenType::TEMPLATE_VAR},
        
        // Custom keywords
        {"Custom", TokenType::CUSTOM},
        {"@Style", TokenType::CUSTOM_STYLE},
        {"@Element", TokenType::CUSTOM_ELEMENT},
        {"@Var", TokenType::CUSTOM_VAR},
        
        // Origin keywords
        {"Origin", TokenType::ORIGIN},
        {"@Html", TokenType::ORIGIN_HTML},
        {"@Style", TokenType::ORIGIN_STYLE},
        {"@JavaScript", TokenType::ORIGIN_JAVASCRIPT},
        
        // Import keywords
        {"Import", TokenType::IMPORT},
        {"@Html", TokenType::IMPORT_HTML},
        {"@Style", TokenType::IMPORT_STYLE},
        {"@JavaScript", TokenType::IMPORT_JAVASCRIPT},
        {"@Chtl", TokenType::IMPORT_CHTL},
        {"@CJmod", TokenType::IMPORT_CJMOD},
        
        // Other keywords
        {"Namespace", TokenType::NAMESPACE},
        {"Configuration", TokenType::CONFIGURATION},
        {"use", TokenType::USE},
        {"except", TokenType::EXCEPT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"inherit", TokenType::INHERIT},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"at", TokenType::AT_TOP},
        {"top", TokenType::AT_TOP},
        {"bottom", TokenType::AT_BOTTOM},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        
        // Boolean literals
        {"true", TokenType::BOOLEAN_LITERAL},
        {"false", TokenType::BOOLEAN_LITERAL}
    };
    
    initialize_chtl_keywords();
}

void Lexer::initialize_operators() {
    operators_ = {
        {"=", TokenType::ASSIGN},
        {":", TokenType::COLON},
        {";", TokenType::SEMICOLON},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"->", TokenType::ARROW},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::MULTIPLY},
        {"/", TokenType::DIVIDE},
        {"%", TokenType::MODULO},
        {"**", TokenType::POWER},
        {"==", TokenType::EQUAL},
        {"!=", TokenType::NOT_EQUAL},
        {"<", TokenType::LESS},
        {"<=", TokenType::LESS_EQUAL},
        {">", TokenType::GREATER},
        {">=", TokenType::GREATER_EQUAL},
        {"&&", TokenType::AND},
        {"||", TokenType::OR},
        {"!", TokenType::NOT},
        {"?", TokenType::QUESTION},
        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {"[", TokenType::LEFT_BRACKET},
        {"]", TokenType::RIGHT_BRACKET},
        {"{", TokenType::LEFT_BRACE},
        {"}", TokenType::RIGHT_BRACE}
    };
}

void Lexer::initialize_chtl_keywords() {
    // CHTL特有的关键字和语法
    keywords_["html5"] = TokenType::IDENTIFIER; // HTML5类型
    keywords_["html"] = TokenType::IDENTIFIER;
    keywords_["head"] = TokenType::IDENTIFIER;
    keywords_["body"] = TokenType::IDENTIFIER;
    keywords_["div"] = TokenType::IDENTIFIER;
    keywords_["span"] = TokenType::IDENTIFIER;
    keywords_["p"] = TokenType::IDENTIFIER;
    keywords_["a"] = TokenType::IDENTIFIER;
    keywords_["img"] = TokenType::IDENTIFIER;
    keywords_["ul"] = TokenType::IDENTIFIER;
    keywords_["ol"] = TokenType::IDENTIFIER;
    keywords_["li"] = TokenType::IDENTIFIER;
    keywords_["table"] = TokenType::IDENTIFIER;
    keywords_["tr"] = TokenType::IDENTIFIER;
    keywords_["td"] = TokenType::IDENTIFIER;
    keywords_["th"] = TokenType::IDENTIFIER;
    keywords_["form"] = TokenType::IDENTIFIER;
    keywords_["input"] = TokenType::IDENTIFIER;
    keywords_["button"] = TokenType::IDENTIFIER;
    keywords_["title"] = TokenType::IDENTIFIER;
    keywords_["meta"] = TokenType::IDENTIFIER;
    keywords_["link"] = TokenType::IDENTIFIER;
    keywords_["script"] = TokenType::SCRIPT;
    keywords_["style"] = TokenType::STYLE;
    
    // CHTL JS关键字
    keywords_["fileloader"] = TokenType::IDENTIFIER;
    keywords_["listen"] = TokenType::IDENTIFIER;
    keywords_["delegate"] = TokenType::IDENTIFIER;
    keywords_["animate"] = TokenType::IDENTIFIER;
    keywords_["vir"] = TokenType::IDENTIFIER;
    keywords_["router"] = TokenType::IDENTIFIER;
    keywords_["iNeverAway"] = TokenType::IDENTIFIER;
    keywords_["util"] = TokenType::IDENTIFIER;
    keywords_["printMylove"] = TokenType::IDENTIFIER;
    
    // 配置关键字
    keywords_["INDEX_INITIAL_COUNT"] = TokenType::IDENTIFIER;
    keywords_["DEBUG_MODE"] = TokenType::IDENTIFIER;
    keywords_["DISABLE_NAME_GROUP"] = TokenType::IDENTIFIER;
    keywords_["DISABLE_STYLE_AUTO_ADD_CLASS"] = TokenType::IDENTIFIER;
    keywords_["DISABLE_STYLE_AUTO_ADD_ID"] = TokenType::IDENTIFIER;
    keywords_["DISABLE_DEFAULT_NAMESPACE"] = TokenType::IDENTIFIER;
    keywords_["DISABLE_CUSTOM_ORIGIN_TYPE"] = TokenType::IDENTIFIER;
    keywords_["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = TokenType::IDENTIFIER;
    keywords_["DISABLE_SCRIPT_AUTO_ADD_ID"] = TokenType::IDENTIFIER;
    
    // 模块关键字
    keywords_["Info"] = TokenType::IDENTIFIER;
    keywords_["Export"] = TokenType::IDENTIFIER;
    keywords_["Name"] = TokenType::IDENTIFIER;
    keywords_["OriginType"] = TokenType::IDENTIFIER;
}

char Lexer::current_char() const {
    if (position_ >= source_.length()) {
        return '\0';
    }
    return source_[position_];
}

char Lexer::peek_char(size_t offset) const {
    size_t index = position_ + offset;
    if (index >= source_.length()) {
        return '\0';
    }
    return source_[index];
}

void Lexer::advance() {
    if (position_ < source_.length()) {
        if (source_[position_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        position_++;
    }
}

void Lexer::skip_whitespace() {
    while (position_ < source_.length() && is_whitespace(current_char())) {
        advance();
    }
}

void Lexer::skip_comment() {
    if (current_char() == '/' && peek_char() == '/') {
        skip_single_line_comment();
    } else if (current_char() == '/' && peek_char() == '*') {
        skip_multi_line_comment();
    } else if (current_char() == '-' && peek_char() == '-') {
        skip_generator_comment();
    }
}

void Lexer::skip_single_line_comment() {
    while (position_ < source_.length() && current_char() != '\n') {
        advance();
    }
}

void Lexer::skip_multi_line_comment() {
    advance(); // skip '/'
    advance(); // skip '*'
    
    while (position_ < source_.length()) {
        if (current_char() == '*' && peek_char() == '/') {
            advance(); // skip '*'
            advance(); // skip '/'
            break;
        }
        advance();
    }
}

void Lexer::skip_generator_comment() {
    while (position_ < source_.length() && current_char() != '\n') {
        advance();
    }
}

Token Lexer::scan_identifier() {
    start_position_ = position_;
    
    while (position_ < source_.length() && 
           (is_letter(current_char()) || is_digit(current_char()) || current_char() == '_')) {
        advance();
    }
    
    std::string value = source_.substr(start_position_, position_ - start_position_);
    
    // Check if it's a keyword
    auto it = keywords_.find(value);
    if (it != keywords_.end()) {
        return create_token(it->second, value);
    }
    
    return create_token(TokenType::IDENTIFIER, value);
}

Token Lexer::scan_string_literal() {
    start_position_ = position_;
    char quote = current_char();
    advance(); // skip opening quote
    
    while (position_ < source_.length() && current_char() != quote) {
        if (current_char() == '\\' && peek_char() == quote) {
            advance(); // skip backslash
        }
        advance();
    }
    
    if (position_ < source_.length()) {
        advance(); // skip closing quote
    }
    
    std::string value = source_.substr(start_position_, position_ - start_position_);
    return create_token(TokenType::STRING_LITERAL, value);
}

Token Lexer::scan_number_literal() {
    start_position_ = position_;
    
    while (position_ < source_.length() && 
           (is_digit(current_char()) || current_char() == '.')) {
        advance();
    }
    
    std::string value = source_.substr(start_position_, position_ - start_position_);
    return create_token(TokenType::NUMBER_LITERAL, value);
}

Token Lexer::scan_operator() {
    start_position_ = position_;
    
    // Check for multi-character operators first
    if (position_ + 1 < source_.length()) {
        std::string two_char = source_.substr(position_, 2);
        auto it = operators_.find(two_char);
        if (it != operators_.end()) {
            advance();
            advance();
            return create_token(it->second, two_char);
        }
    }
    
    // Single character operators
    std::string one_char(1, current_char());
    auto it = operators_.find(one_char);
    if (it != operators_.end()) {
        advance();
        return create_token(it->second, one_char);
    }
    
    return scan_unknown();
}

Token Lexer::scan_bracket() {
    char c = current_char();
    advance();
    
    switch (c) {
        case '(': return create_token(TokenType::LEFT_PAREN, "(");
        case ')': return create_token(TokenType::RIGHT_PAREN, ")");
        case '[': return create_token(TokenType::LEFT_BRACKET, "[");
        case ']': return create_token(TokenType::RIGHT_BRACKET, "]");
        case '{': return create_token(TokenType::LEFT_BRACE, "{");
        case '}': return create_token(TokenType::RIGHT_BRACE, "}");
        default: return scan_unknown();
    }
}

Token Lexer::scan_unknown() {
    char c = current_char();
    advance();
    return create_token(TokenType::UNKNOWN, std::string(1, c));
}

bool Lexer::is_letter(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::is_digit(char c) const {
    return std::isdigit(c);
}

bool Lexer::is_whitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool Lexer::is_operator_start(char c) const {
    return operators_.find(std::string(1, c)) != operators_.end();
}

bool Lexer::is_bracket(char c) const {
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}';
}

Token Lexer::create_token(TokenType type, const std::string& value) {
    return Token(type, value, line_, column_, start_position_);
}

Token Lexer::create_token(TokenType type) {
    return Token(type, "", line_, column_, start_position_);
}

TokenStream Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (position_ < source_.length()) {
        skip_whitespace();
        
        if (position_ >= source_.length()) {
            break;
        }
        
        char c = current_char();
        
        if (is_letter(c)) {
            tokens.push_back(scan_identifier());
        } else if (c == '"' || c == '\'') {
            tokens.push_back(scan_string_literal());
        } else if (is_digit(c)) {
            tokens.push_back(scan_number_literal());
        } else if (is_operator_start(c)) {
            tokens.push_back(scan_operator());
        } else if (is_bracket(c)) {
            tokens.push_back(scan_bracket());
        } else if (c == '/' && (peek_char() == '/' || peek_char() == '*')) {
            skip_comment();
        } else if (c == '-' && peek_char() == '-') {
            skip_comment();
        } else {
            tokens.push_back(scan_unknown());
        }
    }
    
    tokens.push_back(create_token(TokenType::EOF_TOKEN));
    return TokenStream(tokens);
}

std::vector<Token> Lexer::tokenize_to_vector() {
    return tokenize().get_tokens();
}

void Lexer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    start_position_ = 0;
}

} // namespace lexer
} // namespace chtl
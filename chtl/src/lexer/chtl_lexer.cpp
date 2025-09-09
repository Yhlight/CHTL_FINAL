#include "lexer/chtl_lexer.h"
#include <cctype>
#include <algorithm>

namespace chtl {
namespace lexer {

CHTLLexer::CHTLLexer(const std::string& source) 
    : source_(source), position_(0), line_(1), column_(1), start_position_(0) {
    initialize_keywords();
    initialize_operators();
    initialize_html_elements();
    initialize_chtl_js_keywords();
}

void CHTLLexer::initialize_keywords() {
    keywords_ = {
        // Basic CHTL keywords
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        
        // Template keywords
        {"Template", TokenType::TEMPLATE},
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
        {"false", TokenType::BOOLEAN_LITERAL},
        
        // HTML5
        {"html5", TokenType::IDENTIFIER}
    };
}

void CHTLLexer::initialize_operators() {
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

void CHTLLexer::initialize_html_elements() {
    html_elements_ = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "textarea", "select", "option", "label",
        "h1", "h2", "h3", "h4", "h5", "h6", "br", "hr", "section", "article",
        "header", "footer", "nav", "aside", "main", "figure", "figcaption",
        "canvas", "svg", "video", "audio", "source", "track", "embed", "object",
        "iframe", "area", "map", "base", "col", "colgroup", "thead", "tbody",
        "tfoot", "caption", "fieldset", "legend", "optgroup", "datalist",
        "output", "progress", "meter", "details", "summary", "dialog", "menu",
        "menuitem", "command", "keygen", "param", "wbr"
    };
}

void CHTLLexer::initialize_chtl_js_keywords() {
    chtl_js_keywords_ = {
        "fileloader", "listen", "delegate", "animate", "vir", "router",
        "iNeverAway", "util", "printMylove", "const", "let", "var",
        "function", "if", "else", "for", "while", "do", "switch", "case",
        "default", "break", "continue", "return", "try", "catch", "finally",
        "throw", "new", "this", "typeof", "instanceof", "in", "of",
        "class", "extends", "super", "static", "async", "await", "yield",
        "import", "export", "from", "as", "default"
    };
}

char CHTLLexer::current_char() const {
    if (position_ >= source_.length()) {
        return '\0';
    }
    return source_[position_];
}

char CHTLLexer::peek_char(size_t offset) const {
    size_t index = position_ + offset;
    if (index >= source_.length()) {
        return '\0';
    }
    return source_[index];
}

void CHTLLexer::advance() {
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

void CHTLLexer::skip_whitespace() {
    while (position_ < source_.length() && is_whitespace(current_char())) {
        advance();
    }
}

void CHTLLexer::skip_comment() {
    if (current_char() == '/' && peek_char() == '/') {
        skip_single_line_comment();
    } else if (current_char() == '/' && peek_char() == '*') {
        skip_multi_line_comment();
    } else if (current_char() == '-' && peek_char() == '-') {
        skip_generator_comment();
    }
}

void CHTLLexer::skip_single_line_comment() {
    while (position_ < source_.length() && current_char() != '\n') {
        advance();
    }
}

void CHTLLexer::skip_multi_line_comment() {
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

void CHTLLexer::skip_generator_comment() {
    while (position_ < source_.length() && current_char() != '\n') {
        advance();
    }
}

Token CHTLLexer::scan_identifier() {
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
    
    // Check if it's an HTML element
    if (html_elements_.find(value) != html_elements_.end()) {
        return create_token(TokenType::IDENTIFIER, value);
    }
    
    // Check if it's a CHTL JS keyword
    if (chtl_js_keywords_.find(value) != chtl_js_keywords_.end()) {
        return create_token(TokenType::IDENTIFIER, value);
    }
    
    return create_token(TokenType::IDENTIFIER, value);
}

Token CHTLLexer::scan_string_literal() {
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

Token CHTLLexer::scan_number_literal() {
    start_position_ = position_;
    
    while (position_ < source_.length() && 
           (is_digit(current_char()) || current_char() == '.')) {
        advance();
    }
    
    std::string value = source_.substr(start_position_, position_ - start_position_);
    return create_token(TokenType::NUMBER_LITERAL, value);
}

Token CHTLLexer::scan_operator() {
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

Token CHTLLexer::scan_bracket() {
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

Token CHTLLexer::scan_chtl_special() {
    if (current_char() == '[') {
        return scan_square_bracket_construct();
    } else if (current_char() == '{' && peek_char() == '{') {
        return scan_curly_selector();
    }
    
    return scan_unknown();
}

Token CHTLLexer::scan_selector() {
    start_position_ = position_;
    
    while (position_ < source_.length() && current_char() != '}') {
        advance();
    }
    
    if (current_char() == '}') {
        advance(); // skip closing }
    }
    
    std::string value = source_.substr(start_position_, position_ - start_position_);
    return create_token(TokenType::IDENTIFIER, value); // Treat as identifier for now
}

Token CHTLLexer::scan_unknown() {
    char c = current_char();
    advance();
    return create_token(TokenType::UNKNOWN, std::string(1, c));
}

Token CHTLLexer::scan_square_bracket_construct() {
    start_position_ = position_;
    advance(); // skip '['
    
    while (position_ < source_.length() && current_char() != ']') {
        advance();
    }
    
    if (current_char() == ']') {
        advance(); // skip ']'
    }
    
    std::string value = source_.substr(start_position_, position_ - start_position_);
    
    // Determine the type based on content
    if (value.find("Template") != std::string::npos) {
        return create_token(TokenType::TEMPLATE, value);
    } else if (value.find("Custom") != std::string::npos) {
        return create_token(TokenType::CUSTOM, value);
    } else if (value.find("Origin") != std::string::npos) {
        return create_token(TokenType::ORIGIN, value);
    } else if (value.find("Import") != std::string::npos) {
        return create_token(TokenType::IMPORT, value);
    } else if (value.find("Namespace") != std::string::npos) {
        return create_token(TokenType::NAMESPACE, value);
    } else if (value.find("Configuration") != std::string::npos) {
        return create_token(TokenType::CONFIGURATION, value);
    }
    
    return create_token(TokenType::IDENTIFIER, value);
}

Token CHTLLexer::scan_curly_selector() {
    start_position_ = position_;
    advance(); // skip first '{'
    advance(); // skip second '{'
    
    while (position_ < source_.length() && 
           !(current_char() == '}' && peek_char() == '}')) {
        advance();
    }
    
    if (current_char() == '}' && peek_char() == '}') {
        advance(); // skip first '}'
        advance(); // skip second '}'
    }
    
    std::string value = source_.substr(start_position_, position_ - start_position_);
    return create_token(TokenType::IDENTIFIER, value); // Treat as identifier for now
}

Token CHTLLexer::scan_unquoted_literal() {
    start_position_ = position_;
    
    while (position_ < source_.length() && 
           !is_whitespace(current_char()) && 
           !is_operator_start(current_char()) && 
           !is_bracket(current_char()) &&
           current_char() != ';' && current_char() != ',') {
        advance();
    }
    
    std::string value = source_.substr(start_position_, position_ - start_position_);
    return create_token(TokenType::STRING_LITERAL, value);
}

bool CHTLLexer::is_letter(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::is_digit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::is_whitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool CHTLLexer::is_operator_start(char c) const {
    return operators_.find(std::string(1, c)) != operators_.end();
}

bool CHTLLexer::is_bracket(char c) const {
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}';
}

bool CHTLLexer::is_chtl_special_start() const {
    return current_char() == '[' || (current_char() == '{' && peek_char() == '{');
}

bool CHTLLexer::is_selector_start() const {
    return current_char() == '{' && peek_char() == '{';
}

Token CHTLLexer::create_token(TokenType type, const std::string& value) {
    return Token(type, value, line_, column_, start_position_);
}

Token CHTLLexer::create_token(TokenType type) {
    return Token(type, "", line_, column_, start_position_);
}

TokenStream CHTLLexer::tokenize() {
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
        } else if (is_chtl_special_start()) {
            tokens.push_back(scan_chtl_special());
        } else if (is_bracket(c)) {
            tokens.push_back(scan_bracket());
        } else if (c == '/' && (peek_char() == '/' || peek_char() == '*')) {
            skip_comment();
        } else if (c == '-' && peek_char() == '-') {
            skip_comment();
        } else {
            // Try to scan as unquoted literal
            tokens.push_back(scan_unquoted_literal());
        }
    }
    
    tokens.push_back(create_token(TokenType::EOF_TOKEN));
    return TokenStream(tokens);
}

std::vector<Token> CHTLLexer::tokenize_to_vector() {
    return tokenize().get_tokens();
}

void CHTLLexer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    start_position_ = 0;
}

} // namespace lexer
} // namespace chtl
#include "CHTLLexer.h"
#include <iostream>

namespace CHTL {

const std::unordered_map<std::string, TokenType> CHTLLexer::keywords = {
    {"text", TokenType::Text},
    {"style", TokenType::Style},
    {"script", TokenType::Script},
    {"inherit", TokenType::Inherit},
    {"delete", TokenType::Delete},
    {"insert", TokenType::Insert},
    {"after", TokenType::After},
    {"before", TokenType::Before},
    {"replace", TokenType::Replace},
    {"from", TokenType::From},
    {"as", TokenType::As},
    {"except", TokenType::Except},
    {"use", TokenType::Use},
    {"html5", TokenType::Html5},
    {"@Style", TokenType::AtStyle},
    {"@Element", TokenType::AtElement},
    {"@Var", TokenType::AtVar},
    {"@Html", TokenType::AtHtml},
    {"@JavaScript", TokenType::AtJavaScript},
    {"@Chtl", TokenType::AtChtl},
};

CHTLLexer::CHTLLexer(const std::string& source) : source_(source) {}

std::vector<Token> CHTLLexer::scanTokens() {
    while (!isAtEnd()) {
        start_ = current_;
        scanToken();
    }
    tokens_.push_back({TokenType::EndOfFile, "", line_, current_ - line_start_ + 1});
    return tokens_;
}

void CHTLLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::OpenBrace); break;
        case '}': addToken(TokenType::CloseBrace); break;
        case '(': addToken(TokenType::OpenParen); break;
        case ')': addToken(TokenType::CloseParen); break;
        case ']': addToken(TokenType::CloseBracket); break;
        case ':': addToken(TokenType::Colon); break;
        case ';': addToken(TokenType::Semicolon); break;
        case ',': addToken(TokenType::Comma); break;
        case '.': addToken(TokenType::Dot); break;
        case '?': addToken(TokenType::QuestionMark); break;
        case '&':
            if (match('&')) {
                addToken(TokenType::DoubleAmpersand);
            } else {
                identifier();
            }
            break;
        case '|': addToken(match('|') ? TokenType::DoublePipe : TokenType::Pipe); break;
        case '=': addToken(match('=') ? TokenType::DoubleEquals : TokenType::Equals); break;
        case '!':
            if (match('=')) {
                addToken(TokenType::NotEquals);
            } else {
                // Could be an error, or just a lone '!' if the language supports it.
                // For now, let's treat it as part of an unquoted literal if it's not '!='
                unquotedLiteral();
            }
            break;
        case '+': addToken(TokenType::Plus); break;
        case '>': addToken(match('=') ? TokenType::GreaterThanEquals : TokenType::GreaterThan); break;
        case '<': addToken(match('=') ? TokenType::LessThanEquals : TokenType::LessThan); break;
        case '%': addToken(TokenType::Percent); break;

        case '$': {
            start_++; // Skip the opening '$'
            while (isAlphaNumeric(peek()) || peek() == '_') {
                advance();
            }
            std::string varName = source_.substr(start_, current_ - start_);
            if (peek() != '$') {
                // This is not a valid responsive value, treat as unquoted literal
                current_ = start_ -1; // backtrack
                unquotedLiteral();
            } else {
                advance(); // consume the closing '$'
                addToken(TokenType::ResponsiveValue, varName);
            }
            break;
        }

        case '#':
            if (peek() == ' ') {
                while (peek() != '\n' && !isAtEnd()) advance();
                std::string comment = source_.substr(start_ + 2, current_ - (start_ + 2));
                addToken(TokenType::GeneratorComment, comment);
            } else {
                identifier();
            }
            break;

        case '-':
            if (match('>')) {
                addToken(TokenType::Arrow);
            } else {
                addToken(TokenType::Minus);
            }
            break;

        case '*':
            if (match('*')) {
                addToken(TokenType::DoubleAsterisk);
            } else {
                addToken(TokenType::Asterisk);
            }
            break;

        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line_++;
                    advance();
                }
                if (!isAtEnd()) {
                    advance();
                    advance();
                }
            } else {
                addToken(TokenType::Slash);
            }
            break;

        case '[':
            handleSpecialSyntax();
            break;

        case '@':
            identifier();
            break;

        case '"':
        case '\'':
            stringLiteral(c);
            break;

        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line_++;
            line_start_ = current_;
            break;

        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c) || c == '_') { // Removed '#'
                identifier();
            } else {
                unquotedLiteral();
            }
            break;
    }
}

void CHTLLexer::handleSpecialSyntax() {
    while (peek() != ']' && !isAtEnd()) {
        advance();
    }
    if (isAtEnd()) {
        addToken(TokenType::Unexpected, "Unterminated special syntax starting with '['.");
        return;
    }
    advance();

    std::string text = source_.substr(start_, current_ - start_);
    if (text == "[Template]") addToken(TokenType::Template);
    else if (text == "[Custom]") addToken(TokenType::Custom);
    else if (text == "[Origin]") addToken(TokenType::Origin);
    else if (text == "[Import]") addToken(TokenType::Import);
    else if (text == "[Namespace]") addToken(TokenType::Namespace);
    else if (text == "[Configuration]") addToken(TokenType::Configuration);
    else if (text == "[Info]") addToken(TokenType::Info);
    else if (text == "[Export]") addToken(TokenType::Export);
    else if (text == "[Name]") addToken(TokenType::Name);
    else if (text == "[OriginType]") addToken(TokenType::OriginType);
    else {
        current_ = start_ + 1;
        addToken(TokenType::OpenBracket);
    }
}

void CHTLLexer::identifier() {
    if (source_[start_] == '@' || source_[start_] == '#' || source_[start_] == '&') {
        while (isAlphaNumeric(peek()) || peek() == '-' || peek() == ':') advance();
    } else {
        while (isAlphaNumeric(peek()) || peek() == '_' || peek() == '-') advance();
    }

    std::string text = source_.substr(start_, current_ - start_);

    if (text == "at" && isspace(peek())) {
        size_t temp_current = current_;
        while(isspace(source_[temp_current])) temp_current++;
        if (source_.substr(temp_current, 3) == "top") {
            current_ = temp_current + 3;
            addToken(TokenType::AtTop, "at top");
            return;
        } else if (source_.substr(temp_current, 6) == "bottom") {
            current_ = temp_current + 6;
            addToken(TokenType::AtBottom, "at bottom");
            return;
        }
    }

    TokenType type;
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        type = it->second;
    } else {
        type = TokenType::Identifier;
    }
    addToken(type, text);
}

void CHTLLexer::number() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }

    addToken(TokenType::Number, source_.substr(start_, current_ - start_));
}

void CHTLLexer::stringLiteral(char quote) {
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line_++;
        advance();
    }
    if (isAtEnd()) {
        addToken(TokenType::Unexpected, "Unterminated string.");
        return;
    }
    advance();
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    addToken(TokenType::StringLiteral, value);
}

void CHTLLexer::unquotedLiteral() {
    while (peek() != ';' && peek() != '{' && peek() != '}' && peek() != '\n' && !isAtEnd()) {
        advance();
    }
    std::string value = source_.substr(start_, current_ - start_);
    size_t last = value.find_last_not_of(" \t\r\n");
    if (std::string::npos != last) {
        value = value.substr(0, last + 1);
    }
    addToken(TokenType::UnquotedLiteral, value);
}

char CHTLLexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char CHTLLexer::peekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

char CHTLLexer::advance() {
    current_++;
    return source_[current_ - 1];
}

bool CHTLLexer::isAtEnd() const {
    return current_ >= source_.length();
}

bool CHTLLexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    return true;
}

bool CHTLLexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool CHTLLexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

void CHTLLexer::addToken(TokenType type) {
    addToken(type, source_.substr(start_, current_ - start_));
}

void CHTLLexer::addToken(TokenType type, const std::string& lexeme) {
    tokens_.push_back({type, lexeme, line_, start_ - line_start_ + 1});
}

}

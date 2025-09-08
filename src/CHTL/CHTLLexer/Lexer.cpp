#include "Lexer.h"
#include <cctype>
#include <cstring>

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

char Lexer::peek() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char Lexer::advance() {
    if (position >= source.length()) {
        return '\0';
    }
    char currentChar = source[position];
    position++;
    if (currentChar == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return currentChar;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (source.substr(position, 2) == "//" || source.substr(position, 2) == "--") {
        advance();
        advance();
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
    } else if (source.substr(position, 2) == "/*") {
        advance();
        advance();
        while (source.substr(position, 2) != "*/" && peek() != '\0') {
            advance();
        }
        if (peek() != '\0') {
            advance();
            advance();
        }
    }
}

Token Lexer::readString(char quote) {
    int startCol = column;
    std::string value;
    advance(); // Consume opening quote
    while (peek() != quote && peek() != '\0') {
        value += advance();
    }
    if (peek() == quote) {
        advance(); // Consume closing quote
    }
    return {TokenType::STRING, value, line, startCol};
}

// In CHTL, identifiers are used for tags, attribute names, and some values.
// Allowing hyphens makes it consistent with CSS classes/ids.
Token Lexer::readIdentifier() {
    int startCol = column;
    std::string value;
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        value += advance();
    }
    if (value == "text") {
        return {TokenType::KEYWORD_TEXT, value, line, startCol};
    }
    return {TokenType::IDENTIFIER, value, line, startCol};
}


// An unquoted literal is a sequence of characters that is not a recognized token.
// It is terminated by a delimiter like ';', '{', '}', or a comment.
// The parser will be responsible for deciding when to expect this token.
Token Lexer::readUnquotedLiteral() {
    int startCol = column;
    std::string value;
    while (peek() != '\0' && peek() != '{' && peek() != '}' && peek() != ';') {
        // Check for start of a comment, which terminates the literal
        if (peek() == '/') {
            if (source.substr(position, 2) == "//" || source.substr(position, 2) == "/*") {
                break;
            }
        }
        if (peek() == '-') {
             if (source.substr(position, 2) == "--") {
                break;
            }
        }
        value += advance();
    }
    // Trim trailing whitespace from the literal
    size_t end = value.find_last_not_of(" \t\r\n");
    if (std::string::npos != end) {
        value = value.substr(0, end + 1);
    }
    return {TokenType::UNQUOTED_LITERAL, value, line, startCol};
}


Token Lexer::getNextToken() {
    // Combined whitespace and comment skipping
    while (peek() != '\0') {
        if (isspace(peek())) {
            skipWhitespace();
            continue;
        }
        if (peek() == '/') {
            if (source.substr(position, 2) == "//" || source.substr(position, 2) == "/*") {
                skipComment();
                continue;
            }
        }
        if (peek() == '-') {
            if (source.substr(position, 2) == "--") {
                skipComment();
                continue;
            }
        }
        break;
    }

    int startCol = column;
    char currentChar = peek();

    if (currentChar == '\0') {
        return {TokenType::END_OF_FILE, "", line, startCol};
    }

    // For text content, we want to group multiple "words" into one literal.
    // The parser will later determine the context, but the lexer can make a reasonable guess.
    // If we are not at a delimiter, we can assume it's a literal.
    // This is a bit of a hack to handle unquoted text blocks without a stateful lexer.
    // The parser can concatenate subsequent UNQUOTED_LITERAL/IDENTIFIER tokens if needed.
    if (currentChar != '"' && currentChar != '\'' && !strchr("{}[];:=,()@#.&><+*/-", currentChar) && !isalnum(currentChar) && currentChar != '_') {
         return readUnquotedLiteral();
    }

    // Now, handle single characters and identifiers
    currentChar = advance();

    switch (currentChar) {
        case '{': return {TokenType::L_BRACE, "{", line, startCol};
        case '}': return {TokenType::R_BRACE, "}", line, startCol};
        case '(': return {TokenType::L_PAREN, "(", line, startCol};
        case ')': return {TokenType::R_PAREN, ")", line, startCol};
        case '[': return {TokenType::L_BRACKET, "[", line, startCol};
        case ']': return {TokenType::R_BRACKET, "]", line, startCol};
        case ':': return {TokenType::COLON, ":", line, startCol};
        case ';': return {TokenType::SEMICOLON, ";", line, startCol};
        case '=': return {TokenType::EQUALS, "=", line, startCol};
        case ',': return {TokenType::COMMA, ",", line, startCol};
        case '@': return {TokenType::AT, "@", line, startCol};
        case '&': return {TokenType::AMPERSAND, "&", line, startCol};
        case '#': return {TokenType::HASH, "#", line, startCol};
        case '.': return {TokenType::DOT, ".", line, startCol};
        case '>': return {TokenType::GREATER_THAN, ">", line, startCol};
        case '<': return {TokenType::LESS_THAN, "<", line, startCol};
        case '?': return {TokenType::QUESTION, "?", line, startCol};
        case '+': return {TokenType::PLUS, "+", line, startCol};
        case '*': return {TokenType::ASTERISK, "*", line, startCol};
        case '/': return {TokenType::SLASH, "/", line, startCol};
        case '-': return {TokenType::MINUS, "-", line, startCol}; // Can be a standalone operator
        case '"':
        case '\'':
            position--; column--;
            return readString(currentChar);
        default:
            if (isalpha(currentChar) || currentChar == '_') {
                position--; column--;
                return readIdentifier();
            }
            // Fallback for anything else that wasn't caught
            position--; column--;
            return readUnquotedLiteral();
    }

    return {TokenType::ILLEGAL, std::string(1, currentChar), line, startCol};
}

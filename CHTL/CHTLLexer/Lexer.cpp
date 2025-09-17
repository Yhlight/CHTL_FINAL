#include "Lexer.h"
#include <cctype>
#include <map>

// Map of keywords to their corresponding token types
static const std::map<std::string, TokenType> keywords = {
    // "text" and "style" are now parsed as regular identifiers.
    // The parser will determine their meaning based on context.
};

Lexer::Lexer(const std::string& source) : source(source), position(0), readPosition(0), ch(0), line(1) {
    readChar(); // Initialize the lexer by reading the first character
}

// Reads the next character from the source and advances the position.
void Lexer::readChar() {
    if (readPosition >= source.length()) {
        ch = 0; // 0 is our EOF character
    } else {
        ch = source[readPosition];
    }
    position = readPosition;
    readPosition++;
    if (ch == '\n') {
        line++;
    }
}

// Peeks at the next character without consuming it.
char Lexer::peekChar() const {
    if (readPosition >= source.length()) {
        return 0;
    }
    return source[readPosition];
}

// Skips over any whitespace characters.
void Lexer::skipWhitespace() {
    while (isspace(ch)) {
        readChar();
    }
}

// Skips a single-line comment (e.g., // this is a comment)
void Lexer::skipSingleLineComment() {
    while (ch != '\n' && ch != 0) {
        readChar();
    }
}

// Skips a multi-line comment (e.g., /* ... */)
void Lexer::skipMultiLineComment() {
    readChar(); // consume the '*'
    while (true) {
        if (ch == 0) { // End of file inside a comment
            break;
        }
        if (ch == '*' && peekChar() == '/') {
            readChar(); // consume the '*'
            readChar(); // consume the '/'
            break;
        }
        readChar();
    }
}

// A helper to easily create a token.
Token Lexer::makeToken(TokenType type, const std::string& literal) {
    return Token{type, literal, line};
}

// Reads an identifier and checks if it's a keyword.
Token Lexer::readIdentifier() {
    size_t startPos = position;
    while (isalnum(ch) || ch == '_' || ch == '-') { // Allow hyphens in identifiers
        readChar();
    }
    std::string literal = source.substr(startPos, position - startPos);

    auto it = keywords.find(literal);
    if (it != keywords.end()) {
        return makeToken(it->second, literal);
    }
    return makeToken(TokenType::IDENTIFIER, literal);
}

// Reads a string literal enclosed in single or double quotes.
Token Lexer::readStringLiteral() {
    char quoteType = ch;
    size_t startPos = position + 1;
    readChar(); // Consume opening quote
    while (ch != quoteType && ch != 0) {
        readChar();
    }
    std::string literal = source.substr(startPos, position - startPos);
    readChar(); // Consume closing quote
    return makeToken(TokenType::STRING_LITERAL, literal);
}

// Reads a # comment. These are treated as tokens because they can be meaningful.
Token Lexer::readHashComment() {
    size_t startPos = position;
    readChar(); // Consume '#'
    if(isspace(ch)){ // The spec says "#" must be followed by a space
         while (ch != '\n' && ch != 0) {
            readChar();
        }
        std::string literal = source.substr(startPos, position - startPos);
        return makeToken(TokenType::HASH_COMMENT, literal);
    }
    // If not followed by space, it's not a valid generator comment.
    // For now, we can treat it as an illegal token.
    return makeToken(TokenType::ILLEGAL, "#");
}


// The main method to get the next token from the source.
Token Lexer::nextToken() {
    skipWhitespace();

    Token tok;

    switch (ch) {
        case '{': tok = makeToken(TokenType::LEFT_BRACE, "{"); readChar(); break;
        case '}': tok = makeToken(TokenType::RIGHT_BRACE, "}"); readChar(); break;
        case ':': tok = makeToken(TokenType::COLON, ":"); readChar(); break;
        case ';': tok = makeToken(TokenType::SEMICOLON, ";"); readChar(); break;
        case '=': tok = makeToken(TokenType::EQUALS, "="); readChar(); break;
        case '&': tok = makeToken(TokenType::AMPERSAND, "&"); readChar(); break;
        case '.': tok = makeToken(TokenType::DOT, "."); readChar(); break;
        case '@': tok = makeToken(TokenType::AT_SIGN, "@"); readChar(); break;
        case '[': tok = makeToken(TokenType::LEFT_BRACKET, "["); readChar(); break;
        case ']': tok = makeToken(TokenType::RIGHT_BRACKET, "]"); readChar(); break;
        case '(': tok = makeToken(TokenType::LEFT_PAREN, "("); readChar(); break;
        case ')': tok = makeToken(TokenType::RIGHT_PAREN, ")"); readChar(); break;
        case '"':
        case '\'':
            return readStringLiteral();
        case '#':
            if (isspace(peekChar())) {
                return readHashComment();
            } else {
                tok = makeToken(TokenType::POUND, "#");
                readChar();
            }
            break;
        case '/':
            if (peekChar() == '/') {
                readChar(); readChar(); // consume '//'
                skipSingleLineComment();
                return nextToken(); // Recursively call nextToken to get the token after the comment
            } else if (peekChar() == '*') {
                readChar(); readChar(); // consume '/*'
                skipMultiLineComment();
                return nextToken();
            } else {
                 tok = makeToken(TokenType::ILLEGAL, std::string(1, ch)); readChar();
            }
            break;
        case 0:
            tok = makeToken(TokenType::END_OF_FILE, "");
            break;
        default:
            // Allow identifiers (and unquoted literals) to start with letters, numbers, or underscore.
            if (isalnum(ch) || ch == '_') {
                // An unquoted literal looks the same as an identifier to the lexer.
                // The parser will use context to differentiate them.
                return readIdentifier();
            } else {
                tok = makeToken(TokenType::ILLEGAL, std::string(1, ch));
                readChar();
            }
            break;
    }

    return tok;
}

// Helper function to tokenize the entire source code at once.
std::vector<Token> Lexer::getAllTokens() {
    std::vector<Token> tokens;
    Token tok;
    do {
        tok = nextToken();
        tokens.push_back(tok);
    } while (tok.type != TokenType::END_OF_FILE);
    return tokens;
}

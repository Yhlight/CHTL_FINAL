package CHTL.CHTLLexer;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class CHTLLexer {
    private final String source;
    private final List<Token> tokens = new ArrayList<>();
    private int start = 0;
    private int current = 0;
    private int line = 1;
    private int lineStart = 0;

    private static final Map<String, TokenType> keywords;

    static {
        keywords = new HashMap<>();
        keywords.put("text", TokenType.TEXT);
        keywords.put("style", TokenType.STYLE);
        keywords.put("Template", TokenType.TEMPLATE);
        keywords.put("Custom", TokenType.CUSTOM);
        keywords.put("inherit", TokenType.INHERIT);
        keywords.put("delete", TokenType.DELETE);
        keywords.put("insert", TokenType.INSERT);
        keywords.put("after", TokenType.AFTER);
        keywords.put("before", TokenType.BEFORE);
        keywords.put("replace", TokenType.REPLACE);
        keywords.put("Import", TokenType.IMPORT);
        keywords.put("from", TokenType.FROM);
        keywords.put("as", TokenType.AS);
    }

    public CHTLLexer(String source) {
        this.source = source;
    }

    public List<Token> tokenize() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }

        tokens.add(new Token(TokenType.EOF, "", line, current - lineStart + 1));
        return tokens;
    }

    private void scanToken() {
        char c = advance();
        switch (c) {
            case '{': addToken(TokenType.LBRACE); break;
            case '}': addToken(TokenType.RBRACE); break;
            case '[': addToken(TokenType.LBRACKET); break;
            case ']': addToken(TokenType.RBRACKET); break;
            case ':': addToken(TokenType.COLON); break;
            case '=': addToken(TokenType.EQUALS); break;
            case ';': addToken(TokenType.SEMICOLON); break;
            case ',': addToken(TokenType.COMMA); break;
            case '.': addToken(TokenType.DOT); break;
            case '@': addToken(TokenType.AT); break;
            case '#': addToken(TokenType.HASH); break;
            case '&': addToken(TokenType.AMPERSAND); break;
            case '/':
                if (match('/')) {
                    // A single-line comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (match('*')) {
                    multilineComment();
                } else {
                    addToken(TokenType.SLASH);
                }
                break;
            case '-':
                if (match('-')) {
                    while (peek() != '\n' && !isAtEnd()) advance();
                    String value = source.substring(start + 2, current).trim();
                    addToken(TokenType.GENERATOR_COMMENT, value);
                } else {
                    // It could be part of an identifier or a negative number.
                    // Let's treat it as part of an identifier for now.
                    identifier();
                }
                break;

            // Ignore whitespace
            case ' ':
            case '\r':
            case '\t':
                break;

            case '\n':
                line++;
                lineStart = current;
                break;

            case '"': string('"'); break;
            case '\'': string('\''); break;

            default:
                if (isDigit(c)) {
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                }
                break;
        }
    }

    private void multilineComment() {
        while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
            if (peek() == '\n') {
                line++;
                lineStart = current + 1;
            }
            advance();
        }

        if (isAtEnd()) {
            // Unterminated comment error. For now, we just stop.
            return;
        }

        // Consume the "*/"
        advance();
        advance();
    }

    private void identifier() {
        while (isAlphaNumeric(peek())) advance();

        String text = source.substring(start, current);
        TokenType type = keywords.getOrDefault(text, TokenType.IDENTIFIER);
        addToken(type);
    }

    private void number() {
        while (isDigit(peek())) advance();

        if (peek() == '.' && isDigit(peekNext())) {
            advance();
            while (isDigit(peek())) advance();
        }

        // For values like '100px' or '100%', we consume the alphabetic part or '%'.
        while(isAlpha(peek()) || peek() == '%') advance();

        addToken(TokenType.NUMBER);
    }

    private void string(char quoteType) {
        while (peek() != quoteType && !isAtEnd()) {
            if (peek() == '\n') {
                line++;
                lineStart = current + 1;
            }
            advance();
        }

        if (isAtEnd()) {
            // Unterminated string error.
            return;
        }

        advance(); // The closing quote.
        String value = source.substring(start + 1, current - 1);
        addToken(TokenType.STRING, value);
    }

    private boolean match(char expected) {
        if (isAtEnd() || source.charAt(current) != expected) return false;
        current++;
        return true;
    }

    private char peek() {
        if (isAtEnd()) return '\0';
        return source.charAt(current);
    }

    private char peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source.charAt(current + 1);
    }

    private boolean isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    private boolean isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c) || c == '-';
    }

    private boolean isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    private boolean isAtEnd() {
        return current >= source.length();
    }

    private char advance() {
        current++;
        return source.charAt(current - 1);
    }

    private void addToken(TokenType type) {
        String text = source.substring(start, current);
        tokens.add(new Token(type, text, line, start - lineStart + 1));
    }

    private void addToken(TokenType type, String literal) {
        tokens.add(new Token(type, literal, line, start - lineStart + 1));
    }
}

package com.chtholly.chthl.lexer;

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

    private static final Map<String, TokenType> keywords;

    static {
        keywords = new HashMap<>();
        keywords.put("text", TokenType.TEXT);
        keywords.put("style", TokenType.STYLE);
        keywords.put("script", TokenType.SCRIPT);
        keywords.put("Template", TokenType.TEMPLATE);
        keywords.put("Custom", TokenType.CUSTOM);
        keywords.put("Origin", TokenType.ORIGIN);
        keywords.put("Import", TokenType.IMPORT);
        keywords.put("Namespace", TokenType.NAMESPACE);
        keywords.put("from", TokenType.FROM);
        keywords.put("as", TokenType.AS);
        keywords.put("use", TokenType.USE);
        keywords.put("except", TokenType.EXCEPT);
        keywords.put("delete", TokenType.DELETE);
        keywords.put("insert", TokenType.INSERT);
        keywords.put("set", TokenType.SET);
        keywords.put("after", TokenType.AFTER);
        keywords.put("before", TokenType.BEFORE);
        keywords.put("replace", TokenType.REPLACE);
        keywords.put("at", TokenType.AT);
        keywords.put("top", TokenType.TOP);
        keywords.put("bottom", TokenType.BOTTOM);
        keywords.put("into", TokenType.INTO);
        keywords.put("inherit", TokenType.INHERIT);
    }

    public CHTLLexer(String source) {
        this.source = source;
    }

    public List<Token> scanTokens() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }

        tokens.add(new Token(TokenType.EOF, "", null, line));
        return tokens;
    }

    private void scanToken() {
        char c = advance();
        switch (c) {
            case '{': addToken(TokenType.LEFT_BRACE); break;
            case '}': addToken(TokenType.RIGHT_BRACE); break;
            case '(': addToken(TokenType.LEFT_PAREN); break;
            case ')': addToken(TokenType.RIGHT_PAREN); break;
            case '[': addToken(TokenType.LEFT_BRACKET); break;
            case ']': addToken(TokenType.RIGHT_BRACKET); break;
            case ':': addToken(TokenType.COLON); break;
            case ';': addToken(TokenType.SEMICOLON); break;
            case '.': addToken(TokenType.DOT); break;
            case ',': addToken(TokenType.COMMA); break;
            case '+': addToken(TokenType.PLUS); break;
            case '%': addToken(TokenType.PERCENT); break;
            case '#': addToken(TokenType.HASH); break;
            case '?': addToken(TokenType.QUESTION); break;
            case '!':
                addToken(match('=') ? TokenType.BANG_EQUAL : TokenType.BANG);
                break;
            case '=': // Note: CHTL ':' and '=' are equivalent in some contexts, but not in expressions.
                addToken(match('=') ? TokenType.EQUAL_EQUAL : TokenType.EQUAL);
                break;
            case '<':
                addToken(match('=') ? TokenType.LESS_EQUAL : TokenType.LESS);
                break;
            case '>':
                addToken(match('=') ? TokenType.GREATER_EQUAL : TokenType.GREATER);
                break;
            case '&':
                addToken(match('&') ? TokenType.AMPERSAND_AMPERSAND : TokenType.AMPERSAND);
                break;
            case '|':
                addToken(match('|') ? TokenType.PIPE_PIPE : TokenType.PIPE);
                break;
            case '*':
                addToken(match('*') ? TokenType.STAR_STAR : TokenType.STAR);
                break;
            case '-':
                if (match('-')) {
                    // It's a generator comment. Consume the rest of the line.
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                    // The comment content is from after the '--' to the end of the line.
                    String commentText = source.substring(start + 2, current).trim();
                    addToken(TokenType.MINUS_MINUS, commentText);
                } else {
                    addToken(TokenType.MINUS);
                }
                break;
            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (match('*')) {
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') line++;
                        advance();
                    }
                    if (!isAtEnd()) {
                        advance();
                        advance();
                    }
                } else {
                    addToken(TokenType.SLASH);
                }
                break;

            case ' ':
            case '\r':
            case '\t':
                break;

            case '\n':
                line++;
                break;

            case '"':
            case '\'':
                string(c);
                break;

            default:
                if (isAlpha(c) || isDigit(c) || c == '@') {
                    identifier();
                } else {
                    System.err.println("Line " + line + ": Unexpected character '" + c + "'.");
                }
                break;
        }
    }

    private void identifier() {
        // Allow '@' at the start of an identifier for template types like @Style
        if (peek() == '@') {
            advance();
        }
        while (isAlphaNumeric(peek()) || peek() == '-') advance();

        String text = source.substring(start, current);

        TokenType type = keywords.get(text);
        if (type == null) type = TokenType.IDENTIFIER;
        addToken(type);
    }

    private void string(char quoteType) {
        while (peek() != quoteType && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();
        }

        if (isAtEnd()) {
            System.err.println("Line " + line + ": Unterminated string.");
            return;
        }

        advance();

        String value = source.substring(start + 1, current - 1);
        addToken(TokenType.STRING, value);
    }

    private boolean isAtEnd() {
        return current >= source.length();
    }

    private char advance() {
        current++;
        return source.charAt(current - 1);
    }

    private void addToken(TokenType type) {
        addToken(type, null);
    }

    private void addToken(TokenType type, Object literal) {
        String text = source.substring(start, current);
        tokens.add(new Token(type, text, literal, line));
    }

    private boolean match(char expected) {
        if (isAtEnd()) return false;
        if (source.charAt(current) != expected) return false;
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
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
                c == '_';
    }

    private boolean isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    private boolean isDigit(char c) {
        return c >= '0' && c <= '9';
    }
}

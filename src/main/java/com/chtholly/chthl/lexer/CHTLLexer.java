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
        keywords.put("after", TokenType.AFTER);
        keywords.put("before", TokenType.BEFORE);
        keywords.put("replace", TokenType.REPLACE);
        keywords.put("at", TokenType.AT);
        keywords.put("top", TokenType.TOP);
        keywords.put("bottom", TokenType.BOTTOM);
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
            case '=': addToken(TokenType.EQUAL); break;
            case '.': addToken(TokenType.DOT); break;
            case ',': addToken(TokenType.COMMA); break;
            case '+': addToken(TokenType.PLUS); break;
            case '%': addToken(TokenType.PERCENT); break;
            case '*':
                addToken(match('*') ? TokenType.STAR_STAR : TokenType.STAR);
                break;
            case '-':
                if (match('-')) {
                    // For now, just a token. Parser will give it meaning.
                    addToken(TokenType.MINUS_MINUS);
                } else {
                    addToken(TokenType.MINUS);
                }
                break;
            case '/':
                if (match('/')) {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (match('*')) {
                    // Multi-line comment.
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') line++;
                        advance();
                    }
                    if (isAtEnd()) {
                        // Handle unterminated comment error if necessary
                    } else {
                        advance(); // consume *
                        advance(); // consume /
                    }
                } else {
                    addToken(TokenType.SLASH);
                }
                break;

            // Ignore whitespace.
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
                if (isAlpha(c)) {
                    identifier();
                } else {
                    // For now, we'll log an error for unexpected characters.
                    // This can be improved later.
                    System.err.println("Line " + line + ": Unexpected character '" + c + "'.");
                }
                break;
        }
    }

    private void identifier() {
        while (isAlphaNumeric(peek())) advance();

        String text = source.substring(start, current);
        // Check for special bracketed keywords like [Template]
        if (text.equals("Template") && peek() == ']' && tokens.get(tokens.size()-1).getType() == TokenType.LEFT_BRACKET) {
            // This is a very basic check and might need to be more robust.
            // For now, we assume [Template] is a single unit.
            // The parser will handle the composition.
        }

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

        // Consume the closing quote.
        advance();

        // Get the string value without the quotes.
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

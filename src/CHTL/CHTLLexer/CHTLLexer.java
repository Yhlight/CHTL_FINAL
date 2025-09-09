package CHTL.CHTLLexer;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class CHTLLexer {
    private final String source;
    private final List<Token> tokens = new ArrayList<>();
    private final Map<String, TokenType> keywords;

    private int start = 0;
    private int current = 0;
    private int line = 1;
    private int lineStart = 0;

    private static final Map<String, TokenType> DEFAULT_KEYWORDS;

    static {
        DEFAULT_KEYWORDS = new HashMap<>();
        DEFAULT_KEYWORDS.put("text", TokenType.TEXT);
        DEFAULT_KEYWORDS.put("style", TokenType.STYLE);
        DEFAULT_KEYWORDS.put("Template", TokenType.TEMPLATE);
        DEFAULT_KEYWORDS.put("Custom", TokenType.CUSTOM);
        DEFAULT_KEYWORDS.put("Import", TokenType.IMPORT);
        DEFAULT_KEYWORDS.put("Origin", TokenType.ORIGIN);
        DEFAULT_KEYWORDS.put("Configuration", TokenType.CONFIGURATION);
        DEFAULT_KEYWORDS.put("use", TokenType.USE);
        DEFAULT_KEYWORDS.put("inherit", TokenType.INHERIT);
        DEFAULT_KEYWORDS.put("delete", TokenType.DELETE);
        DEFAULT_KEYWORDS.put("insert", TokenType.INSERT);
        DEFAULT_KEYWORDS.put("after", TokenType.AFTER);
        DEFAULT_KEYWORDS.put("before", TokenType.BEFORE);
        DEFAULT_KEYWORDS.put("replace", TokenType.REPLACE);
        DEFAULT_KEYWORDS.put("from", TokenType.FROM);
        DEFAULT_KEYWORDS.put("as", TokenType.AS);
        DEFAULT_KEYWORDS.put("Name", TokenType.NAME);
    }

    /**
     * Constructor for using a dynamic set of keywords.
     * @param source The source code to tokenize.
     * @param keywords The map of keyword strings to token types.
     */
    public CHTLLexer(String source, Map<String, TokenType> keywords) {
        this.source = source;
        this.keywords = keywords;
    }

    /**
     * Constructor for using the default set of keywords.
     * @param source The source code to tokenize.
     */
    public CHTLLexer(String source) {
        this(source, DEFAULT_KEYWORDS);
    }

    public static Map<String, TokenType> getDefaultKeywords() {
        return new HashMap<>(DEFAULT_KEYWORDS);
    }

    public List<Token> tokenize() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }
        tokens.add(new Token(TokenType.EOF, "", line, current - lineStart + 1, current, current));
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
                    addToken(TokenType.GENERATOR_COMMENT);
                } else {
                    identifier();
                }
                break;
            case ' ': case '\r': case '\t': break;
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
        if (isAtEnd()) return;
        advance();
        advance();
    }

    private void identifier() {
        while (isAlphaNumeric(peek())) advance();
        String text = source.substring(start, current);
        TokenType type = this.keywords.getOrDefault(text, TokenType.IDENTIFIER);
        addToken(type);
    }

    private void number() {
        while (isDigit(peek())) advance();
        if (peek() == '.' && isDigit(peekNext())) {
            advance();
            while (isDigit(peek())) advance();
        }
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
        if (isAtEnd()) return;
        advance();
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
        addToken(type, source.substring(start, current));
    }

    private void addToken(TokenType type, String value) {
        tokens.add(new Token(type, value, line, start - lineStart + 1, start, current));
    }
}

package CHTL_JS.Lexer;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;

public class CHTLJSLexer {
    private final String input;
    private int position = 0;
    private static final Map<String, TokenType> KEYWORDS = new HashMap<>();

    static {
        KEYWORDS.put("listen", TokenType.LISTEN);
        KEYWORDS.put("animate", TokenType.ANIMATE);
        KEYWORDS.put("delegate", TokenType.DELEGATE);
        KEYWORDS.put("target", TokenType.TARGET);
        KEYWORDS.put("router", TokenType.ROUTER);
    }

    public CHTLJSLexer(String input) {
        this.input = input;
    }

    public List<Token> tokenize() {
        List<Token> tokens = new ArrayList<>();
        while (position < input.length()) {
            char current = peek();
            if (Character.isWhitespace(current)) {
                advance();
                continue;
            }
            if (current == '{' && lookahead() == '{') {
                tokens.add(new Token(TokenType.L_CURLY_BRACE, "{{"));
                advance(); advance();
                tokens.add(selector());
                if (peek() == '}' && lookahead() == '}') {
                    tokens.add(new Token(TokenType.R_CURLY_BRACE, "}}"));
                    advance(); advance();
                } else {
                    throw new RuntimeException("Unclosed {{ selector.");
                }
            } else if (Character.isLetter(current)) {
                tokens.add(identifier());
            } else if (current == '-' && lookahead() == '>') {
                tokens.add(new Token(TokenType.ARROW, "->"));
                advance(); advance();
            } else if (current == '{') {
                 tokens.add(new Token(TokenType.LBRACE, "{")); advance();
            } else if (current == '}') {
                 tokens.add(new Token(TokenType.RBRACE, "}")); advance();
            } else if (current == ':') {
                 tokens.add(new Token(TokenType.COLON, ":")); advance();
            } else if (current == ';') {
                 tokens.add(new Token(TokenType.SEMICOLON, ";")); advance();
            } else if (current == ',') {
                 tokens.add(new Token(TokenType.COMMA, ",")); advance();
            } else {
                // For now, we'll just advance past unknown characters.
                // A real implementation would handle JS code blocks more gracefully.
                advance();
            }
        }
        tokens.add(new Token(TokenType.EOF, ""));
        return tokens;
    }

    private Token selector() {
        int start = position;
        while (position < input.length() && !(peek() == '}' && lookahead() == '}')) {
            advance();
        }
        return new Token(TokenType.SELECTOR, input.substring(start, position).trim());
    }

    private Token identifier() {
        int start = position;
        while (position < input.length() && Character.isLetterOrDigit(peek())) {
            advance();
        }
        String text = input.substring(start, position);
        TokenType type = KEYWORDS.getOrDefault(text, TokenType.IDENTIFIER);
        return new Token(type, text);
    }

    private char peek() {
        if (position >= input.length()) return '\0';
        return input.charAt(position);
    }

    private char lookahead() {
        if (position + 1 >= input.length()) return '\0';
        return input.charAt(position + 1);
    }

    private void advance() {
        position++;
    }
}

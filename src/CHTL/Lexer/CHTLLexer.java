package CHTL.Lexer;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;

public class CHTLLexer {
    private final String input;
    private int position = 0;
    private static final Map<String, TokenType> KEYWORDS = new HashMap<>();

    static {
        KEYWORDS.put("text", TokenType.TEXT);
        KEYWORDS.put("style", TokenType.STYLE);
        KEYWORDS.put("script", TokenType.SCRIPT);
    }

    public CHTLLexer(String input) {
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
            if (Character.isLetter(current)) {
                tokens.add(identifier());
            } else if (current == '{') {
                tokens.add(new Token(TokenType.LBRACE, "{")); advance();
            } else if (current == '}') {
                tokens.add(new Token(TokenType.RBRACE, "}")); advance();
            } else if (current == ':') {
                tokens.add(new Token(TokenType.COLON, ":")); advance();
            } else if (current == '=') {
                tokens.add(new Token(TokenType.EQUALS, "=")); advance();
            } else if (current == ';') {
                tokens.add(new Token(TokenType.SEMICOLON, ";")); advance();
            } else if (current == '"' || current == '\'') {
                tokens.add(string(current));
            } else {
                 // For simplicity, treat unquoted values as identifiers/strings
                 tokens.add(unquotedString());
            }
        }
        tokens.add(new Token(TokenType.EOF, ""));
        return tokens;
    }

    private Token identifier() {
        int start = position;
        while (position < input.length() && (Character.isLetterOrDigit(peek()) || peek() == '-')) {
            advance();
        }
        String text = input.substring(start, position);
        TokenType type = KEYWORDS.getOrDefault(text, TokenType.IDENTIFIER);
        return new Token(type, text);
    }

    private Token unquotedString() {
        int start = position;
        // Consume until a semicolon or brace, which typically terminate a property value
        while (position < input.length() && peek() != ';' && peek() != '{' && peek() != '}') {
            advance();
        }
        return new Token(TokenType.STRING, input.substring(start, position).trim());
    }

    private Token string(char quoteType) {
        int start = position;
        advance(); // Consume opening quote
        while (position < input.length() && peek() != quoteType) {
            advance();
        }
        if (position < input.length()) {
             advance(); // Consume closing quote
        }
        return new Token(TokenType.STRING, input.substring(start + 1, position - 1));
    }

    private char peek() {
        if (position >= input.length()) return '\0';
        return input.charAt(position);
    }

    private void advance() {
        position++;
    }
}

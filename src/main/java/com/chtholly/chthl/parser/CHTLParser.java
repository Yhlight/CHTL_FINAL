package com.chtholly.chthl.parser;

import com.chtholly.chthl.ast.*;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * A recursive descent parser for the CHTL language.
 * It consumes a stream of tokens from the CHTLLexer and produces an Abstract Syntax Tree (AST).
 */
public class CHTLParser {

    private static class ParseError extends RuntimeException {
        ParseError(Token token, String message) {
            super("[line " + token.getLine() + "] Error at '" + token.getLexeme() + "': " + message);
        }
    }

    private final List<Token> tokens;
    private int current = 0;

    public CHTLParser(List<Token> tokens) {
        this.tokens = tokens;
    }

    public List<Node> parse() {
        List<Node> nodes = new ArrayList<>();
        while (!isAtEnd()) {
            Node node = declaration();
            if (node != null) {
                nodes.add(node);
            }
        }
        return nodes;
    }

    private Node declaration() {
        try {
            if (peek().getType() == TokenType.TEXT) {
                return textDeclaration();
            }
            if (peek().getType() == TokenType.IDENTIFIER) {
                return elementDeclaration();
            }
            throw new ParseError(peek(), "Expected a declaration.");
        } catch (ParseError error) {
            System.err.println(error.getMessage());
            synchronize();
            return null;
        }
    }

    private Node elementDeclaration() {
        Token name = consume(TokenType.IDENTIFIER, "Expect element name.");
        consume(TokenType.LEFT_BRACE, "Expect '{' after element name.");

        List<Node> children = new ArrayList<>();
        Map<String, String> attributes = new HashMap<>();

        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType.IDENTIFIER) && peekNext().getType() == TokenType.COLON) {
                attribute(attributes);
            } else if (check(TokenType.STYLE)) {
                children.add(styleBlock());
            } else {
                Node child = declaration();
                if (child != null) {
                    children.add(child);
                }
            }
        }

        consume(TokenType.RIGHT_BRACE, "Expect '}' after element block.");
        return new ElementNode(name.getLexeme(), attributes, children);
    }

    private void attribute(Map<String, String> attributes) {
        Token name = consume(TokenType.IDENTIFIER, "Expect attribute name.");
        consume(TokenType.COLON, "Expect ':' after attribute name.");

        Token valueToken;
        if (match(TokenType.STRING)) {
            valueToken = previous();
        } else if (match(TokenType.IDENTIFIER)) {
            valueToken = previous();
        } else {
            throw new ParseError(peek(), "Expect attribute value (string or identifier).");
        }

        String value = (valueToken.getLiteral() != null)
                ? valueToken.getLiteral().toString()
                : valueToken.getLexeme();

        attributes.put(name.getLexeme(), value);

        consume(TokenType.SEMICOLON, "Expect ';' after attribute value.");
    }

    private Node styleBlock() {
        consume(TokenType.STYLE, "Expect 'style' keyword.");
        consume(TokenType.LEFT_BRACE, "Expect '{' after 'style'.");

        List<StylePropertyNode> directProperties = new ArrayList<>();
        List<SelectorBlockNode> selectorBlocks = new ArrayList<>();

        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType.IDENTIFIER) && peekNext().getType() == TokenType.COLON) {
                directProperties.add(parseStyleProperty());
            } else {
                selectorBlocks.add(parseSelectorBlock());
            }
        }

        consume(TokenType.RIGHT_BRACE, "Expect '}' after style block.");
        return new StyleBlockNode(directProperties, selectorBlocks);
    }

    private StylePropertyNode parseStyleProperty() {
        Token name = consume(TokenType.IDENTIFIER, "Expect style property name.");
        consume(TokenType.COLON, "Expect ':' after style property name.");

        StringBuilder valueBuilder = new StringBuilder();
        while (!check(TokenType.SEMICOLON) && !isAtEnd()) {
            if (valueBuilder.length() > 0) {
                valueBuilder.append(" ");
            }
            valueBuilder.append(advance().getLexeme());
        }

        if (valueBuilder.length() == 0) {
            throw new ParseError(peek(), "Style property value cannot be empty.");
        }

        consume(TokenType.SEMICOLON, "Expect ';' after style property value.");
        return new StylePropertyNode(name.getLexeme(), valueBuilder.toString().trim());
    }

    private SelectorBlockNode parseSelectorBlock() {
        StringBuilder selectorBuilder = new StringBuilder();
        while (!check(TokenType.LEFT_BRACE) && !isAtEnd()) {
            selectorBuilder.append(advance().getLexeme());
        }

        String selector = selectorBuilder.toString().trim();
        if (selector.isEmpty()) {
            throw new ParseError(peek(), "Selector cannot be empty.");
        }

        consume(TokenType.LEFT_BRACE, "Expect '{' after selector.");

        List<StylePropertyNode> properties = new ArrayList<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            properties.add(parseStyleProperty());
        }

        consume(TokenType.RIGHT_BRACE, "Expect '}' after selector block.");
        return new SelectorBlockNode(selector, properties);
    }

    private Node textDeclaration() {
        consume(TokenType.TEXT, "Expect 'text' keyword.");
        consume(TokenType.LEFT_BRACE, "Expect '{' after 'text' keyword.");
        Token content = consume(TokenType.STRING, "Expect a string literal inside a text block.");
        consume(TokenType.RIGHT_BRACE, "Expect '}' after text block.");
        return new TextNode((String) content.getLiteral());
    }

    private boolean match(TokenType... types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    private Token consume(TokenType type, String message) {
        if (check(type)) return advance();
        throw new ParseError(peek(), message);
    }

    private boolean check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().getType() == type;
    }

    private Token peekNext() {
        if (current + 1 >= tokens.size()) {
            return tokens.get(tokens.size() - 1);
        }
        return tokens.get(current + 1);
    }

    private Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    private boolean isAtEnd() {
        return peek().getType() == TokenType.EOF;
    }

    private Token peek() {
        return tokens.get(current);
    }

    private Token previous() {
        return tokens.get(current - 1);
    }

    private void synchronize() {
        advance();
        while (!isAtEnd()) {
            if (previous().getType() == TokenType.RIGHT_BRACE) return;
            switch (peek().getType()) {
                case IDENTIFIER:
                case TEXT:
                case STYLE:
                    return;
            }
            advance();
        }
    }
}

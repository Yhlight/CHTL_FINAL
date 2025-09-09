package com.chtholly.chthl.parser;

import com.chtholly.chthl.ast.*;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * A recursive descent parser for the CHTL language.
 * It consumes a stream of tokens from the CHTLLexer and produces an Abstract Syntax Tree (AST).
 */
public class CHTLParser {

    private static class ParseError extends RuntimeException {
        ParseError(Token token, String message) {
            // In a real compiler, you'd have more sophisticated error reporting.
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
            nodes.add(declaration());
        }
        return nodes;
    }

    private Node declaration() {
        try {
            if (match(TokenType.TEXT)) {
                return textDeclaration();
            }
            if (match(TokenType.IDENTIFIER)) {
                return elementDeclaration();
            }
            // If we're here, it's a token we don't recognize at the top level yet.
            // We'll skip it to avoid getting stuck in a loop on errors.
            throw new ParseError(peek(), "Expected an element or text declaration.");
        } catch (ParseError error) {
            // For now, print the error and synchronize to the next statement.
            System.err.println(error.getMessage());
            synchronize();
            return null; // Return null for the node that failed to parse.
        }
    }

    private Node elementDeclaration() {
        Token name = previous(); // The IDENTIFIER token
        consume(TokenType.LEFT_BRACE, "Expect '{' after element name.");

        List<Node> children = new ArrayList<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            children.add(declaration());
        }

        consume(TokenType.RIGHT_BRACE, "Expect '}' after element block.");
        // For now, attributes are an empty map. We'll add attribute parsing later.
        return new ElementNode(name.getLexeme(), new HashMap<>(), children);
    }

    private Node textDeclaration() {
        // Assumes 'text' token was already consumed by match().
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
                    return;
            }
            advance();
        }
    }
}

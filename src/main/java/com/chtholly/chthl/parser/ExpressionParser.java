package com.chtholly.chthl.parser;

import com.chtholly.chthl.ast.expr.*;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;

import java.util.List;

/**
 * Parses a list of tokens into an Expression AST.
 * Handles operator precedence for binary operations.
 */
public class ExpressionParser {

    private static class ParseError extends RuntimeException {
        ParseError(Token token, String message) {
            super("[line " + token.getLine() + "] Error at '" + token.getLexeme() + "': " + message);
        }
    }

    private final List<Token> tokens;
    private int current = 0;

    public ExpressionParser(List<Token> tokens) {
        this.tokens = tokens;
    }

    public Expression parse() {
        try {
            return expression();
        } catch (ParseError error) {
            // In a real compiler, we might have better error recovery.
            System.err.println(error.getMessage());
            return null;
        }
    }

    private Expression expression() {
        return term();
    }

    private Expression term() {
        Expression expr = factor();
        while (match(TokenType.MINUS, TokenType.PLUS)) {
            Token operator = previous();
            Expression right = factor();
            expr = new BinaryExpr(expr, operator, right);
        }
        return expr;
    }

    private Expression factor() {
        Expression expr = primary();
        while (match(TokenType.SLASH, TokenType.STAR)) {
            Token operator = previous();
            Expression right = primary();
            expr = new BinaryExpr(expr, operator, right);
        }
        return expr;
    }

    private Expression primary() {
        if (match(TokenType.IDENTIFIER)) {
            // Both variables (`width`) and literals with units (`100px`) are tokenized as IDENTIFIER.
            // We treat them all as VariableExpr at the parsing stage.
            // The evaluator will be responsible for interpreting their semantic meaning.
            return new VariableExpr(previous());
        }

        if (match(TokenType.STRING)) {
            return new LiteralExpr(previous().getLiteral());
        }

        if (match(TokenType.LEFT_PAREN)) {
            Expression expr = expression();
            consume(TokenType.RIGHT_PAREN, "Expect ')' after expression.");
            return new GroupingExpr(expr);
        }

        throw new ParseError(peek(), "Expect expression.");
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

    private boolean isAtEnd() {
        // This parser works on a sub-list of tokens, so the end is the list size.
        return current >= tokens.size();
    }

    private Token peek() {
        if (isAtEnd()) {
            // Return the last token to avoid IndexOutOfBounds, its type won't match anything.
            return tokens.get(tokens.size() - 1);
        }
        return tokens.get(current);
    }

    private Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    private Token previous() {
        return tokens.get(current - 1);
    }
}

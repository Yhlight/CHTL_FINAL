package com.chtholly.chthl.parser;

import com.chtholly.chthl.ast.expr.*;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;

import java.util.ArrayList;
import java.util.List;

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
            System.err.println(error.getMessage());
            return null;
        }
    }

    private Expression expression() {
        return conditional();
    }

    private Expression conditional() {
        Expression expr = logic_or();
        if (match(TokenType.QUESTION)) {
            Expression thenBranch = expression();
            consume(TokenType.COLON, "Expect ':' after then branch.");
            Expression elseBranch = conditional();
            expr = new ConditionalExpr(expr, thenBranch, elseBranch);
        }
        return expr;
    }

    private Expression logic_or() {
        Expression expr = logic_and();
        while (match(TokenType.PIPE_PIPE)) {
            Token operator = previous();
            Expression right = logic_and();
            expr = new BinaryExpr(expr, operator, right);
        }
        return expr;
    }

    private Expression logic_and() {
        Expression expr = equality();
        while (match(TokenType.AMPERSAND_AMPERSAND)) {
            Token operator = previous();
            Expression right = equality();
            expr = new BinaryExpr(expr, operator, right);
        }
        return expr;
    }

    private Expression equality() {
        Expression expr = comparison();
        while (match(TokenType.BANG_EQUAL, TokenType.EQUAL_EQUAL)) {
            Token operator = previous();
            Expression right = comparison();
            expr = new BinaryExpr(expr, operator, right);
        }
        return expr;
    }

    private Expression comparison() {
        Expression expr = term();
        while (match(TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL)) {
            Token operator = previous();
            Expression right = term();
            expr = new BinaryExpr(expr, operator, right);
        }
        return expr;
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
        Expression expr = call();
        while (match(TokenType.SLASH, TokenType.STAR)) {
            Token operator = previous();
            Expression right = call();
            expr = new BinaryExpr(expr, operator, right);
        }
        return expr;
    }

    private Expression call() {
        Expression expr = primary();
        while (true) {
            if (match(TokenType.LEFT_PAREN)) {
                expr = finishCall(expr);
            } else if (match(TokenType.DOT)) {
                Token name = consume(TokenType.IDENTIFIER, "Expect property name after '.'.");
                expr = new ReferenceExpr(expr, name);
            } else {
                break;
            }
        }
        return expr;
    }

    private Expression finishCall(Expression callee) {
        List<Expression> arguments = new ArrayList<>();
        if (!check(TokenType.RIGHT_PAREN)) {
            do {
                arguments.add(expression());
            } while (match(TokenType.COMMA));
        }
        Token paren = consume(TokenType.RIGHT_PAREN, "Expect ')' after arguments.");
        return new CallExpr(callee, paren, arguments);
    }

    private Expression primary() {
        if (match(TokenType.AMPERSAND)) {
            Token name = consume(TokenType.IDENTIFIER, "Expect variable name after '&'.");
            return new GlobalVariableExpr(name);
        }
        if (match(TokenType.STRING)) {
            return new LiteralExpr(previous().getLiteral());
        }
        if (match(TokenType.DOT, TokenType.HASH)) {
            Token prefix = previous();
            Token name = consume(TokenType.IDENTIFIER, "Expect name after '.' or '#'.");
            Token selectorToken = new Token(TokenType.IDENTIFIER, prefix.getLexeme() + name.getLexeme(), null, prefix.getLine());
            return new VariableExpr(selectorToken);
        }
        if (match(TokenType.IDENTIFIER)) {
            return new VariableExpr(previous());
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
        return current >= tokens.size();
    }

    private Token peek() {
        if (isAtEnd()) {
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

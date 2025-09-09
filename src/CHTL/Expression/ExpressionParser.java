package CHTL.Expression;

import java.util.List;

public class ExpressionParser {
    private final List<Token> tokens;
    private int current = 0;

    public ExpressionParser(List<Token> tokens) {
        this.tokens = tokens;
    }

    public ExpressionNode parse() {
        return ternary();
    }

    // Precedence: Ternary Conditional (?:)
    private ExpressionNode ternary() {
        ExpressionNode condition = logicalOr();
        if (match(ExpressionTokenType.QUESTION)) {
            ExpressionNode trueExpr = ternary();
            ExpressionNode falseExpr = null;
            if (match(ExpressionTokenType.COLON)) {
                falseExpr = ternary();
            }
            return new TernaryNode(condition, trueExpr, falseExpr);
        }
        return condition;
    }

    // Precedence: Logical OR (||)
    private ExpressionNode logicalOr() {
        ExpressionNode expr = logicalAnd();
        while (match(ExpressionTokenType.OR)) {
            String operator = previous().value();
            ExpressionNode right = logicalAnd();
            expr = new BinaryOpNode(expr, operator, right);
        }
        return expr;
    }

    // Precedence: Logical AND (&&)
    private ExpressionNode logicalAnd() {
        ExpressionNode expr = comparison();
        while (match(ExpressionTokenType.AND)) {
            String operator = previous().value();
            ExpressionNode right = comparison();
            expr = new BinaryOpNode(expr, operator, right);
        }
        return expr;
    }

    // Precedence: Comparison (>, >=, <, <=)
    private ExpressionNode comparison() {
        ExpressionNode expr = addition();
        while (match(ExpressionTokenType.GREATER, ExpressionTokenType.GREATER_EQUAL, ExpressionTokenType.LESS, ExpressionTokenType.LESS_EQUAL)) {
            String operator = previous().value();
            ExpressionNode right = addition();
            expr = new BinaryOpNode(expr, operator, right);
        }
        return expr;
    }

    // Precedence: Addition/Subtraction (+, -)
    private ExpressionNode addition() {
        ExpressionNode expr = multiplication();
        while (match(ExpressionTokenType.PLUS, ExpressionTokenType.MINUS)) {
            String operator = previous().value();
            ExpressionNode right = multiplication();
            expr = new BinaryOpNode(expr, operator, right);
        }
        return expr;
    }

    // Precedence: Multiplication/Division/Modulo (*, /, %)
    private ExpressionNode multiplication() {
        ExpressionNode expr = power();
        while (match(ExpressionTokenType.STAR, ExpressionTokenType.SLASH, ExpressionTokenType.PERCENT)) {
            String operator = previous().value();
            ExpressionNode right = power();
            expr = new BinaryOpNode(expr, operator, right);
        }
        return expr;
    }

    // Precedence: Power (**)
    private ExpressionNode power() {
        ExpressionNode expr = primary();
        while (match(ExpressionTokenType.POWER)) {
            String operator = previous().value();
            ExpressionNode right = primary();
            expr = new BinaryOpNode(expr, operator, right);
        }
        return expr;
    }

    // Precedence: Primary (Literals, References, Grouping)
    private ExpressionNode primary() {
        if (match(ExpressionTokenType.NUMBER)) {
            double value = Double.parseDouble(previous().value());
            // Check for a unit
            if (match(ExpressionTokenType.IDENTIFIER)) {
                return new UnitNode(value, previous().value());
            }
            return new LiteralNode(value);
        }

        if (match(ExpressionTokenType.STRING)) {
            return new LiteralNode(previous().value());
        }

        if (match(ExpressionTokenType.SELECTOR)) {
            String selector = previous().value();
            if (match(ExpressionTokenType.DOT)) {
                Token property = advance(); // Assume IDENTIFIER
                return new ReferenceNode(selector, property.value());
            }
            // It might be a reference to the element itself, e.g. `box.width`
            // The lexer needs to be smarter to handle this. For now, we assume simple cases.
        }

        if (match(ExpressionTokenType.IDENTIFIER)) {
             // Could be a reference to own property e.g. `width`
             // This requires context the parser doesn't have. The evaluator will handle it.
             // For now, treat it as a reference with no selector.
             return new ReferenceNode(null, previous().value());
        }

        if (match(ExpressionTokenType.LPAREN)) {
            ExpressionNode expr = expression();
            if (!match(ExpressionTokenType.RPAREN)) {
                throw new RuntimeException("Expect ')' after expression.");
            }
            return expr;
        }

        throw new RuntimeException("Expect expression.");
    }

    private Token peek() { return tokens.get(current); }
    private Token previous() { return tokens.get(current - 1); }
    private boolean isAtEnd() { return peek().type() == ExpressionTokenType.EOF; }
    private boolean check(ExpressionTokenType type) { if (isAtEnd()) return false; return peek().type() == type; }
    private Token advance() { if (!isAtEnd()) current++; return previous(); }
    private boolean match(ExpressionTokenType... types) {
        for (ExpressionTokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }
}

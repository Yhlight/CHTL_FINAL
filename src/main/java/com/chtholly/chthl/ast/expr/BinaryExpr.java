package com.chtholly.chthl.ast.expr;

import com.chtholly.chthl.lexer.Token;

/**
 * Represents a binary operation in an expression, e.g., <code>100px + 50px</code>.
 */
public class BinaryExpr extends Expression {
    public final Expression left;
    public final Token operator;
    public final Expression right;

    public BinaryExpr(Expression left, Token operator, Expression right) {
        this.left = left;
        this.operator = operator;
        this.right = right;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitBinaryExpr(this);
    }

    @Override
    public Expression clone() {
        return new BinaryExpr(this.left.clone(), this.operator, this.right.clone());
    }
}

package com.chtholly.chthl.ast.expr;

/**
 * Represents a conditional (ternary) expression, e.g., <code>width > 50px ? 'red' : 'blue'</code>.
 */
public class ConditionalExpr extends Expression {
    public final Expression condition;
    public final Expression thenBranch;
    public final Expression elseBranch;

    public ConditionalExpr(Expression condition, Expression thenBranch, Expression elseBranch) {
        this.condition = condition;
        this.thenBranch = thenBranch;
        this.elseBranch = elseBranch;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitConditionalExpr(this);
    }
}

package com.chtholly.chthl.ast.expr;

/**
 * Represents a grouping in an expression, e.g., <code>(10px + 5px)</code>.
 */
public class GroupingExpr extends Expression {
    public final Expression expression;

    public GroupingExpr(Expression expression) {
        this.expression = expression;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitGroupingExpr(this);
    }
}

package com.chtholly.chthl.ast.expr;

/**
 * Represents a literal value in an expression, e.g., <code>100px</code> or <code>"red"</code>.
 */
public class LiteralExpr extends Expression {
    public final Object value;

    public LiteralExpr(Object value) {
        this.value = value;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitLiteralExpr(this);
    }
}

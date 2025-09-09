package com.chtholly.chthl.ast.expr;

import com.chtholly.chthl.lexer.Token;

/**
 * Represents a property access on an object or selector, e.g., <code>box.width</code>.
 * This is a more general representation than just for CSS selectors.
 */
public class ReferenceExpr extends Expression {
    public final Expression object;
    public final Token property;

    public ReferenceExpr(Expression object, Token property) {
        this.object = object;
        this.property = property;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitReferenceExpr(this);
    }
}

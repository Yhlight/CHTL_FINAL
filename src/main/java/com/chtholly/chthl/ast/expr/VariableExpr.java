package com.chtholly.chthl.ast.expr;

import com.chtholly.chthl.lexer.Token;

/**
 * Represents a variable or property reference in an expression, e.g., the `width` in <code>height: width</code>.
 */
public class VariableExpr extends Expression {
    public final Token name;

    public VariableExpr(Token name) {
        this.name = name;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitVariableExpr(this);
    }

    @Override
    public Expression clone() {
        return new VariableExpr(this.name);
    }
}

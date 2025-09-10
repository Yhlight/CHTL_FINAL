package com.chtholly.chthl.ast.expr;

import com.chtholly.chthl.lexer.Token;

public class GlobalVariableExpr extends Expression {
    public final Token name;

    public GlobalVariableExpr(Token name) {
        this.name = name;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitGlobalVariableExpr(this);
    }

    @Override
    public Expression clone() {
        return new GlobalVariableExpr(name);
    }
}

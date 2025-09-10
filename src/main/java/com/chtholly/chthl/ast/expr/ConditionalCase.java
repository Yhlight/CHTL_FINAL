package com.chtholly.chthl.ast.expr;

public class ConditionalCase {
    public final Expression condition;
    public final Expression thenBranch;

    public ConditionalCase(Expression condition, Expression thenBranch) {
        this.condition = condition;
        this.thenBranch = thenBranch;
    }

    public ConditionalCase clone() {
        return new ConditionalCase(condition.clone(), thenBranch.clone());
    }
}

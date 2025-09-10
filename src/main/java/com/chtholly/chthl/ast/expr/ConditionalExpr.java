package com.chtholly.chthl.ast.expr;

import java.util.List;
import java.util.stream.Collectors;

/**
 * Represents a chain of conditional expressions.
 * e.g., <code>cond1 ? then1, cond2 ? then2 : elseBranch</code>.
 */
public class ConditionalExpr extends Expression {
    public final List<ConditionalCase> cases;
    public final Expression elseBranch; // Can be null

    public ConditionalExpr(List<ConditionalCase> cases, Expression elseBranch) {
        this.cases = cases;
        this.elseBranch = elseBranch;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitConditionalExpr(this);
    }

    @Override
    public Expression clone() {
        List<ConditionalCase> clonedCases = cases.stream().map(ConditionalCase::clone).collect(Collectors.toList());
        Expression clonedElse = (elseBranch != null) ? elseBranch.clone() : null;
        return new ConditionalExpr(clonedCases, clonedElse);
    }
}

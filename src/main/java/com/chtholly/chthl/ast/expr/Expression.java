package com.chtholly.chthl.ast.expr;

/**
 * The base class for all expression nodes in the AST.
 */
public abstract class Expression {
    /**
     * Accepts a visitor, implementing the visitor design pattern.
     * @param visitor The visitor to accept.
     * @param <R> The return type of the visitor's operation.
     * @return The result of the visitor's operation.
     */
    public abstract <R> R accept(Visitor<R> visitor);
}

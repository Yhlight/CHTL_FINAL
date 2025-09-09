package com.chtholly.chthl.ast;

/**
 * The base interface for all nodes in the Abstract Syntax Tree (AST).
 */
public interface Node {
    /**
     * Accepts a visitor, implementing the visitor design pattern.
     * @param visitor The visitor to accept.
     * @param <R> The return type of the visitor's operation.
     * @return The result of the visitor's operation.
     */
    <R> R accept(Visitor<R> visitor);
}

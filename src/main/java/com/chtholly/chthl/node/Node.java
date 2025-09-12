package com.chtholly.chthl.node;

/**
 * The base interface for all nodes in the Abstract Syntax Tree (AST).
 */
import com.chtholly.chthl.generator.Visitor;

public interface Node {

    /**
     * Returns the literal value of the token associated with this node.
     * Useful for debugging and error reporting.
     *
     * @return The string literal from the source code.
     */
    String tokenLiteral();

    /**
     * A utility method for debugging that provides a string representation of the node.
     *
     * @return A string representation of the AST node.
     */
    String toString();

    /**
     * Accepts a visitor, implementing the visitor design pattern.
     * @param visitor The visitor to accept.
     * @return The result of the visit.
     * @param <T> The return type of the visitor.
     */
    <T> T accept(Visitor<T> visitor);
}

package CHTL.CHTLNode;

/**
 * The base interface for all nodes in the CHTL Abstract Syntax Tree (AST).
 * This serves as a common type for all structural elements of the code.
 * The Visitor pattern will be added later for operations like code generation.
 */
public interface BaseNode {
    /**
     * Accepts a visitor, implementing the visitor design pattern.
     * @param visitor The visitor to accept.
     * @param <R> The return type of the visitor's methods.
     * @return The result of the visitor's processing.
     */
    <R> R accept(Visitor<R> visitor);

    /**
     * Creates and returns a deep copy of this node.
     * @return A new node instance with the same content.
     */
    BaseNode clone();
}

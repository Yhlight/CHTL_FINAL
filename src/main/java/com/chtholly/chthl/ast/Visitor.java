package com.chtholly.chthl.ast;

/**
 * The Visitor interface for the AST nodes.
 * This allows for clean separation of operations on the tree structure.
 * @param <R> The return type of the visit methods.
 */
public interface Visitor<R> {
    R visitElementNode(ElementNode node);
    R visitTextNode(TextNode node);
    R visitCommentNode(CommentNode node);

    /**
     * Visits a StyleBlockNode.
     * @param node The StyleBlockNode to visit.
     * @return The result of the visit.
     */
    default R visitStyleBlockNode(StyleBlockNode node) {
        // Default implementation to avoid breaking existing visitors.
        // Can be overridden by visitors that need to handle style blocks.
        return null;
    }
}

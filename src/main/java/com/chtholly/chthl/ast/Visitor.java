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
        return null;
    }

    /**
     * Visits a StylePropertyNode.
     * @param node The StylePropertyNode to visit.
     * @return The result of the visit.
     */
    default R visitStylePropertyNode(StylePropertyNode node) {
        return null;
    }

    /**
     * Visits a SelectorBlockNode.
     * @param node The SelectorBlockNode to visit.
     * @return The result of the visit.
     */
    default R visitSelectorBlockNode(SelectorBlockNode node) {
        return null;
    }
}

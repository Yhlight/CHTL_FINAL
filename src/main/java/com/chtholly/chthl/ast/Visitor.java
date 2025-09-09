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
}

package com.chtholly.chthl.generator;

import com.chtholly.chthl.node.DocumentNode;
import com.chtholly.chthl.node.ElementNode;
import com.chtholly.chthl.node.TextNode;

/**
 * An interface for the Visitor design pattern.
 * It provides a way to apply operations on nodes of the AST without changing the node classes.
 * @param <T> The return type of the visit methods.
 */
public interface Visitor<T> {
    T visit(DocumentNode node);
    T visit(ElementNode node);
    T visit(TextNode node);
}

package com.chtholly.chthl.ast;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * Represents a [Namespace] node in the CHTL AST.
 * e.g., <code>[Namespace] MySpace { ... }</code>
 */
public class NamespaceNode implements Node {

    public final String name;
    public final List<Node> children;

    public NamespaceNode(String name, List<Node> children) {
        this.name = name;
        this.children = children;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitNamespaceNode(this);
    }

    @Override
    public Node clone() {
        List<Node> clonedChildren = children.stream()
                                            .map(Node::clone)
                                            .collect(Collectors.toList());
        return new NamespaceNode(name, clonedChildren);
    }
}

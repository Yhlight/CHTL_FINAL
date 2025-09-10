package com.chtholly.chthl.ast;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Represents an element node in the CHTL AST, e.g., <code>div { ... }</code>.
 */
public class ElementNode implements Node {
    public final String tagName;
    public final Map<String, String> attributes;
    public final List<Node> children;

    public ElementNode(String tagName, Map<String, String> attributes, List<Node> children) {
        this.tagName = tagName;
        this.attributes = attributes;
        this.children = children;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitElementNode(this);
    }

    @Override
    public Node clone() {
        Map<String, String> clonedAttributes = new HashMap<>(this.attributes);
        List<Node> clonedChildren = new ArrayList<>();
        for (Node child : this.children) {
            clonedChildren.add(child.clone());
        }
        return new ElementNode(this.tagName, clonedAttributes, clonedChildren);
    }
}

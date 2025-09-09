package com.chtholly.chthl.ast;

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
}

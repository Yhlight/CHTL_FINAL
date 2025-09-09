package com.chtholly.chthl.ast;

import java.util.List;

/**
 * Represents a CSS rule block with a selector, containing multiple style properties or template usages.
 * e.g., <code>.my-class { color: blue; @Style other; }</code>
 */
public class SelectorBlockNode implements Node {
    public final String selector;
    public final List<Node> body; // Can contain StylePropertyNode and TemplateUsageNode

    public SelectorBlockNode(String selector, List<Node> body) {
        this.selector = selector;
        this.body = body;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitSelectorBlockNode(this);
    }
}

package com.chtholly.chthl.ast;

import java.util.List;

/**
 * Represents a CSS rule block with a selector, containing multiple style properties.
 * e.g., <code>.my-class { color: blue; }</code>
 */
public class SelectorBlockNode implements Node {
    public final String selector;
    public final List<StylePropertyNode> properties;

    public SelectorBlockNode(String selector, List<StylePropertyNode> properties) {
        this.selector = selector;
        this.properties = properties;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitSelectorBlockNode(this);
    }
}

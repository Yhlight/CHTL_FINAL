package com.chtholly.chthl.ast;

import java.util.List;

/**
 * Represents a style block node in the CHTL AST, e.g., <code>style { ... }</code>.
 * This node can contain both direct properties (for inline styles) and nested
 * selector blocks (for global styles).
 */
public class StyleBlockNode implements Node {
    public final List<StylePropertyNode> directProperties;
    public final List<SelectorBlockNode> selectorBlocks;

    public StyleBlockNode(List<StylePropertyNode> directProperties, List<SelectorBlockNode> selectorBlocks) {
        this.directProperties = directProperties;
        this.selectorBlocks = selectorBlocks;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitStyleBlockNode(this);
    }
}

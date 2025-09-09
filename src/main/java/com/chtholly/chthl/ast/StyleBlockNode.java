package com.chtholly.chthl.ast;

import java.util.Map;

/**
 * Represents a style block node in the CHTL AST, e.g., <code>style { color: red; }</code>.
 */
public class StyleBlockNode implements Node {
    public final Map<String, String> properties;

    public StyleBlockNode(Map<String, String> properties) {
        this.properties = properties;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitStyleBlockNode(this);
    }
}

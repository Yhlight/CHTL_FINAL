package com.chtholly.chthl.ast;

/**
 * Represents a single key-value style property within a style block or selector block.
 * e.g., <code>color: red;</code>
 */
public class StylePropertyNode implements Node {
    public final String key;
    public final String value; // For now, the value is a simple string. This will be expanded later.

    public StylePropertyNode(String key, String value) {
        this.key = key;
        this.value = value;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitStylePropertyNode(this);
    }
}

package com.chtholly.chthl.ast;

import com.chtholly.chthl.ast.expr.Expression;

/**
 * Represents a single key-value style property within a style block or selector block.
 * e.g., <code>color: red;</code>
 */
public class StylePropertyNode implements Node {
    public final String key;
    public final Expression value;

    public StylePropertyNode(String key, Expression value) {
        this.key = key;
        this.value = value;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitStylePropertyNode(this);
    }

    @Override
    public Node clone() {
        return new StylePropertyNode(this.key, this.value.clone());
    }
}

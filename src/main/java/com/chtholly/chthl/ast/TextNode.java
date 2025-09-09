package com.chtholly.chthl.ast;

/**
 * Represents a text node in the CHTL AST, e.g., <code>text { "some text" }</code>.
 */
public class TextNode implements Node {
    public final String text;

    public TextNode(String text) {
        this.text = text;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitTextNode(this);
    }
}

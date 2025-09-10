package com.chtholly.chthl.ast;

/**
 * Represents a generator-aware comment node in the CHTL AST, e.g., <code>-- a comment</code>.
 * These comments are intended to be processed by the generator.
 */
public class CommentNode implements Node {
    public final String text;

    public CommentNode(String text) {
        this.text = text;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitCommentNode(this);
    }

    @Override
    public Node clone() {
        return new CommentNode(this.text);
    }
}

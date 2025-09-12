package com.chtholly.chthl.node;

import com.chtholly.chthl.generator.Visitor;
import com.chtholly.chthl.lexer.Token;

/**
 * Represents a text node in the AST.
 * e.g., text { "This is some text" }
 */
public class TextNode implements Node {
    // The STRING token containing the text content
    private final Token token;
    private final String value;

    public TextNode(Token token) {
        this.token = token;
        this.value = token.literal;
    }

    public String getValue() {
        return value;
    }

    @Override
    public String tokenLiteral() {
        return token.literal;
    }

    @Override
    public String toString() {
        return value;
    }

    @Override
    public <T> T accept(Visitor<T> visitor) {
        return visitor.visit(this);
    }
}

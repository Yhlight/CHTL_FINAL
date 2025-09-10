package com.chtholly.chthl.ast;

import com.chtholly.chthl.lexer.Token;

public class OriginNode implements Node {
    public final Token type; // @Html, @Style, etc.
    public final Token name; // Optional
    public final String content; // The raw string content
    public final boolean isUsage;

    public OriginNode(Token type, Token name, String content, boolean isUsage) {
        this.type = type;
        this.name = name;
        this.content = content;
        this.isUsage = isUsage;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitOriginNode(this);
    }

    @Override
    public Node clone() {
        // Tokens are immutable-like, String is immutable. No deep copy needed.
        return new OriginNode(type, name, content, isUsage);
    }
}

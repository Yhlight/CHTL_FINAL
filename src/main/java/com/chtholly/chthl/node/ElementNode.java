package com.chtholly.chthl.node;

import com.chtholly.chthl.generator.Visitor;
import com.chtholly.chthl.lexer.Token;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * Represents an element node in the AST, corresponding to an HTML tag.
 * e.g., div { ... }
 */
public class ElementNode implements Node {
    // The token for the tag name, e.g., IDENTIFIER("div")
    private final Token token;
    private final String tagName;
    // Attributes will be implemented in a later step
    private final Map<String, String> attributes = new HashMap<>();
    private final List<Node> children = new ArrayList<>();

    public ElementNode(Token token) {
        this.token = token;
        this.tagName = token.literal;
    }

    public String getTagName() {
        return tagName;
    }

    public List<Node> getChildren() {
        return children;
    }

    public void addChild(Node node) {
        this.children.add(node);
    }

    // Methods for attributes will be added later

    @Override
    public String tokenLiteral() {
        return token.literal;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("<").append(tagName).append(">");
        for (Node child : children) {
            sb.append(child.toString());
        }
        sb.append("</").append(tagName).append(">");
        return sb.toString();
    }

    @Override
    public <T> T accept(Visitor<T> visitor) {
        return visitor.visit(this);
    }
}

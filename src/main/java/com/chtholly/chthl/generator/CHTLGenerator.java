package com.chtholly.chthl.generator;

import com.chtholly.chthl.node.DocumentNode;
import com.chtholly.chthl.node.ElementNode;
import com.chtholly.chthl.node.Node;
import com.chtholly.chthl.node.TextNode;

public class CHTLGenerator implements Visitor<String> {

    public String generate(Node node) {
        return node.accept(this);
    }

    @Override
    public String visit(DocumentNode node) {
        StringBuilder sb = new StringBuilder();
        for (Node child : node.getChildren()) {
            sb.append(child.accept(this));
        }
        return sb.toString();
    }

    @Override
    public String visit(ElementNode node) {
        StringBuilder sb = new StringBuilder();
        sb.append("<").append(node.getTagName()).append(">");
        for (Node child : node.getChildren()) {
            sb.append(child.accept(this));
        }
        sb.append("</").append(node.getTagName()).append(">");
        return sb.toString();
    }

    @Override
    public String visit(TextNode node) {
        // In a real scenario, you might want to escape HTML characters here.
        // For now, we'll return the raw value.
        return node.getValue();
    }
}

package com.chtholly.chthl.node;

import com.chtholly.chthl.generator.Visitor;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * The root node of a CHTL document's AST. It contains all top-level statements/nodes.
 */
public class DocumentNode implements Node {
    private final List<Node> children = new ArrayList<>();

    public List<Node> getChildren() {
        return children;
    }

    public void addChild(Node node) {
        this.children.add(node);
    }

    @Override
    public String tokenLiteral() {
        if (children.isEmpty()) {
            return "";
        }
        return children.get(0).tokenLiteral();
    }

    @Override
    public String toString() {
        return children.stream()
                .map(Node::toString)
                .collect(Collectors.joining("\n"));
    }

    @Override
    public <T> T accept(Visitor<T> visitor) {
        return visitor.visit(this);
    }
}

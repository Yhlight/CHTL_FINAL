package com.chtholly.chthl.generator;

import com.chtholly.chthl.ast.*;

import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Generates the final HTML string from an Abstract Syntax Tree (AST).
 * Implements the Visitor pattern to traverse the AST.
 */
public class CHTLGenerator implements Visitor<String> {

    private static final Set<String> SELF_CLOSING_TAGS = Set.of(
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    );

    public String generate(List<Node> nodes) {
        StringBuilder builder = new StringBuilder();
        for (Node node : nodes) {
            builder.append(node.accept(this));
        }
        return builder.toString();
    }

    @Override
    public String visitElementNode(ElementNode node) {
        StringBuilder builder = new StringBuilder();

        // Opening tag
        builder.append("<").append(node.tagName);

        // Attributes
        for (Map.Entry<String, String> attribute : node.attributes.entrySet()) {
            builder.append(" ")
                   .append(attribute.getKey())
                   .append("=\"")
                   .append(attribute.getValue()) // Note: No HTML escaping for now.
                   .append("\"");
        }

        if (node.children.isEmpty() && SELF_CLOSING_TAGS.contains(node.tagName.toLowerCase())) {
            builder.append(">");
        } else {
            builder.append(">");
            // Children
            for (Node child : node.children) {
                builder.append(child.accept(this));
            }
            // Closing tag
            builder.append("</").append(node.tagName).append(">");
        }

        return builder.toString();
    }

    @Override
    public String visitTextNode(TextNode node) {
        // Note: No HTML escaping for now. This is a simplification.
        return node.text;
    }

    @Override
    public String visitCommentNode(CommentNode node) {
        // The CHTL.md spec doesn't specify the format of -- comments,
        // but converting to HTML comments is the logical generation step.
        return "<!-- " + node.text + " -->";
    }
}

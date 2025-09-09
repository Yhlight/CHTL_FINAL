package com.chtholly.chthl.generator;

import com.chtholly.chthl.ast.*;

import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

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

        // --- Style block processing ---
        StringBuilder styleAttrBuilder = new StringBuilder();
        for (Node child : node.children) {
            if (child instanceof StyleBlockNode) {
                StyleBlockNode styleNode = (StyleBlockNode) child;
                for (Map.Entry<String, String> property : styleNode.properties.entrySet()) {
                    styleAttrBuilder.append(property.getKey()).append(":")
                                  .append(property.getValue()).append(";");
                }
            }
        }

        // Opening tag
        builder.append("<").append(node.tagName);

        // Attributes
        for (Map.Entry<String, String> attribute : node.attributes.entrySet()) {
            builder.append(" ")
                   .append(attribute.getKey())
                   .append("=\"")
                   .append(attribute.getValue())
                   .append("\"");
        }

        // Append style attribute if styles were found
        if (styleAttrBuilder.length() > 0) {
            builder.append(" style=\"").append(styleAttrBuilder.toString()).append("\"");
        }

        List<Node> contentChildren = node.children.stream()
                .filter(child -> !(child instanceof StyleBlockNode))
                .collect(Collectors.toList());

        if (contentChildren.isEmpty() && SELF_CLOSING_TAGS.contains(node.tagName.toLowerCase())) {
            builder.append(">");
        } else {
            builder.append(">");
            // Children
            for (Node child : contentChildren) {
                builder.append(child.accept(this));
            }
            // Closing tag
            builder.append("</").append(node.tagName).append(">");
        }

        return builder.toString();
    }

    @Override
    public String visitTextNode(TextNode node) {
        return node.text;
    }

    @Override
    public String visitCommentNode(CommentNode node) {
        return "<!-- " + node.text + " -->";
    }

    @Override
    public String visitStyleBlockNode(StyleBlockNode node) {
        // Handled by the parent ElementNode, so this produces no direct output.
        return "";
    }
}

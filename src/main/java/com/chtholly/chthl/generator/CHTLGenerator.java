package com.chtholly.chthl.generator;

import com.chtholly.chthl.ast.*;

import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.stream.Collectors;

/**
 * Generates the final HTML and CSS from an Abstract Syntax Tree (AST).
 * Implements the Visitor pattern to traverse the AST.
 */
public class CHTLGenerator implements Visitor<String> {

    private static final Set<String> SELF_CLOSING_TAGS = Set.of(
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    );

    private final StringBuilder globalCssBuilder;
    private ElementNode currentElement = null;

    public CHTLGenerator() {
        this.globalCssBuilder = new StringBuilder();
    }

    public CompilationResult generate(List<Node> nodes) {
        StringBuilder htmlBuilder = new StringBuilder();
        for (Node node : nodes) {
            htmlBuilder.append(node.accept(this));
        }
        return new CompilationResult(htmlBuilder.toString(), globalCssBuilder.toString());
    }

    private String resolveSelector(String selector, ElementNode parent) {
        if (!selector.contains("&")) {
            return selector;
        }
        if (parent == null) {
            return selector;
        }

        String parentSelector = "";
        if (parent.attributes.containsKey("class")) {
            String firstClass = parent.attributes.get("class").split("\\s+")[0];
            parentSelector = "." + firstClass;
        } else if (parent.attributes.containsKey("id")) {
            parentSelector = "#" + parent.attributes.get("id");
        } else {
            return selector;
        }

        return selector.replace("&", parentSelector);
    }

    @Override
    public String visitElementNode(ElementNode node) {
        ElementNode previousElement = this.currentElement;
        this.currentElement = node;

        StringBuilder builder = new StringBuilder();

        StringBuilder styleAttrBuilder = new StringBuilder();
        for (Node child : node.children) {
            if (child instanceof StyleBlockNode) {
                StyleBlockNode styleNode = (StyleBlockNode) child;
                for (StylePropertyNode property : styleNode.directProperties) {
                    styleAttrBuilder.append(property.key).append(":")
                                  .append(property.value).append(";");
                }

                for (SelectorBlockNode selectorBlock : styleNode.selectorBlocks) {
                    String selector = selectorBlock.selector.trim();
                    if (selector.startsWith(".")) {
                        if (!node.attributes.containsKey("class")) {
                            String className = selector.substring(1).split("[\\s:]")[0];
                            node.attributes.put("class", className);
                        }
                    } else if (selector.startsWith("#")) {
                        if (!node.attributes.containsKey("id")) {
                            String idName = selector.substring(1).split("[\\s:]")[0];
                            node.attributes.put("id", idName);
                        }
                    }
                }
                child.accept(this);
            }
        }

        builder.append("<").append(node.tagName);

        // Use TreeMap to sort attributes for consistent output, making tests reliable.
        Map<String, String> sortedAttributes = new TreeMap<>(node.attributes);
        for (Map.Entry<String, String> attribute : sortedAttributes.entrySet()) {
            builder.append(" ")
                   .append(attribute.getKey())
                   .append("=\"")
                   .append(attribute.getValue())
                   .append("\"");
        }

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
            for (Node child : contentChildren) {
                builder.append(child.accept(this));
            }
            builder.append("</").append(node.tagName).append(">");
        }

        this.currentElement = previousElement;
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
        for (SelectorBlockNode selectorBlock : node.selectorBlocks) {
            selectorBlock.accept(this);
        }
        return "";
    }

    @Override
    public String visitSelectorBlockNode(SelectorBlockNode node) {
        String resolvedSelector = resolveSelector(node.selector, this.currentElement);

        globalCssBuilder.append(resolvedSelector).append(" {\n");
        for (StylePropertyNode property : node.properties) {
            globalCssBuilder.append("    ")
                          .append(property.key)
                          .append(": ")
                          .append(property.value)
                          .append(";\n");
        }
        globalCssBuilder.append("}\n");
        return "";
    }
}

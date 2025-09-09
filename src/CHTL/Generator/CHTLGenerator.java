package CHTL.Generator;

import CHTL.Node.*;
import java.util.List;
import java.util.Map;

public class CHTLGenerator {

    public String generate(List<BaseNode> ast) {
        StringBuilder builder = new StringBuilder();
        for (BaseNode node : ast) {
            builder.append(generateNode(node));
        }
        return builder.toString();
    }

    private String generateNode(BaseNode node) {
        if (node instanceof ElementNode) return generateElementNode((ElementNode) node);
        if (node instanceof TextNode) return ((TextNode) node).getText();
        // Other node types (templates, etc.) should have been processed and removed.
        return "";
    }

    private String generateElementNode(ElementNode node) {
        StringBuilder builder = new StringBuilder();
        builder.append("<").append(node.getTagName());

        // Process attributes, which now includes style properties merged from style blocks
        Map<String, String> finalAttributes = new HashMap<>();
        for (Map.Entry<String, Object> entry : node.getAttributes().entrySet()) {
            finalAttributes.put(entry.getKey(), entry.getValue().toString());
        }

        // Process style blocks and merge into a single style attribute
        StringBuilder styleBuilder = new StringBuilder();
        for (BaseNode child : node.getChildren()) {
            if (child instanceof StyleNode styleNode) {
                for (Map.Entry<String, String> prop : styleNode.getProperties().entrySet()) {
                    styleBuilder.append(prop.getKey()).append(":").append(prop.getValue()).append(";");
                }
            }
        }
        if (styleBuilder.length() > 0) {
            String existingStyle = finalAttributes.getOrDefault("style", "");
            finalAttributes.put("style", existingStyle + styleBuilder.toString());
        }

        for (Map.Entry<String, String> entry : finalAttributes.entrySet()) {
            builder.append(" ").append(entry.getKey()).append("=\"").append(entry.getValue()).append("\"");
        }

        builder.append(">");

        for (BaseNode child : node.getChildren()) {
            // Don't generate the style blocks themselves
            if (!(child instanceof StyleNode)) {
                builder.append(generateNode(child));
            }
        }

        builder.append("</").append(node.getTagName()).append(">");
        return builder.toString();
    }
}

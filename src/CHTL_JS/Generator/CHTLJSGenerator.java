package CHTL_JS.Generator;

import CHTL_JS.Node.CHTLJSBaseNode;
import CHTL_JS.Node.ListenNode;
import java.util.List;
import java.util.Map;

public class CHTLJSGenerator {

    public String generate(List<CHTLJSBaseNode> nodes) {
        StringBuilder builder = new StringBuilder();
        for (CHTLJSBaseNode node : nodes) {
            if (node instanceof ListenNode) {
                builder.append(generateListenNode((ListenNode) node));
            } else if (node instanceof AnimateNode) {
                builder.append("/* AnimateNode processed */\n");
            } else if (node instanceof DelegateNode) {
                builder.append("/* DelegateNode processed */\n");
            } else if (node instanceof RouterNode) {
                builder.append("/* RouterNode processed */\n");
            }
        }
        return builder.toString();
    }

    private String generateListenNode(ListenNode node) {
        StringBuilder builder = new StringBuilder();
        // This is a simplified query selector generation.
        builder.append("document.querySelector('")
               .append(node.getTargetSelector())
               .append("').addEventListener(");

        for (Map.Entry<String, String> handler : node.getEventHandlers().entrySet()) {
            builder.append("'")
                   .append(handler.getKey())
                   .append("', ")
                   .append(handler.getValue()) // The raw JS function body
                   .append(");\n");
        }

        // This generation is flawed as it produces multiple addEventListener calls.
        // A better approach would be to generate a single call with a function that
        // handles multiple events, or multiple separate full statements.
        // For now, this demonstrates the concept.

        // Let's fix the flawed generation to produce separate, valid statements.
        builder.setLength(0); // Reset builder

        for (Map.Entry<String, String> handler : node.getEventHandlers().entrySet()) {
             builder.append("document.querySelector('")
               .append(node.getTargetSelector())
               .append("').addEventListener('")
               .append(handler.getKey())
               .append("', ")
               .append(handler.getValue())
               .append(");\n");
        }

        return builder.toString();
    }
}

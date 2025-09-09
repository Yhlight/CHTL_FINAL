package CHTL_JS.Generator;

import CHTL_JS.Node.CHTLJSBaseNode;
import CHTL_JS.Node.ListenNode;
import java.util.List;
import java.util.Map;

public class CHTLJSGenerator {

    public String generate(List<CHTLJSBaseNode> nodes) {
        StringBuilder builder = new StringBuilder();
        for (CHTLJSBaseNode node : nodes) {
            if (node instanceof ListenNode listenNode) {
                builder.append(generateListenNode(listenNode));
            } else if (node instanceof AnimateNode animateNode) {
                builder.append(generateAnimateNode(animateNode));
            } else if (node instanceof DelegateNode delegateNode) {
                builder.append(generateDelegateNode(delegateNode));
            } else if (node instanceof RouterNode routerNode) {
                builder.append(generateRouterNode(routerNode));
            }
        }
        return builder.toString();
    }

    private String generateAnimateNode(AnimateNode node) {
        StringBuilder builder = new StringBuilder("animate({\n");
        for(Map.Entry<String, String> entry : node.getProperties().entrySet()) {
            builder.append("  ").append(entry.getKey()).append(": ").append(entry.getValue()).append(",\n");
        }
        builder.append("});\n");
        return builder.toString();
    }

    private String generateDelegateNode(DelegateNode node) {
         StringBuilder builder = new StringBuilder();
         builder.append("document.querySelector('").append(node.getParentSelector()).append("').delegate({\n");
         for(Map.Entry<String, String> entry : node.getDelegatedEvents().entrySet()) {
            builder.append("  ").append(entry.getKey()).append(": ").append(entry.getValue()).append(",\n");
        }
        builder.append("});\n");
        return builder.toString();
    }

    private String generateRouterNode(RouterNode node) {
        StringBuilder builder = new StringBuilder("router({\n");
        for(Map.Entry<String, String> entry : node.getRoutes().entrySet()) {
            builder.append("  '").append(entry.getKey()).append("': ").append(entry.getValue()).append(",\n");
        }
        builder.append("});\n");
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

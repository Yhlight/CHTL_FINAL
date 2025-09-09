package CHTL_JS.Generator;

import CHTL_JS.Node.*;
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

    private String generateListenNode(ListenNode node) {
        StringBuilder builder = new StringBuilder();
        for (Map.Entry<String, String> handler : node.getEventHandlers().entrySet()) {
             builder.append("document.querySelector('")
               .append(node.getTargetSelector())
               .append("').addEventListener('")
               .append(handler.getKey())
               .append("', ")
               .append(handler.getValue()) // This assumes value is a valid JS function expression
               .append(");\n");
        }
        return builder.toString();
    }

    private String generateAnimateNode(AnimateNode node) {
        // This is a simplified generator. A real one would create a complex
        // requestAnimationFrame loop based on the properties.
        StringBuilder builder = new StringBuilder();
        builder.append("{\n  // Animate block for target: ")
               .append(node.getProperties().getOrDefault("target", "undefined"))
               .append("\n");
        builder.append("  // Duration: ")
               .append(node.getProperties().getOrDefault("duration", "0"))
               .append("ms\n");
        builder.append("}\n");
        return builder.toString();
    }

    private String generateDelegateNode(DelegateNode node) {
        // Simplified generator for event delegation.
        StringBuilder builder = new StringBuilder();
        builder.append("document.querySelector('")
               .append(node.getParentSelector())
               .append("').addEventListener('click', (e) => {\n");
        for(Map.Entry<String, String> entry : node.getDelegatedEvents().entrySet()) {
            // This assumes the key is the target selector for the delegation
             builder.append("  if (e.target.matches('").append(entry.getKey()).append("')) {\n");
             builder.append("    (").append(entry.getValue()).append(")(e);\n");
             builder.append("  }\n");
        }
        builder.append("});\n");
        return builder.toString();
    }

    private String generateRouterNode(RouterNode node) {
        // Highly simplified router. Does not create a real SPA router.
        StringBuilder builder = new StringBuilder("// Router configuration:\n");
        for(Map.Entry<String, String> entry : node.getRoutes().entrySet()) {
            builder.append("// Route: ").append(entry.getKey()).append(" -> ").append(entry.getValue()).append("\n");
        }
        return builder.toString();
    }
}

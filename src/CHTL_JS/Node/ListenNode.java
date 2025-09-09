package CHTL_JS.Node;

import java.util.Map;
import java.util.HashMap;

/**
 * Represents a CHTL JS `listen { ... }` block.
 */
public class ListenNode implements CHTLJSBaseNode {

    // The target of the listener, e.g., "{{.box}}"
    private final String targetSelector;

    // Map of event names (e.g., "click") to the raw JS function body string.
    private final Map<String, String> eventHandlers;

    public ListenNode(String targetSelector) {
        this.targetSelector = targetSelector;
        this.eventHandlers = new HashMap<>();
    }

    public void addEventHandler(String event, String handlerBody) {
        this.eventHandlers.put(event, handlerBody);
    }

    public String getTargetSelector() {
        return targetSelector;
    }

    public Map<String, String> getEventHandlers() {
        return eventHandlers;
    }
}

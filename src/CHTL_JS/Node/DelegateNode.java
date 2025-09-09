package CHTL_JS.Node;

import java.util.Map;

// Represents a CHTL JS `delegate { ... }` block.
public class DelegateNode implements CHTLJSBaseNode {
    private final String parentSelector;
    private final Map<String, String> delegatedEvents;

    public DelegateNode(String parentSelector) {
        this.parentSelector = parentSelector;
        this.delegatedEvents = new java.util.HashMap<>();
    }

    public void addDelegatedEvent(String key, String value) {
        this.delegatedEvents.put(key, value);
    }

    public String getParentSelector() { return parentSelector; }
    public Map<String, String> getDelegatedEvents() { return delegatedEvents; }
}

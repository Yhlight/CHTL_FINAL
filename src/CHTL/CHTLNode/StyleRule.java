package CHTL.CHTLNode;

import java.util.Map;
import java.util.HashMap;

/**
 * Represents a single CSS-like rule (e.g., '.my-class { color: blue; }')
 * found within a local style block.
 */
public class StyleRule {
    private final String selector;
    private final Map<String, String> properties;

    public StyleRule(String selector) {
        this.selector = selector;
        this.properties = new HashMap<>();
    }

    public void addProperty(String key, String value) {
        this.properties.put(key, value);
    }

    public String getSelector() {
        return selector;
    }

    public Map<String, String> getProperties() {
        return properties;
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append(selector).append(" {");
        for (Map.Entry<String, String> entry : properties.entrySet()) {
            builder.append(" ").append(entry.getKey()).append(": ").append(entry.getValue()).append(";");
        }
        builder.append(" }");
        return builder.toString();
    }
}

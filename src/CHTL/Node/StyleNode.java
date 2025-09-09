package CHTL.Node;

import java.util.Map;
import java.util.HashMap;

// Represents a style {} block in the CHTL AST.
public class StyleNode implements BaseNode {
    // The properties map will store the raw string value of each property.
    // The ASTProcessor will be responsible for parsing and evaluating the expressions in these values.
    private final Map<String, String> properties;

    public StyleNode() {
        this.properties = new HashMap<>();
        this.usages = new ArrayList<>();
    }

    public void addProperty(String key, String value) {
        this.properties.put(key, value);
    }

    private final List<StyleTemplateUsageNode> usages;

    public Map<String, String> getProperties() {
        return properties;
    }

    public List<StyleTemplateUsageNode> getUsages() {
        return usages;
    }

    public void addUsage(StyleTemplateUsageNode usage) {
        this.usages.add(usage);
    }

    @Override
    public BaseNode clone() {
        StyleNode cloned = new StyleNode();
        for (Map.Entry<String, String> entry : this.properties.entrySet()) {
            cloned.addProperty(entry.getKey(), entry.getValue());
        }
        for (StyleTemplateUsageNode usage : this.usages) {
            cloned.addUsage((StyleTemplateUsageNode) usage.clone());
        }
        return cloned;
    }
}

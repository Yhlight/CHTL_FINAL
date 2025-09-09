package CHTL.CHTLNode;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Represents a [Custom] @Style declaration in the AST.
 */
public class CustomStyleNode implements CustomNode {
    private final String customName;
    private final Map<String, String> properties;
    private final List<String> parentTemplates;
    private final List<String> valuelessProperties;

    public CustomStyleNode(String customName) {
        this.customName = customName;
        this.properties = new HashMap<>();
        this.parentTemplates = new ArrayList<>();
        this.valuelessProperties = new ArrayList<>();
    }

    public void addProperty(String key, String value) {
        this.properties.put(key, value);
    }

    public void addParentTemplate(String parentName) {
        this.parentTemplates.add(parentName);
    }

    public void addValuelessProperty(String name) {
        this.valuelessProperties.add(name);
    }

    @Override
    public String getCustomName() {
        return customName;
    }

    public Map<String, String> getProperties() {
        return properties;
    }

    public List<String> getParentTemplates() {
        return parentTemplates;
    }

    public List<String> getValuelessProperties() {
        return valuelessProperties;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitCustomStyleNode(this);
    }

    @Override
    public BaseNode clone() {
        CustomStyleNode cloned = new CustomStyleNode(this.customName);
        this.properties.forEach(cloned::addProperty);
        this.parentTemplates.forEach(cloned::addParentTemplate);
        this.valuelessProperties.forEach(cloned::addValuelessProperty);
        return cloned;
    }
}

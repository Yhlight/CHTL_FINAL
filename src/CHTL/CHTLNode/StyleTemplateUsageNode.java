package CHTL.CHTLNode;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class StyleTemplateUsageNode implements BaseNode {
    private final String templateName;
    private final String namespace; // Can be null if not specified
    private final Map<String, String> specializationProperties;
    private final List<String> deletedProperties;

    public StyleTemplateUsageNode(String templateName, String namespace) {
        this.templateName = templateName;
        this.namespace = namespace;
        this.specializationProperties = new HashMap<>();
        this.deletedProperties = new ArrayList<>();
    }

    public void addSpecializationProperty(String key, String value) {
        this.specializationProperties.put(key, value);
    }

    public void addDeletedProperty(String name) {
        this.deletedProperties.add(name);
    }

    public String getTemplateName() {
        return templateName;
    }

    public String getNamespace() {
        return namespace;
    }

    public Map<String, String> getSpecializationProperties() {
        return specializationProperties;
    }

    public List<String> getDeletedProperties() {
        return deletedProperties;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitStyleTemplateUsageNode(this);
    }

    @Override
    public BaseNode clone() {
        StyleTemplateUsageNode cloned = new StyleTemplateUsageNode(this.templateName, this.namespace);
        this.specializationProperties.forEach(cloned::addSpecializationProperty);
        this.deletedProperties.forEach(cloned::addDeletedProperty);
        return cloned;
    }
}

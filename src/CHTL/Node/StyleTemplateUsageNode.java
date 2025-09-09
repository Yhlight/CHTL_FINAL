package CHTL.Node;

import java.util.Map;
import java.util.HashMap;
import java.util.Set;
import java.util.HashSet;

public class StyleTemplateUsageNode implements BaseNode {
    private final String templateName;
    private final String namespace;
    private final Map<String, String> specializationProperties;
    private final Set<String> deletedProperties;

    public StyleTemplateUsageNode(String templateName, String namespace) {
        this.templateName = templateName;
        this.namespace = namespace;
        this.specializationProperties = new HashMap<>();
        this.deletedProperties = new HashSet<>();
    }

    public void addSpecialization(String key, String value) { this.specializationProperties.put(key, value); }
    public void addDeletion(String key) { this.deletedProperties.add(key); }
    public String getTemplateName() { return templateName; }
    public String getNamespace() { return namespace; }
    public Map<String, String> getSpecializationProperties() { return specializationProperties; }
    public Set<String> getDeletedProperties() { return deletedProperties; }
}

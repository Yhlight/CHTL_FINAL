package CHTL.Node;

import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

public class StyleTemplateNode extends TemplateNode {
    private final Map<String, String> properties;
    private final List<String> parentTemplates;

    public StyleTemplateNode(String name) {
        super(name);
        this.properties = new HashMap<>();
        this.parentTemplates = new ArrayList<>();
    }

    public void addProperty(String key, String value) { this.properties.put(key, value); }
    public void addParentTemplate(String parent) { this.parentTemplates.add(parent); }
    public Map<String, String> getProperties() { return properties; }
    public List<String> getParentTemplates() { return parentTemplates; }
}

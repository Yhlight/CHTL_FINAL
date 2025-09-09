package CHTL.CHTLNode;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Represents a [Template] @Style declaration in the AST.
 * It holds a named, reusable group of CSS properties.
 */
public class StyleTemplateNode implements TemplateNode {
    private final String templateName;
    private final Map<String, String> properties;
    private final List<String> parentTemplates;

    public StyleTemplateNode(String templateName) {
        this.templateName = templateName;
        this.properties = new HashMap<>();
        this.parentTemplates = new ArrayList<>();
    }

    public void addProperty(String key, String value) {
        this.properties.put(key, value);
    }

    public void addParentTemplate(String parentName) {
        this.parentTemplates.add(parentName);
    }

    @Override
    public String getTemplateName() {
        return templateName;
    }

    public Map<String, String> getProperties() {
        return properties;
    }

    public List<String> getParentTemplates() {
        return parentTemplates;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitStyleTemplateNode(this);
    }

    @Override
    public BaseNode clone() {
        StyleTemplateNode cloned = new StyleTemplateNode(this.templateName);
        for (Map.Entry<String, String> entry : this.properties.entrySet()) {
            cloned.addProperty(entry.getKey(), entry.getValue());
        }
        for (String parent : this.parentTemplates) {
            cloned.addParentTemplate(parent);
        }
        return cloned;
    }
}

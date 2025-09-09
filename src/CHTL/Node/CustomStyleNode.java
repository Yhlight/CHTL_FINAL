package CHTL.Node;

import java.util.Set;
import java.util.HashSet;

public class CustomStyleNode extends StyleTemplateNode {
    private final Set<String> valuelessProperties;
    private final Set<String> constrainedProperties; // for 'except'

    public CustomStyleNode(String name) {
        super(name);
        this.valuelessProperties = new HashSet<>();
        this.constrainedProperties = new HashSet<>();
    }

    public void addValuelessProperty(String name) { this.valuelessProperties.add(name); }
    public void addConstrainedProperty(String name) { this.constrainedProperties.add(name); }
    public Set<String> getValuelessProperties() { return valuelessProperties; }
    public Set<String> getConstrainedProperties() { return constrainedProperties; }
}

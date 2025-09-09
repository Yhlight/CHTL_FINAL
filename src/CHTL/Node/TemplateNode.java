package CHTL.Node;

// Base class for all [Template] and [Custom] definitions.
public abstract class TemplateNode implements BaseNode {
    private final String name;

    protected TemplateNode(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }
}

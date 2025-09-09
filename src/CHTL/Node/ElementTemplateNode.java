package CHTL.Node;

import java.util.List;
import java.util.ArrayList;

public class ElementTemplateNode extends TemplateNode {
    private final List<BaseNode> children;

    public ElementTemplateNode(String name) {
        super(name);
        this.children = new ArrayList<>();
    }

    public void addChild(BaseNode child) { this.children.add(child); }
    public List<BaseNode> getChildren() { return children; }
}

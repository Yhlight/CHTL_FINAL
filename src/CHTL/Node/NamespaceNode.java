package CHTL.Node;

import java.util.List;
import java.util.ArrayList;

// Represents a [Namespace] block
public class NamespaceNode implements BaseNode {
    private final String name;
    private final List<BaseNode> children;

    public NamespaceNode(String name) {
        this.name = name;
        this.children = new ArrayList<>();
    }

    public void addChild(BaseNode node) {
        this.children.add(node);
    }

    public String getName() {
        return name;
    }

    public List<BaseNode> getChildren() {
        return children;
    }

    @Override
    public BaseNode clone() {
        NamespaceNode cloned = new NamespaceNode(name);
        for (BaseNode child : children) {
            cloned.addChild(child.clone());
        }
        return cloned;
    }
}

package CHTL.CHTLNode;

import java.util.ArrayList;
import java.util.List;

/**
 * Represents a [Custom] @Element declaration in the AST.
 */
public class CustomElementNode implements CustomNode {
    private final String customName;
    private final List<BaseNode> children;

    public CustomElementNode(String customName) {
        this.customName = customName;
        this.children = new ArrayList<>();
    }

    public void addChild(BaseNode node) {
        this.children.add(node);
    }

    @Override
    public String getCustomName() {
        return customName;
    }

    public List<BaseNode> getChildren() {
        return children;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitCustomElementNode(this);
    }

    @Override
    public BaseNode clone() {
        CustomElementNode cloned = new CustomElementNode(this.customName);
        for (BaseNode child : this.children) {
            cloned.addChild(child.clone());
        }
        return cloned;
    }
}

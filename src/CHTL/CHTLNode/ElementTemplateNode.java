package CHTL.CHTLNode;

import java.util.List;
import java.util.ArrayList;

/**
 * Represents a [Template] @Element declaration in the AST.
 * It holds a named, reusable group of other AST nodes.
 */
public class ElementTemplateNode implements TemplateNode {
    private final String templateName;
    private final List<BaseNode> children;

    public ElementTemplateNode(String templateName) {
        this.templateName = templateName;
        this.children = new ArrayList<>();
    }

    public void addChild(BaseNode node) {
        this.children.add(node);
    }

    @Override
    public String getTemplateName() {
        return templateName;
    }

    public List<BaseNode> getChildren() {
        return children;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitElementTemplateNode(this);
    }

    @Override
    public BaseNode clone() {
        ElementTemplateNode cloned = new ElementTemplateNode(this.templateName);
        for (BaseNode child : this.children) {
            cloned.addChild(child.clone());
        }
        return cloned;
    }
}

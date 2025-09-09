package CHTL.CHTLNode;

import java.util.List;
import java.util.Map;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Represents an element node in the AST, such as 'div' or 'span'.
 * It contains the element's tag name, a map of its attributes,
 * and a list of its child nodes.
 */
public class ElementNode implements BaseNode {
    private final String tagName;
    private final Map<String, String> attributes;
    private final List<BaseNode> children;

    public ElementNode(String tagName, Map<String, String> attributes, List<BaseNode> children) {
        this.tagName = tagName;
        this.attributes = (attributes != null) ? attributes : new HashMap<>();
        this.children = (children != null) ? children : new ArrayList<>();
    }

    public ElementNode(String tagName) {
        this(tagName, new HashMap<>(), new ArrayList<>());
    }

    public String getTagName() {
        return tagName;
    }

    public Map<String, String> getAttributes() {
        return attributes;
    }

    public List<BaseNode> getChildren() {
        return children;
    }

    public void addChild(BaseNode node) {
        this.children.add(node);
    }

    public void addAttribute(String key, String value) {
        this.attributes.put(key, value);
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitElementNode(this);
    }

    @Override
    public BaseNode clone() {
        List<BaseNode> clonedChildren = new ArrayList<>();
        for (BaseNode child : this.children) {
            clonedChildren.add(child.clone());
        }
        Map<String, String> clonedAttributes = new HashMap<>(this.attributes);
        return new ElementNode(this.tagName, clonedAttributes, clonedChildren);
    }
}

package CHTL.Node;

import java.util.List;
import java.util.Map;
import java.util.ArrayList;
import java.util.HashMap;

// Represents an element node in the CHTL AST.
public class ElementNode implements BaseNode {
    private final String tagName;
    private final Map<String, Object> attributes;
    private List<BaseNode> children;

    public ElementNode(String tagName) {
        this.tagName = tagName;
        this.attributes = new HashMap<>();
        this.children = new ArrayList<>();
    }

    public ElementNode(String tagName, Map<String, Object> attributes, List<BaseNode> children) {
        this.tagName = tagName;
        this.attributes = attributes;
        this.children = children;
    }

    public String getTagName() { return tagName; }
    public Map<String, Object> getAttributes() { return attributes; }
    public List<BaseNode> getChildren() { return children; }
    public void setChildren(List<BaseNode> children) { this.children = children; }
    public void addChild(BaseNode node) { this.children.add(node); }
    public void addAttribute(String key, Object value) { this.attributes.put(key, value); }

    @Override
    public BaseNode clone() {
        Map<String, Object> clonedAttributes = new HashMap<>(this.attributes);
        List<BaseNode> clonedChildren = new ArrayList<>();
        for (BaseNode child : this.children) {
            if (child instanceof ElementNode) clonedChildren.add(((ElementNode)child).clone());
            else if (child instanceof TextNode) clonedChildren.add(((TextNode)child).clone());
            else if (child instanceof StyleNode) clonedChildren.add(((StyleNode)child).clone());
            // Add other node types here as they are created
        }
        return new ElementNode(this.tagName, clonedAttributes, clonedChildren);
    }
}

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

    public String getTagName() { return tagName; }
    public Map<String, Object> getAttributes() { return attributes; }
    public List<BaseNode> getChildren() { return children; }
    public void setChildren(List<BaseNode> children) { this.children = children; }
    public void addChild(BaseNode node) { this.children.add(node); }
    public void addAttribute(String key, Object value) { this.attributes.put(key, value); }
}

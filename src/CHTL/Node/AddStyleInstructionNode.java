package CHTL.Node;

public class AddStyleInstructionNode implements BaseNode {
    private final String selector;
    private final StyleNode styleNode;

    public AddStyleInstructionNode(String selector, StyleNode styleNode) {
        this.selector = selector;
        this.styleNode = styleNode;
    }

    public String getSelector() {
        return selector;
    }

    public StyleNode getStyleNode() {
        return styleNode;
    }
}

package CHTL.Node;

// Represents a text node in the CHTL AST.
public class TextNode implements BaseNode {
    private final String text;

    public TextNode(String text) {
        this.text = text;
    }

    public String getText() {
        return text;
    }
}

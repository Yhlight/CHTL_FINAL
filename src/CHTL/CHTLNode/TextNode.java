package CHTL.CHTLNode;

/**
 * Represents a text node in the AST.
 * This node holds plain text content that appears within an element.
 */
public class TextNode implements BaseNode {
    private final String text;

    public TextNode(String text) {
        this.text = text;
    }

    public String getText() {
        return text;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitTextNode(this);
    }

    @Override
    public BaseNode clone() {
        return new TextNode(this.text);
    }
}

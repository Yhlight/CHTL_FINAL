package CHTL.CHTLNode;

/**
 * Represents an [Origin] block in the AST.
 * It contains raw content that should be passed through the generator
 * without modification.
 */
public class OriginNode implements BaseNode {
    private final String type; // e.g., Html, Style, JavaScript
    private final String content;

    public OriginNode(String type, String content) {
        this.type = type;
        this.content = content;
    }

    public String getType() {
        return type;
    }

    public String getContent() {
        return content;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitOriginNode(this);
    }

    @Override
    public BaseNode clone() {
        return new OriginNode(this.type, this.content);
    }
}

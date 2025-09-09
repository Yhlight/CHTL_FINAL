package CHTL.Expression;

public class LiteralNode implements ExpressionNode {
    private final Object value;

    public LiteralNode(Object value) {
        this.value = value;
    }

    public Object getValue() {
        return value;
    }
}

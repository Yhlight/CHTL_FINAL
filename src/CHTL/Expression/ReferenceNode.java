package CHTL.Expression;

public class ReferenceNode implements ExpressionNode {
    private final String selector;
    private final String property;

    public ReferenceNode(String selector, String property) {
        this.selector = selector;
        this.property = property;
    }

    public String getSelector() {
        return selector;
    }

    public String getProperty() {
        return property;
    }
}

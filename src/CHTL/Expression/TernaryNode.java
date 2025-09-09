package CHTL.Expression;

public class TernaryNode implements ExpressionNode {
    private final ExpressionNode condition;
    private final ExpressionNode trueExpression;
    private final ExpressionNode falseExpression; // Can be null

    public TernaryNode(ExpressionNode condition, ExpressionNode trueExpression, ExpressionNode falseExpression) {
        this.condition = condition;
        this.trueExpression = trueExpression;
        this.falseExpression = falseExpression;
    }

    public ExpressionNode getCondition() {
        return condition;
    }

    public ExpressionNode getTrueExpression() {
        return trueExpression;
    }

    public ExpressionNode getFalseExpression() {
        return falseExpression;
    }
}

package CHTL.Expression;

public class UnitNode implements ExpressionNode {
    private final double value;
    private final String unit;

    public UnitNode(double value, String unit) {
        this.value = value;
        this.unit = unit;
    }

    public double getValue() {
        return value;
    }

    public String getUnit() {
        return unit;
    }
}

package CHTL.Expression;

import java.util.List;
import java.util.ArrayList;

// Represents a function-call-like syntax, used for variable lookups.
// e.g., ThemeColor(tableColor)
public class FunctionCallNode implements ExpressionNode {
    private final String functionName;
    private final List<ExpressionNode> arguments;

    public FunctionCallNode(String functionName) {
        this.functionName = functionName;
        this.arguments = new ArrayList<>();
    }

    public void addArgument(ExpressionNode arg) {
        this.arguments.add(arg);
    }

    public String getFunctionName() {
        return functionName;
    }

    public List<ExpressionNode> getArguments() {
        return arguments;
    }
}

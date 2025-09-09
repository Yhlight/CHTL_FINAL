package CHTL.Expression;

public class ExpressionEvaluator {

    public ExpressionNode evaluate(ExpressionNode node, EvaluationContext context) {
        if (node instanceof LiteralNode) {
            return node;
        }
        if (node instanceof UnitNode) {
            return node;
        }
        if (node instanceof BinaryOpNode) {
            return evaluateBinaryOp((BinaryOpNode) node, context);
        }
        if (node instanceof TernaryNode) {
            return evaluateTernary((TernaryNode) node, context);
        }
        if (node instanceof ReferenceNode) {
            return evaluateReference((ReferenceNode) node, context);
        }
        throw new RuntimeException("Unsupported expression node type: " + node.getClass().getSimpleName());
    }

    private ExpressionNode evaluateTernary(TernaryNode node, EvaluationContext context) {
        ExpressionNode conditionResult = evaluate(node.getCondition(), context);
        // This is a simplification. A real implementation would have a proper truthiness check.
        boolean isTrue = false;
        if (conditionResult instanceof LiteralNode) {
             Object val = ((LiteralNode) conditionResult).getValue();
             if (val instanceof Boolean && (Boolean) val) {
                 isTrue = true;
             }
        }

        if (isTrue) {
            return evaluate(node.getTrueExpression(), context);
        } else {
            if (node.getFalseExpression() != null) {
                return evaluate(node.getFalseExpression(), context);
            }
            // If false branch is optional and condition is false, what should happen?
            // The spec is unclear. For now, we return an empty literal.
            return new LiteralNode("");
        }
    }

    private ExpressionNode evaluateReference(ReferenceNode node, EvaluationContext context) {
        if (node.getSelector() == null) {
            // It's a reference to a property on the same element, e.g. `width`.
            // The ASTProcessor will need to handle this by looking up the property on the `self` element.
            // This creates a dependency graph problem. For now, we'll return the node itself.
            return node;
        }
        // This is a highly simplified reference search. It only looks for IDs.
        ElementNode target = findElementById(context.documentRoot(), node.getSelector().substring(1));
        if (target == null) {
            throw new RuntimeException("Could not find element with selector: " + node.getSelector());
        }
        Object rawValue = target.getAttributes().get(node.getProperty());
        if (rawValue == null) {
             throw new RuntimeException("Property '" + node.getProperty() + "' not found on element " + node.getSelector());
        }
        // The value might be another expression string, so we need to recursively parse and evaluate it.
        // This is the core of the dependency problem.
        // For now, we assume it's a simple value that can be parsed.
        ExpressionLexer lexer = new ExpressionLexer(rawValue.toString());
        ExpressionParser parser = new ExpressionParser(lexer.tokenize());
        return evaluate(parser.parse(), new EvaluationContext(target, context.documentRoot()));
    }

    private ElementNode findElementById(List<BaseNode> nodes, String id) {
        for (BaseNode node : nodes) {
            if (node instanceof ElementNode element) {
                Object elementId = element.getAttributes().get("id");
                if (elementId instanceof String && elementId.equals(id)) {
                    return element;
                }
                ElementNode foundInChildren = findElementById(element.getChildren(), id);
                if (foundInChildren != null) {
                    return foundInChildren;
                }
            }
        }
        return null;
    }

    private ExpressionNode evaluateBinaryOp(BinaryOpNode node, EvaluationContext context) {
        ExpressionNode left = evaluate(node.getLeft(), context);
        ExpressionNode right = evaluate(node.getRight(), context);

        if (left instanceof UnitNode || right instanceof UnitNode) {
            return evaluateUnitOperation(left, right, node.getOperator());
        }

        // Add logic for other types (e.g., number, string, boolean) later
        throw new RuntimeException("Unsupported operand types for binary operation.");
    }

    private ExpressionNode evaluateUnitOperation(ExpressionNode left, ExpressionNode right, String op) {
        double lVal, rVal;
        String unit = null;

        if (left instanceof UnitNode) {
            lVal = ((UnitNode) left).getValue();
            unit = ((UnitNode) left).getUnit();
        } else if (left instanceof LiteralNode && ((LiteralNode) left).getValue() instanceof Number) {
            lVal = ((Number) ((LiteralNode) left).getValue()).doubleValue();
        } else {
            throw new RuntimeException("Invalid left operand for unit operation.");
        }

        if (right instanceof UnitNode) {
            rVal = ((UnitNode) right).getValue();
            if (unit != null && !unit.equals(((UnitNode) right).getUnit())) throw new RuntimeException("Unit mismatch.");
            if (unit == null) unit = ((UnitNode) right).getUnit();
        } else if (right instanceof LiteralNode && ((LiteralNode) right).getValue() instanceof Number) {
            rVal = ((Number) ((LiteralNode) right).getValue()).doubleValue();
        } else {
            throw new RuntimeException("Invalid right operand for unit operation.");
        }

        switch (op) {
            case "+": return new UnitNode(lVal + rVal, unit);
            case "-": return new UnitNode(lVal - rVal, unit);
            case "*": return new UnitNode(lVal * rVal, unit);
            case "/": return new UnitNode(lVal / rVal, unit);
            case "%": return new UnitNode(lVal % rVal, unit);
            case "**": return new UnitNode(Math.pow(lVal, rVal), unit);
            case ">": return new LiteralNode(lVal > rVal);
            case ">=": return new LiteralNode(lVal >= rVal);
            case "<": return new LiteralNode(lVal < rVal);
            case "<=": return new LiteralNode(lVal <= rVal);
            default: throw new RuntimeException("Unsupported operator for unit operation: " + op);
        }
    }
}

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
        if (node instanceof FunctionCallNode) {
            return evaluateFunctionCall((FunctionCallNode) node, context);
        }
        throw new RuntimeException("Unsupported expression node type: " + node.getClass().getSimpleName());
    }

    private ExpressionNode evaluateFunctionCall(FunctionCallNode node, EvaluationContext context) {
        String templateName = node.getFunctionName();
        if (node.getArguments().size() != 1 || !(node.getArguments().get(0) instanceof ReferenceNode)) {
            throw new RuntimeException("Invalid variable usage syntax. Expect 'TemplateName(variableName)'.");
        }
        String varName = ((ReferenceNode) node.getArguments().get(0)).getProperty();

        DefinitionManager defManager = context.definitionManager();
        if (defManager == null) throw new RuntimeException("DefinitionManager not available.");

        // TODO: The namespace should be resolved more intelligently.
        String value = defManager.getVariableValue("default", templateName, varName)
            .orElseThrow(() -> new RuntimeException("Variable '" + varName + "' not found in template '" + templateName + "'."));

        return new LiteralNode(value);
    }

    private ExpressionNode evaluateTernary(TernaryNode node, EvaluationContext context) {
        ExpressionNode conditionResult = evaluate(node.getCondition(), context);

        if (isTruthy(conditionResult)) {
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

    private boolean isTruthy(ExpressionNode node) {
        if (node instanceof LiteralNode ln) {
            Object val = ln.getValue();
            if (val instanceof Boolean) return (Boolean) val;
            if (val instanceof Number) return ((Number) val).doubleValue() != 0;
            if (val instanceof String) return !((String) val).isEmpty();
            return val != null;
        }
        // UnitNodes are considered truthy if not zero
        if (node instanceof UnitNode un) {
            return un.getValue() != 0;
        }
        return false; // References, etc., are not truthy by themselves
    }

    private ExpressionNode evaluateReference(ReferenceNode node, EvaluationContext context) {
        if (node.getSelector() == null) {
            // It's a reference to a property on the same element, e.g. `height: width;`
            String propName = node.getProperty();
            if (context.computedValues().containsKey(propName)) {
                String rawValue = context.computedValues().get(propName);
                ExpressionLexer lexer = new ExpressionLexer(rawValue);
                ExpressionParser parser = new ExpressionParser(lexer.tokenize());
                // Evaluate this sub-expression in the same context
                return evaluate(parser.parse(), context);
            } else {
                throw new RuntimeException("Unresolved self-reference to property '" + propName + "'. Check for dependency cycle.");
            }
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

    private enum UnitCategory { LENGTH, TIME, ANGLE, PERCENTAGE, UNKNOWN }

    private UnitCategory getUnitCategory(String unit) {
        if (unit == null) return UnitCategory.UNKNOWN;
        return switch (unit.toLowerCase()) {
            case "px", "em", "rem", "vw", "vh", "cm", "mm", "in", "pt", "pc" -> UnitCategory.LENGTH;
            case "s", "ms" -> UnitCategory.TIME;
            case "deg", "grad", "rad", "turn" -> UnitCategory.ANGLE;
            case "%" -> UnitCategory.PERCENTAGE;
            default -> UnitCategory.UNKNOWN;
        };
    }

    private ExpressionNode evaluateUnitOperation(ExpressionNode left, ExpressionNode right, String op) {
        double lVal, rVal;
        String lUnit = null, rUnit = null;

        if (left instanceof UnitNode un) {
            lVal = un.getValue();
            lUnit = un.getUnit();
        } else if (left instanceof LiteralNode ln && ln.getValue() instanceof Number) {
            lVal = ((Number) ln.getValue()).doubleValue();
        } else {
            throw new RuntimeException("Invalid left operand for unit operation.");
        }

        if (right instanceof UnitNode un) {
            rVal = un.getValue();
            rUnit = un.getUnit();
        } else if (right instanceof LiteralNode ln && ln.getValue() instanceof Number) {
            rVal = ((Number) ln.getValue()).doubleValue();
        } else {
            throw new RuntimeException("Invalid right operand for unit operation.");
        }

        UnitCategory lCat = getUnitCategory(lUnit);
        UnitCategory rCat = getUnitCategory(rUnit);

        if (lUnit != null && rUnit != null && lCat != rCat) {
            throw new RuntimeException("Incompatible units for operation: " + lUnit + " and " + rUnit);
        }

        String resultUnit = lUnit != null ? lUnit : rUnit;
        if (resultUnit == null) { // Two literals were operated on
            // This path shouldn't be taken if called correctly, but as a fallback:
            double result = 0;
            switch(op) {
                case "+": result = lVal + rVal; break;
                // ... handle other ops for literals
            }
            return new LiteralNode(result);
        }

        switch (op) {
            case "+": return new UnitNode(lVal + rVal, resultUnit);
            case "-": return new UnitNode(lVal - rVal, resultUnit);
            case "*": return new UnitNode(lVal * rVal, resultUnit);
            case "/": return new UnitNode(lVal / rVal, resultUnit);
            case "%": return new UnitNode(lVal % rVal, resultUnit);
            case "**": return new UnitNode(Math.pow(lVal, rVal), resultUnit);
            case ">": return new LiteralNode(lVal > rVal);
            case ">=": return new LiteralNode(lVal >= rVal);
            case "<": return new LiteralNode(lVal < rVal);
            case "<=": return new LiteralNode(lVal <= rVal);
            default: throw new RuntimeException("Unsupported operator for unit operation: " + op);
        }
    }
}

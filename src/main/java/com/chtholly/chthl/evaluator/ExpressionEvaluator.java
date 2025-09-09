package com.chtholly.chthl.evaluator;

import com.chtholly.chthl.ast.ElementNode;
import com.chtholly.chthl.ast.expr.*;

/**
 * Evaluates an Expression AST and produces a final value.
 * For this initial version, it converts the expression back into a string representation.
 * Later, this will handle arithmetic, variable lookups, and conditionals.
 */
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ExpressionEvaluator implements com.chtholly.chthl.ast.expr.Visitor<String> {

    private final ElementNode context;

    public ExpressionEvaluator(ElementNode context) {
        this.context = context;
    }

    private static UnitValue parseUnitValue(String literal) {
        if (literal == null || literal.trim().isEmpty()) {
            return null;
        }

        Pattern pattern = Pattern.compile("^([\\-+]?[0-9]*\\.?[0-9]+)(.*)$");
        Matcher matcher = pattern.matcher(literal.trim());

        if (matcher.matches()) {
            double value = Double.parseDouble(matcher.group(1));
            String unit = matcher.group(2).trim();
            return new UnitValue(value, unit);
        }

        // Return null if it's not a parsable numeric value (e.g., "red", "auto").
        return null;
    }

    public String evaluate(Expression expr) {
        return expr.accept(this);
    }

    @Override
    public String visitBinaryExpr(BinaryExpr expr) {
        String leftStr = evaluate(expr.left);
        String rightStr = evaluate(expr.right);

        UnitValue leftUnit = parseUnitValue(leftStr);
        UnitValue rightUnit = parseUnitValue(rightStr);

        // If either side is not a number (e.g., "auto") or units are incompatible, fallback to string concat.
        if (leftUnit == null || rightUnit == null || (!leftUnit.unit.equals(rightUnit.unit) && !leftUnit.unit.isEmpty() && !rightUnit.unit.isEmpty())) {
            return leftStr + " " + expr.operator.getLexeme() + " " + rightStr;
        }

        double resultValue;
        switch (expr.operator.getType()) {
            case PLUS:
                resultValue = leftUnit.value + rightUnit.value;
                break;
            case MINUS:
                resultValue = leftUnit.value - rightUnit.value;
                break;
            case STAR:
                resultValue = leftUnit.value * rightUnit.value;
                break;
            case SLASH:
                if (rightUnit.value == 0) {
                    // Division by zero, return 0 with unit as a safe default.
                    return "0" + leftUnit.unit;
                }
                resultValue = leftUnit.value / rightUnit.value;
                break;
            default:
                // Fallback for unsupported operators (e.g., logical ops for now)
                return leftStr + " " + expr.operator.getLexeme() + " " + rightStr;
        }

        String resultUnit = leftUnit.unit.isEmpty() ? rightUnit.unit : leftUnit.unit;
        return new UnitValue(resultValue, resultUnit).toString();
    }

    @Override
    public String visitGroupingExpr(GroupingExpr expr) {
        // The parentheses are part of the structure, not the output string.
        return evaluate(expr.expression);
    }

    @Override
    public String visitLiteralExpr(LiteralExpr expr) {
        // For string literals, return the content. For others, use toString().
        if (expr.value instanceof String) {
            return (String) expr.value;
        }
        return expr.value.toString();
    }

    @Override
    public String visitVariableExpr(VariableExpr expr) {
        // This handles IDENTIFIER tokens from the expression.
        // For now, we treat them as literal string values (e.g., "100px", "red", "width").
        // The logic to differentiate a variable lookup from a literal value will be added later.
        return expr.name.getLexeme();
    }
}

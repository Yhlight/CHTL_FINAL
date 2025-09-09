package com.chtholly.chthl.evaluator;

import com.chtholly.chthl.ast.ElementNode;
import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.ast.StyleBlockNode;
import com.chtholly.chthl.ast.StylePropertyNode;
import com.chtholly.chthl.ast.expr.*;
import com.chtholly.chthl.ast.template.TemplateNode;
import com.chtholly.chthl.ast.template.VarTemplateNode;
import com.chtholly.chthl.lexer.Token;

import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

public class ExpressionEvaluator implements com.chtholly.chthl.ast.expr.Visitor<Object> {

    private static final int MAX_EVALUATION_DEPTH = 10;

    private final ElementNode localContext;
    private final List<Node> documentContext;
    private final Map<String, TemplateNode> templateTable;
    private final int evaluationDepth;

    public ExpressionEvaluator(ElementNode localContext, List<Node> documentContext, Map<String, TemplateNode> templateTable) {
        this(localContext, documentContext, templateTable, 0);
    }

    private ExpressionEvaluator(ElementNode localContext, List<Node> documentContext, Map<String, TemplateNode> templateTable, int depth) {
        this.localContext = localContext;
        this.documentContext = documentContext;
        this.templateTable = templateTable;
        this.evaluationDepth = depth;
    }

    private static UnitValue parseUnitValue(Object literal) {
        if (literal == null) return null;
        String s = literal.toString();
        if (s.trim().isEmpty()) return null;

        Pattern pattern = Pattern.compile("^([\\-+]?[0-9]*\\.?[0-9]+)(.*)$");
        Matcher matcher = pattern.matcher(s.trim());
        if (matcher.matches()) {
            return new UnitValue(Double.parseDouble(matcher.group(1)), matcher.group(2).trim());
        }
        return null;
    }

    public Object evaluate(Expression expr) {
        if (evaluationDepth > MAX_EVALUATION_DEPTH) {
            System.err.println("Error: Max evaluation depth exceeded, possible circular reference.");
            return "0";
        }
        return expr.accept(this);
    }

    private boolean isTruthy(Object object) {
        if (object == null) return false;
        if (object instanceof Boolean) return (boolean) object;
        if (object instanceof Number) return ((Number) object).doubleValue() != 0;
        if (object instanceof String) return !((String) object).isEmpty();
        if (object instanceof UnitValue) return ((UnitValue) object).value != 0;
        return true;
    }

    private boolean isEqual(Object a, Object b) {
        if (a == null && b == null) return true;
        if (a == null) return false;
        return a.equals(b);
    }

    @Override
    public Object visitBinaryExpr(BinaryExpr expr) {
        Object leftVal = evaluate(expr.left);
        Object rightVal = evaluate(expr.right);

        switch (expr.operator.getType()) {
            case PIPE_PIPE: return isTruthy(leftVal) || isTruthy(rightVal);
            case AMPERSAND_AMPERSAND: return isTruthy(leftVal) && isTruthy(rightVal);
            case BANG_EQUAL: return !isEqual(leftVal, rightVal);
            case EQUAL_EQUAL: return isEqual(leftVal, rightVal);
        }
        UnitValue leftUnit = parseUnitValue(leftVal);
        UnitValue rightUnit = parseUnitValue(rightVal);
        if (leftUnit == null || rightUnit == null || (!leftUnit.unit.equals(rightUnit.unit) && !leftUnit.unit.isEmpty() && !rightUnit.unit.isEmpty())) {
            return false;
        }
        switch (expr.operator.getType()) {
            case GREATER: return leftUnit.value > rightUnit.value;
            case GREATER_EQUAL: return leftUnit.value >= rightUnit.value;
            case LESS: return leftUnit.value < rightUnit.value;
            case LESS_EQUAL: return leftUnit.value <= rightUnit.value;
            case PLUS: return new UnitValue(leftUnit.value + rightUnit.value, leftUnit.unit);
            case MINUS: return new UnitValue(leftUnit.value - rightUnit.value, leftUnit.unit);
            case STAR: return new UnitValue(leftUnit.value * rightUnit.value, leftUnit.unit);
            case SLASH:
                if (rightUnit.value == 0) return new UnitValue(0, leftUnit.unit);
                return new UnitValue(leftUnit.value / rightUnit.value, leftUnit.unit);
        }
        return null;
    }

    @Override
    public Object visitConditionalExpr(ConditionalExpr expr) {
        Object conditionResult = evaluate(expr.condition);
        if (isTruthy(conditionResult)) {
            return evaluate(expr.thenBranch);
        } else {
            return evaluate(expr.elseBranch);
        }
    }

    @Override
    public Object visitGroupingExpr(GroupingExpr expr) {
        return evaluate(expr.expression);
    }

    @Override
    public Object visitLiteralExpr(LiteralExpr expr) {
        return expr.value;
    }

    @Override
    public Object visitVariableExpr(VariableExpr expr) {
        String varName = expr.name.getLexeme();
        if (localContext != null) {
            Expression propertyExpr = findPropertyInNode(localContext, varName);
            if (propertyExpr != null) {
                ExpressionEvaluator newEvaluator = new ExpressionEvaluator(this.localContext, this.documentContext, this.templateTable, this.evaluationDepth + 1);
                return newEvaluator.evaluate(propertyExpr);
            }
        }
        return varName;
    }

    @Override
    public Object visitReferenceExpr(ReferenceExpr expr) {
        String selectorStr = evaluate(expr.object).toString();
        ElementNode targetNode = findNode(selectorStr);
        if (targetNode == null) return "0";
        String propertyName = expr.property.getLexeme();
        Expression propertyExpr = findPropertyInNode(targetNode, propertyName);
        if (propertyExpr == null) return "0";
        ExpressionEvaluator newEvaluator = new ExpressionEvaluator(targetNode, this.documentContext, this.templateTable, this.evaluationDepth + 1);
        return newEvaluator.evaluate(propertyExpr);
    }

    @Override
    public Object visitCallExpr(CallExpr expr) {
        if (!(expr.callee instanceof VariableExpr)) return "";
        String templateName = ((VariableExpr) expr.callee).name.getLexeme();
        Node template = this.templateTable.get(templateName);
        if (!(template instanceof VarTemplateNode)) return "";
        VarTemplateNode varTemplate = (VarTemplateNode) template;
        if (expr.arguments.size() != 1 || !(expr.arguments.get(0) instanceof VariableExpr)) return "";
        String varName = ((VariableExpr) expr.arguments.get(0)).name.getLexeme();
        Expression valueExpr = varTemplate.variables.get(varName);
        if (valueExpr == null) return "";
        ExpressionEvaluator newEvaluator = new ExpressionEvaluator(null, this.documentContext, this.templateTable, this.evaluationDepth + 1);
        return newEvaluator.evaluate(valueExpr);
    }

    private ElementNode findNode(String selector) {
        for (Node rootNode : this.documentContext) {
            ElementNode found = findNodeRecursive(rootNode, selector);
            if (found != null) return found;
        }
        return null;
    }

    private ElementNode findNodeRecursive(Node node, String selector) {
        if (!(node instanceof ElementNode)) return null;
        ElementNode element = (ElementNode) node;
        boolean matches = false;
        if (selector.startsWith("#")) {
            matches = element.attributes.getOrDefault("id", "").equals(selector.substring(1));
        } else if (selector.startsWith(".")) {
            String className = selector.substring(1);
            String[] classes = element.attributes.getOrDefault("class", "").split("\\s+");
            for (String c : classes) {
                if (c.equals(className)) {
                    matches = true;
                    break;
                }
            }
        } else {
            matches = element.tagName.equalsIgnoreCase(selector);
        }
        if (matches) return element;
        for (Node child : element.children) {
            ElementNode found = findNodeRecursive(child, selector);
            if (found != null) return found;
        }
        return null;
    }

    private Expression findPropertyInNode(ElementNode node, String propertyName) {
        if (node == null) return null;
        for (Node child : node.children) {
            if (child instanceof StyleBlockNode) {
                StyleBlockNode styleNode = (StyleBlockNode) child;
                for (Node propNode : styleNode.directPropertiesAndUsages) {
                    if (propNode instanceof StylePropertyNode) {
                        StylePropertyNode prop = (StylePropertyNode) propNode;
                        if (prop.key.equals(propertyName)) {
                            return prop.value;
                        }
                    }
                }
            }
        }
        return null;
    }
}

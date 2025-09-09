package CHTL.Processor;

import CHTL.Expression.*;
import CHTL.Manage.DefinitionManager;
import CHTL.Node.*;
import Util.DependencyGraph;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

public class ASTProcessor {

    private final DefinitionManager definitionManager;
    private List<BaseNode> ast;

    public ASTProcessor(List<BaseNode> ast, DefinitionManager definitionManager) {
        this.ast = ast;
        this.definitionManager = definitionManager;
    }

    public List<BaseNode> process() {
        expandTemplates(this.ast);
        evaluateAllStyles();
        return this.ast;
    }

    private void expandTemplates(List<BaseNode> nodes) {
        List<BaseNode> newNodes = new ArrayList<>();
        boolean changed = false;
        for(BaseNode node : nodes) {
            if (node instanceof ElementTemplateUsageNode usage) {
                ElementTemplateNode template = (ElementTemplateNode) definitionManager.get(usage.getNamespace(), ElementTemplateNode.class, usage.getTemplateName())
                    .orElseThrow(() -> new RuntimeException("Template not found: " + usage.getTemplateName()));
                List<BaseNode> clonedChildren = new ArrayList<>();
                for (BaseNode child : template.getChildren()) {
                    clonedChildren.add(child.clone());
                }
                applySpecializations(clonedChildren, usage.getSpecializationInstructions());
                newNodes.addAll(clonedChildren);
                changed = true;
            } else {
                if (node instanceof ElementNode element) {
                    expandTemplates(element.getChildren());
                }
                newNodes.add(node);
            }
        }
        if (changed) {
            nodes.clear();
            nodes.addAll(newNodes);
            expandTemplates(nodes);
        }
    }

    private void applySpecializations(List<BaseNode> targetNodes, List<BaseNode> instructions) {
        for (BaseNode inst : instructions) {
            if (inst instanceof InsertInstructionNode ins) {
                List<ElementNode> targets = find(new ArrayList<>(targetNodes), ins.getSelector());
                if(targets.isEmpty()) continue;
                ElementNode target = targets.get(0);
                int index = targetNodes.indexOf(target);
                if (index != -1) {
                    switch (ins.getMode()) {
                        case BEFORE: targetNodes.addAll(index, ins.getNodesToInsert()); break;
                        case AFTER: targetNodes.addAll(index + 1, ins.getNodesToInsert()); break;
                        case REPLACE: targetNodes.remove(index); targetNodes.addAll(index, ins.getNodesToInsert()); break;
                        case AT_TOP: targetNodes.addAll(0, ins.getNodesToInsert()); break;
                        case AT_BOTTOM: targetNodes.addAll(ins.getNodesToInsert()); break;
                    }
                }
            } else if (inst instanceof DeleteInstructionNode del) {
                List<ElementNode> targets = find(new ArrayList<>(targetNodes), del.getSelector());
                targetNodes.removeAll(targets);
            } else if (inst instanceof AddStyleInstructionNode styleInst) {
                List<ElementNode> targets = find(new ArrayList<>(targetNodes), styleInst.getSelector());
                for (ElementNode target : targets) {
                    StyleNode styleNodeToUpdate = target.getChildren().stream()
                        .filter(StyleNode.class::isInstance).map(StyleNode.class::cast).findFirst()
                        .orElseGet(() -> {
                            StyleNode newNode = new StyleNode();
                            target.addChild(newNode);
                            return newNode;
                        });
                    styleInst.getStyleNode().getProperties().forEach(styleNodeToUpdate::addProperty);
                    styleInst.getStyleNode().getUsages().forEach(styleNodeToUpdate::addUsage);
                }
            }
        }
    }

    private void evaluateAllStyles() {
        Map<StyleProperty, String> propertyValues = new HashMap<>();
        Map<StyleProperty, ExpressionNode> propertyExpressions = new HashMap<>();
        discoverProperties(ast, null, propertyValues);
        DependencyGraph<StyleProperty> graph = new DependencyGraph<>();
        for (Map.Entry<StyleProperty, String> entry : propertyValues.entrySet()) {
            StyleProperty currentProp = entry.getKey();
            graph.addNode(currentProp);
            ExpressionNode expression = new ExpressionParser(new ExpressionLexer(entry.getValue()).tokenize()).parse();
            propertyExpressions.put(currentProp, expression);
            for (ReferenceNode ref : findReferences(expression)) {
                List<ElementNode> referencedElements = find(this.ast, ref.getSelector());
                if (!referencedElements.isEmpty()) {
                    graph.addEdge(new StyleProperty(referencedElements.get(0), ref.getProperty()), currentProp);
                }
            }
        }
        List<StyleProperty> order = graph.topologicalSort();
        ExpressionEvaluator evaluator = new ExpressionEvaluator();
        Map<ElementNode, Map<String, String>> elementComputedStyles = new HashMap<>();
        for (StyleProperty prop : order) {
            ExpressionNode expression = propertyExpressions.get(prop);
            if (expression == null) continue;
            Map<String, String> computedForThisElement = elementComputedStyles.computeIfAbsent(prop.parent(), k -> new HashMap<>());
            EvaluationContext context = new EvaluationContext(prop.parent(), this.ast, computedForThisElement, this.definitionManager);
            ExpressionNode result = evaluator.evaluate(expression, context);
            String finalValue = "eval_error";
            if (result instanceof UnitNode un) finalValue = un.getValue() + un.getUnit();
            else if (result instanceof LiteralNode ln) finalValue = ln.getValue().toString();
            if (prop.parent() != null) {
                prop.parent().getChildren().stream()
                    .filter(n -> n instanceof StyleNode).map(n -> (StyleNode)n).findFirst()
                    .ifPresent(s -> s.getProperties().put(prop.propertyName(), finalValue));
            }
            computedForThisElement.put(prop.propertyName(), finalValue);
        }
    }

    private void discoverProperties(List<BaseNode> nodes, ElementNode parent, Map<StyleProperty, String> map) {
        for (BaseNode node : nodes) {
            if (node instanceof ElementNode element) {
                element.getChildren().stream()
                    .filter(StyleNode.class::isInstance).flatMap(n -> ((StyleNode)n).getProperties().entrySet().stream())
                    .forEach(entry -> map.put(new StyleProperty(element, entry.getKey()), entry.getValue()));
                discoverProperties(element.getChildren(), element, map);
            } else if (node instanceof StyleNode styleNode && parent == null) {
                styleNode.getProperties().entrySet().stream()
                    .forEach(entry -> map.put(new StyleProperty(null, entry.getKey()), entry.getValue()));
            }
        }
    }

    private List<ReferenceNode> findReferences(ExpressionNode node) {
        List<ReferenceNode> refs = new ArrayList<>();
        if (node instanceof ReferenceNode ref) refs.add(ref);
        else if (node instanceof BinaryOpNode bn) {
            refs.addAll(findReferences(bn.getLeft()));
            refs.addAll(findReferences(bn.getRight()));
        } else if (node instanceof TernaryNode tn) {
            refs.addAll(findReferences(tn.getCondition()));
            refs.addAll(findReferences(tn.getTrueExpression()));
            if (tn.getFalseExpression() != null) refs.addAll(findReferences(tn.getFalseExpression()));
        }
        return refs;
    }

    public List<ElementNode> find(List<BaseNode> searchContext, String selector) {
        String[] parts = selector.trim().split("\\s+");
        List<ElementNode> currentMatches = findElements(searchContext, parts[0]);
        for (int i = 1; i < parts.length; i++) {
            List<ElementNode> childrenContext = new ArrayList<>();
            for(ElementNode parent : currentMatches) {
                childrenContext.addAll(parent.getChildren());
            }
            currentMatches = findElements(childrenContext, parts[i]);
        }
        return currentMatches;
    }

    private List<ElementNode> findElements(List<BaseNode> nodes, String selectorPart) {
        List<ElementNode> found = new ArrayList<>();
        Pattern indexed = Pattern.compile("(\\w+)\\[(\\d+)\\]");
        Matcher m = indexed.matcher(selectorPart);

        if (m.matches()) {
            String tagName = m.group(1);
            int index = Integer.parseInt(m.group(2));
            int count = 0;
            for (BaseNode node : nodes) {
                if (node instanceof ElementNode element && element.getTagName().equals(tagName)) {
                    if (count == index) {
                        found.add(element);
                        return found;
                    }
                    count++;
                }
            }
        } else {
            for (BaseNode node : nodes) {
                if (node instanceof ElementNode element) {
                    if (matches(element, selectorPart)) {
                        found.add(element);
                    }
                    found.addAll(findElements(element.getChildren(), selectorPart));
                }
            }
        }
        return found;
    }

    private boolean matches(ElementNode element, String selectorPart) {
        if (selectorPart.startsWith(".")) {
            String className = selectorPart.substring(1);
            Object classAttr = element.getAttributes().get("class");
            return classAttr instanceof String && List.of(((String) classAttr).split("\\s+")).contains(className);
        } else if (selectorPart.startsWith("#")) {
            String id = selectorPart.substring(1);
            Object idAttr = element.getAttributes().get("id");
            return idAttr instanceof String && idAttr.equals(id);
        } else {
            return element.getTagName().equals(selectorPart);
        }
    }
}

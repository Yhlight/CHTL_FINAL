package CHTL.Processor;

import CHTL.Expression.*;
import CHTL.Manage.DefinitionManager;
import CHTL.Node.*;
import Util.DependencyGraph;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ASTProcessor {

    private final DefinitionManager definitionManager;
    private List<BaseNode> ast;

    public ASTProcessor(List<BaseNode> ast, DefinitionManager definitionManager) {
        this.ast = ast;
        this.definitionManager = definitionManager;
    }

    public List<BaseNode> process() {
        // 1. Expand all element templates recursively.
        expandTemplates(ast);

        // 2. Evaluate all style expressions.
        evaluateAllStyles();

        return ast;
    }

    private void expandTemplates(List<BaseNode> nodes) {
        List<BaseNode> newNodes = new ArrayList<>();
        boolean changed = false;
        for(BaseNode node : nodes) {
            if (node instanceof ElementTemplateUsageNode usage) {
                // Get the template definition
                ElementTemplateNode template = (ElementTemplateNode) definitionManager.get(usage.getNamespace(), ElementTemplateNode.class, usage.getTemplateName())
                    .orElseThrow(() -> new RuntimeException("Template not found: " + usage.getTemplateName()));

                // Clone the children from the template
                List<BaseNode> clonedChildren = new ArrayList<>();
                for (BaseNode child : template.getChildren()) {
                    clonedChildren.add(cloneNode(child)); // Deep clone
                }

                // Apply specializations
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
            // Repeat until no more templates are expanded
            expandTemplates(nodes);
        }
    }

    private void applySpecializations(List<BaseNode> targetNodes, List<BaseNode> instructions) {
        for (BaseNode inst : instructions) {
            if (inst instanceof InsertInstructionNode ins) {
                List<ElementNode> targets = findInContext(new ArrayList<>(targetNodes), ins.getSelector());
                if(targets.isEmpty()) continue;
                ElementNode target = targets.get(0); // Simplified: assumes first match
                int index = targetNodes.indexOf(target);
                if (index != -1) {
                    switch(ins.getMode()) {
                        case BEFORE: targetNodes.addAll(index, ins.getNodesToInsert()); break;
                        case AFTER: targetNodes.addAll(index + 1, ins.getNodesToInsert()); break;
                        case REPLACE:
                            targetNodes.remove(index);
                            targetNodes.addAll(index, ins.getNodesToInsert());
                            break;
                    }
                }
            } else if (inst instanceof DeleteInstructionNode del) {
                targetNodes.removeIf(n -> {
                    if (n instanceof ElementNode el) {
                        // This is a simplified match, a full selector engine would be needed here
                        return el.getTagName().equals(del.getSelector());
                    }
                    return false;
                });
            } else if (inst instanceof AddStyleInstructionNode styleInst) {
                List<ElementNode> targets = findInContext(new ArrayList<>(targetNodes), styleInst.getSelector());
                for (ElementNode target : targets) {
                    // Find an existing style node or create a new one
                    StyleNode styleNodeToUpdate = null;
                    for(BaseNode child : target.getChildren()) {
                        if (child instanceof StyleNode) {
                            styleNodeToUpdate = (StyleNode) child;
                            break;
                        }
                    }
                    if (styleNodeToUpdate == null) {
                        styleNodeToUpdate = new StyleNode();
                        target.addChild(styleNodeToUpdate);
                    }
                    // Add properties from the instruction's style node
                    styleInst.getStyleNode().getProperties().forEach(styleNodeToUpdate::addProperty);
                }
            }
        }
    }

    private void evaluateAllStyles() {
        Map<StyleProperty, String> propertyValues = new HashMap<>();
        Map<StyleProperty, ExpressionNode> propertyExpressions = new HashMap<>();

        // 1. Discover all properties
        discoverProperties(ast, null, propertyValues);

        // 2. Build dependency graph
        DependencyGraph<StyleProperty> graph = new DependencyGraph<>();
        for (Map.Entry<StyleProperty, String> entry : propertyValues.entrySet()) {
            StyleProperty currentProp = entry.getKey();
            graph.addNode(currentProp);
            ExpressionNode expression = new ExpressionParser(new ExpressionLexer(entry.getValue()).tokenize()).parse();
            propertyExpressions.put(currentProp, expression);
            for (ReferenceNode ref : findReferences(expression)) {
                // Simplified findElement
                ElementNode referencedElement = findElement(ref.getSelector());
                if (referencedElement != null) {
                    graph.addEdge(new StyleProperty(referencedElement, ref.getProperty()), currentProp);
                }
            }
        }

        // 3. Evaluate in order
        List<StyleProperty> order = graph.topologicalSort();
        ExpressionEvaluator evaluator = new ExpressionEvaluator();
        Map<ElementNode, Map<String, String>> elementComputedStyles = new HashMap<>();

        for (StyleProperty prop : order) {
            ExpressionNode expression = propertyExpressions.get(prop);
            if (expression == null) continue;

            Map<String, String> computedForThisElement = elementComputedStyles.computeIfAbsent(prop.parent(), k -> new HashMap<>());
            EvaluationContext context = new EvaluationContext(prop.parent(), this.ast, computedForThisElement);
            ExpressionNode result = evaluator.evaluate(expression, context);
            String finalValue = result.toString(); // Simplified

            // Update the actual AST node
            prop.parent().getChildren().stream()
                .filter(n -> n instanceof StyleNode)
                .map(n -> (StyleNode)n)
                .findFirst()
                .ifPresent(s -> s.getProperties().put(prop.propertyName(), finalValue));

            // Store the computed value for self-referencing
            computedForThisElement.put(prop.propertyName(), finalValue);
        }
    }

    // Simplified helper methods from before
    private void discoverProperties(List<BaseNode> nodes, ElementNode parent, Map<StyleProperty, String> map) {
        for (BaseNode node : nodes) {
            if (node instanceof ElementNode element) {
                // Discover properties in local style blocks
                element.getChildren().stream()
                    .filter(n -> n instanceof StyleNode)
                    .flatMap(n -> ((StyleNode)n).getProperties().entrySet().stream())
                    .forEach(entry -> map.put(new StyleProperty(element, entry.getKey()), entry.getValue()));
                // Recurse into children
                discoverProperties(element.getChildren(), element, map);
            } else if (node instanceof StyleNode styleNode && parent == null) {
                // Discover properties in global style blocks
                styleNode.getProperties().entrySet().stream()
                    .forEach(entry -> map.put(new StyleProperty(null, entry.getKey()), entry.getValue()));
            }
        }
    }

    private List<ReferenceNode> findReferences(ExpressionNode node) {
        List<ReferenceNode> refs = new ArrayList<>();
        if (node instanceof ReferenceNode ref) {
            refs.add(ref);
        } else if (node instanceof BinaryOpNode bn) {
            refs.addAll(findReferences(bn.getLeft()));
            refs.addAll(findReferences(bn.getRight()));
        } else if (node instanceof TernaryNode tn) {
            refs.addAll(findReferences(tn.getCondition()));
            refs.addAll(findReferences(tn.getTrueExpression()));
            if (tn.getFalseExpression() != null) {
                refs.addAll(findReferences(tn.getFalseExpression()));
            }
        }
        return refs;
    }

    private ElementNode findElement(String selector) {
        if (selector != null && selector.startsWith("#")) {
            return findElementById(this.ast, selector.substring(1));
        }
        return null;
    }

    private List<ElementNode> findInContext(List<BaseNode> nodes, String part) {
        List<ElementNode> results = new ArrayList<>();
        // This is a simplified implementation that only handles tag names.
        for (BaseNode node : nodes) {
            if (node instanceof ElementNode element) {
                if (element.getTagName().equals(part)) {
                    results.add(element);
                }
                // Recurse
                results.addAll(findInContext(element.getChildren(), part));
            }
        }
        return results;
    }

    private ElementNode findElementById(List<BaseNode> nodes, String id) {
         for (BaseNode node : nodes) {
            if (node instanceof ElementNode element) {
                Object elementId = element.getAttributes().get("id");
                if (elementId instanceof String && elementId.equals(id)) {
                    return element;
                }
                ElementNode foundInChildren = findElementById(element.getChildren(), id);
                if (foundInChildren != null) return foundInChildren;
            }
        }
        return null;
    }

    private BaseNode cloneNode(BaseNode node) {
        if (node == null) return null;
        return node.clone();
    }
}

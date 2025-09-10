package com.chtholly.chthl.generator;

import com.chtholly.chthl.ast.*;
import com.chtholly.chthl.ast.custom.ModificationNode;
import com.chtholly.chthl.ast.custom.SetNode;
import com.chtholly.chthl.ast.template.*;
import com.chtholly.chthl.evaluator.ExpressionEvaluator;
import com.chtholly.chthl.evaluator.UnitValue;
import com.chtholly.chthl.specializer.TemplateSpecializer;

import java.util.*;
import java.util.stream.Collectors;

public class CHTLGenerator implements Visitor<String> {

    private static final Set<String> SELF_CLOSING_TAGS = Set.of(
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    );

    private final StringBuilder globalCssBuilder;
    private final List<Node> rootNodes;
    private final Map<String, TemplateNode> templateTable;
    private final TemplateSpecializer specializer;
    private final Deque<Map<String, UnitValue>> variableContextStack = new ArrayDeque<>();
    private ElementNode currentElement = null;

    public CHTLGenerator(List<Node> rootNodes, Map<String, TemplateNode> templateTable) {
        this.globalCssBuilder = new StringBuilder();
        this.rootNodes = rootNodes;
        this.templateTable = templateTable;
        this.specializer = new TemplateSpecializer();
    }

    public CompilationResult generate() {
        StringBuilder htmlBuilder = new StringBuilder();
        for (Node node : this.rootNodes) {
            htmlBuilder.append(node.accept(this));
        }
        return new CompilationResult(htmlBuilder.toString(), globalCssBuilder.toString());
    }

    private String resolveSelector(String selector, ElementNode parent) {
        if (!selector.contains("&")) return selector;
        if (parent == null) return selector;
        String parentSelector;
        if (parent.attributes.containsKey("id")) {
            parentSelector = "#" + parent.attributes.get("id");
        } else if (parent.attributes.containsKey("class")) {
            parentSelector = "." + parent.attributes.get("class").split("\\s+")[0];
        } else {
            parentSelector = parent.tagName;
        }
        return selector.replace("&", parentSelector);
    }

    @Override
    public String visitElementNode(ElementNode node) {
        ElementNode previousElement = this.currentElement;
        this.currentElement = node;
        StringBuilder builder = new StringBuilder();
        List<String> styleProps = new ArrayList<>();

        for (Node child : node.children) {
            if (child instanceof StyleBlockNode) {
                StyleBlockNode styleNode = (StyleBlockNode) child;
                for (Node propOrUsage : styleNode.directPropertiesAndUsages) {
                    styleProps.add(visitStyleTemplateBodyNode(propOrUsage));
                }
                for (SelectorBlockNode selectorBlock : styleNode.selectorBlocks) {
                    String selector = selectorBlock.selector.trim();
                     if (selector.startsWith(".")) {
                        String className = selector.substring(1).split("[\\s:]")[0];
                        node.attributes.compute("class", (k, v) -> (v == null) ? className : v + " " + className);
                    } else if (selector.startsWith("#")) {
                        if (!node.attributes.containsKey("id")) {
                            node.attributes.put("id", selector.substring(1).split("[\\s:]")[0]);
                        }
                    }
                }
                child.accept(this);
            }
        }

        builder.append("<").append(node.tagName);
        Map<String, String> sortedAttributes = new TreeMap<>(node.attributes);
        for (Map.Entry<String, String> attribute : sortedAttributes.entrySet()) {
            builder.append(" ").append(attribute.getKey()).append("=\"").append(attribute.getValue()).append("\"");
        }

        String styleAttr = styleProps.stream()
                .filter(s -> s != null && !s.trim().isEmpty())
                .map(s -> s.endsWith(";") ? s.substring(0, s.length() - 1) : s)
                .collect(Collectors.joining("; "));

        if (!styleAttr.isEmpty()) {
            builder.append(" style=\"").append(styleAttr).append("\"");
        }

        List<Node> contentChildren = node.children.stream()
                .filter(child -> !(child instanceof StyleBlockNode))
                .collect(Collectors.toList());

        if (contentChildren.isEmpty() && SELF_CLOSING_TAGS.contains(node.tagName.toLowerCase())) {
            builder.append(">");
        } else {
            builder.append(">");
            for (Node child : contentChildren) {
                builder.append(child.accept(this));
            }
            builder.append("</").append(node.tagName).append(">");
        }

        this.currentElement = previousElement;
        return builder.toString();
    }

    @Override
    public String visitTextNode(TextNode node) {
        return node.text;
    }

    @Override
    public String visitCommentNode(CommentNode node) {
        return "<!-- " + node.text + " -->";
    }

    @Override
    public String visitStyleBlockNode(StyleBlockNode node) {
        for (SelectorBlockNode selectorBlock : node.selectorBlocks) {
            selectorBlock.accept(this);
        }
        return "";
    }

    @Override
    public String visitSelectorBlockNode(SelectorBlockNode node) {
        String resolvedSelector = resolveSelector(node.selector, this.currentElement);
        globalCssBuilder.append(resolvedSelector).append(" {\n");
        for (Node propOrUsage : node.body) {
            String expanded = visitStyleTemplateBodyNode(propOrUsage);
            String[] props = expanded.split(";");
            for (String prop : props) {
                if (!prop.trim().isEmpty()) {
                    globalCssBuilder.append("    ").append(prop.trim()).append(";\n");
                }
            }
        }
        globalCssBuilder.append("}\n");
        return "";
    }

    @Override
    public String visitTemplateUsageNode(TemplateUsageNode node) {
        TemplateNode templateDef = this.templateTable.get(node.name.getLexeme());
        if (templateDef == null) return "";

        boolean hasVariableSpecialization = node.customization != null && node.customization.modifications.stream().anyMatch(m -> m instanceof SetNode);

        if (hasVariableSpecialization) {
            Map<String, UnitValue> specializedVariables = new HashMap<>();

            for (ModificationNode mod : node.customization.modifications) {
                if (mod instanceof SetNode) {
                    SetNode setNode = (SetNode) mod;

                    // Create a temporary stack for this evaluation that includes the variables we've set so far
                    Deque<Map<String, UnitValue>> evalStack = new ArrayDeque<>();
                    evalStack.push(specializedVariables);
                    evalStack.addAll(this.variableContextStack);

                    ExpressionEvaluator setEvaluator = new ExpressionEvaluator(this.currentElement, this.rootNodes, this.templateTable, evalStack);

                    Object value = setEvaluator.evaluate(setNode.value);
                    if (value instanceof UnitValue) {
                        specializedVariables.put(setNode.name.getLexeme(), (UnitValue) value);
                    } else if (value != null) {
                         specializedVariables.put(setNode.name.getLexeme(), UnitValue.fromString(value.toString()));
                    }
                }
            }
            this.variableContextStack.push(specializedVariables);
        }

        String result = "";
        if (templateDef instanceof ElementTemplateNode) {
            List<Node> specializedBody = specializer.specialize(((ElementTemplateNode) templateDef).body, node.customization);
            StringBuilder builder = new StringBuilder();
            for (Node bodyNode : specializedBody) {
                builder.append(bodyNode.accept(this));
            }
            result = builder.toString();
        } else if (templateDef instanceof StyleTemplateNode) {
             List<Node> specializedBody = specializer.specialize(((StyleTemplateNode) templateDef).body, node.customization);
             StringBuilder builder = new StringBuilder();
             for(Node bodyNode : specializedBody) {
                 builder.append(visitStyleTemplateBodyNode(bodyNode));
             }
             result = builder.toString();
        }

        if (hasVariableSpecialization) {
            this.variableContextStack.pop();
        }

        return result;
    }

    private String visitStyleTemplateBodyNode(Node bodyNode) {
        if (bodyNode instanceof StylePropertyNode) {
            StylePropertyNode prop = (StylePropertyNode) bodyNode;
            ExpressionEvaluator evaluator = new ExpressionEvaluator(this.currentElement, this.rootNodes, this.templateTable, this.variableContextStack);
            Object value = evaluator.evaluate(prop.value);
            return prop.key + ": " + value.toString() + ";";
        } else if (bodyNode instanceof TemplateUsageNode) {
            return bodyNode.accept(this);
        }
        return "";
    }

    @Override
    public String visitStyleTemplateNode(StyleTemplateNode node) {
        StringBuilder builder = new StringBuilder();
        for (Node bodyNode : node.body) {
            builder.append(visitStyleTemplateBodyNode(bodyNode));
        }
        return builder.toString();
    }

    @Override
    public String visitElementTemplateNode(ElementTemplateNode node) { return ""; }
}

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

import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;

public class CHTLGenerator implements Visitor<String> {

    public static class CompilationError extends RuntimeException {
        public CompilationError(String message) {
            super(message);
        }
    }

    private static final Set<String> SELF_CLOSING_TAGS = Set.of(
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    );

    private static final String GLOBAL_NAMESPACE = "__global__";

    private final StringBuilder globalCssBuilder;
    private final List<Node> rootNodes;
    private final Map<String, Map<String, TemplateNode>> templateTable;
    private final Map<String, Map<String, OriginNode>> originTable;
    private final TemplateSpecializer specializer;
    private final Deque<Map<String, UnitValue>> variableContextStack = new ArrayDeque<>();
    private String currentNamespace = GLOBAL_NAMESPACE;
    private ElementNode currentElement = null;

    public CHTLGenerator(List<Node> rootNodes, Map<String, Map<String, TemplateNode>> templateTable, Map<String, Map<String, OriginNode>> originTable, com.chtholly.chthl.CHTLConfig config) {
        this.globalCssBuilder = new StringBuilder();
        this.rootNodes = rootNodes;
        this.templateTable = templateTable;
        this.originTable = originTable;
        this.specializer = new TemplateSpecializer(config);
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

        // Enforce constraints before rendering children
        if (node.constraints != null && !node.constraints.isEmpty()) {
            for (Node child : contentChildren) {
                checkConstraints(child, node.constraints, node.tagName);
            }
        }

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
    public String visitNamespaceNode(NamespaceNode node) {
        String previousNamespace = this.currentNamespace;
        this.currentNamespace = node.name;
        StringBuilder builder = new StringBuilder();
        for (Node child : node.children) {
            builder.append(child.accept(this));
        }
        this.currentNamespace = previousNamespace;
        return builder.toString();
    }

    private TemplateNode resolveTemplate(TemplateUsageNode usageNode) {
        String name = usageNode.name.getLexeme();
        if (usageNode.fromNamespace != null) {
            String namespace = usageNode.fromNamespace.getLexeme();
            return templateTable.getOrDefault(namespace, Collections.emptyMap()).get(name);
        } else {
            TemplateNode node = templateTable.getOrDefault(currentNamespace, Collections.emptyMap()).get(name);
            if (node != null) return node;
            return templateTable.getOrDefault(GLOBAL_NAMESPACE, Collections.emptyMap()).get(name);
        }
    }

    private OriginNode resolveOrigin(OriginNode usageNode) {
        String name = usageNode.name.getLexeme();
        if (usageNode.fromNamespace != null) {
            String namespace = usageNode.fromNamespace.getLexeme();
            return originTable.getOrDefault(namespace, Collections.emptyMap()).get(name);
        } else {
            OriginNode node = originTable.getOrDefault(currentNamespace, Collections.emptyMap()).get(name);
            if (node != null) return node;
            return originTable.getOrDefault(GLOBAL_NAMESPACE, Collections.emptyMap()).get(name);
        }
    }

    @Override
    public String visitOriginNode(OriginNode node) {
        if (node.isUsage) {
            OriginNode definition = resolveOrigin(node);
            return (definition != null) ? definition.content : "";
        }
        return node.content;
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
        TemplateNode templateDef = resolveTemplate(node);
        if (templateDef == null) {
            System.err.println("Warning: Template '" + node.name.getLexeme() + "' not found.");
            return "";
        }

        boolean hasVariableSpecialization = node.customization != null && node.customization.modifications.stream().anyMatch(m -> m instanceof SetNode);

        if (hasVariableSpecialization) {
            Map<String, UnitValue> specializedVariables = new HashMap<>();

            for (Node mod : node.customization.modifications) {
                if (mod instanceof SetNode) {
                    SetNode setNode = (SetNode) mod;

                    Deque<Map<String, UnitValue>> evalStack = new ArrayDeque<>();
                    evalStack.push(specializedVariables);
                    evalStack.addAll(this.variableContextStack);

                    ExpressionEvaluator setEvaluator = new ExpressionEvaluator(this.currentElement, this.rootNodes, this.templateTable, this.currentNamespace, evalStack);

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
             List<Node> specializedBody = specializer.specializeStyle(((StyleTemplateNode) templateDef).body, node.customization);
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
            if (prop.value == null) return "";
            ExpressionEvaluator evaluator = new ExpressionEvaluator(this.currentElement, this.rootNodes, this.templateTable, this.currentNamespace, this.variableContextStack);
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

    private void checkConstraints(Node child, List<List<Token>> constraints, String parentTagName) {
        for (List<Token> constraintTokens : constraints) {
            if (constraintTokens.isEmpty()) continue;

            // For now, we handle simple cases. This can be expanded.
            Token firstToken = constraintTokens.get(0);

            // Case 1: Simple tag name constraint (e.g., "except span;")
            if (firstToken.getType() == TokenType.IDENTIFIER && constraintTokens.size() == 1) {
                if (child instanceof ElementNode && ((ElementNode) child).tagName.equals(firstToken.getLexeme())) {
                    throw new CompilationError("Constraint violation in element '" + parentTagName + "': usage of '" + firstToken.getLexeme() + "' is forbidden.");
                }
            }
            // Case 2: Type constraint (e.g., "except @Html;")
            else if (firstToken.getType() == TokenType.AT_SIGN && constraintTokens.size() == 2) {
                String typeName = constraintTokens.get(1).getLexeme();
                if ("Html".equalsIgnoreCase(typeName) && child instanceof ElementNode) {
                     throw new CompilationError("Constraint violation in element '" + parentTagName + "': usage of HTML elements is forbidden.");
                }
            }
            // Case 3: Bracketed type constraint (e.g., "except [Template];")
            else if (firstToken.getType() == TokenType.LEFT_BRACKET && constraintTokens.size() == 3) {
                 Token typeToken = constraintTokens.get(1);
                 if (typeToken.getType() == TokenType.TEMPLATE && child instanceof TemplateUsageNode) {
                     throw new CompilationError("Constraint violation in element '" + parentTagName + "': usage of [Template] is forbidden.");
                 }
                 if (typeToken.getType() == TokenType.CUSTOM && child instanceof TemplateUsageNode) {
                    // This is a simplification. Should check if the used template is a [Custom] one.
                    throw new CompilationError("Constraint violation in element '" + parentTagName + "': usage of [Custom] is forbidden.");
                 }
            }
        }
    }
}

package com.chtholly.chthl.generator;

import com.chtholly.chthl.ast.*;
import com.chtholly.chthl.ast.expr.Expression;
import com.chtholly.chthl.ast.template.*;
import com.chtholly.chthl.evaluator.ExpressionEvaluator;
import com.chtholly.chthl.specializer.TemplateSpecializer;

import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
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
        String parentSelector = "";
        if (parent.attributes.containsKey("class")) {
            parentSelector = "." + parent.attributes.get("class").split("\\s+")[0];
        } else if (parent.attributes.containsKey("id")) {
            parentSelector = "#" + parent.attributes.get("id");
        } else {
            return selector;
        }
        return selector.replace("&", parentSelector);
    }

    @Override
    public String visitElementNode(ElementNode node) {
        ElementNode previousElement = this.currentElement;
        this.currentElement = node;
        StringBuilder builder = new StringBuilder();
        StringBuilder styleAttrBuilder = new StringBuilder();

        for (Node child : node.children) {
            if (child instanceof StyleBlockNode) {
                StyleBlockNode styleNode = (StyleBlockNode) child;
                for (Node propOrUsage : styleNode.directPropertiesAndUsages) {
                    styleAttrBuilder.append(visitStyleTemplateBodyNode(propOrUsage));
                }
                for (SelectorBlockNode selectorBlock : styleNode.selectorBlocks) {
                    String selector = selectorBlock.selector.trim();
                    if (selector.startsWith(".")) {
                        if (!node.attributes.containsKey("class")) {
                            node.attributes.put("class", selector.substring(1).split("[\\s:]")[0]);
                        }
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
        if (styleAttrBuilder.length() > 0) {
            builder.append(" style=\"").append(styleAttrBuilder.toString()).append("\"");
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

        if (templateDef instanceof ElementTemplateNode) {
            List<Node> specializedBody = specializer.specialize(((ElementTemplateNode) templateDef).body, node.customization);
            StringBuilder builder = new StringBuilder();
            for (Node bodyNode : specializedBody) {
                builder.append(bodyNode.accept(this));
            }
            return builder.toString();
        } else if (templateDef instanceof StyleTemplateNode) {
             List<Node> specializedBody = specializer.specialize(((StyleTemplateNode) templateDef).body, node.customization);
             StringBuilder builder = new StringBuilder();
             for(Node bodyNode : specializedBody) {
                 builder.append(visitStyleTemplateBodyNode(bodyNode));
             }
             return builder.toString();
        }
        return "";
    }

    private String visitStyleTemplateBodyNode(Node bodyNode) {
        if (bodyNode instanceof StylePropertyNode) {
            StylePropertyNode prop = (StylePropertyNode) bodyNode;
            ExpressionEvaluator evaluator = new ExpressionEvaluator(this.currentElement, this.rootNodes, this.templateTable);
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

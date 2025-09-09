package CHTL.CHTLProcessor;

import CHTL.CHTLManage.DefinitionManager;
import CHTL.CHTLNode.*;

import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ASTProcessor {

    private final List<StyleRule> globalStyles = new ArrayList<>();
    private final DefinitionManager definitionManager;

    public ASTProcessor(DefinitionManager definitionManager) {
        this.definitionManager = definitionManager;
    }

    public ProcessedAST process(List<BaseNode> ast) {
        expandElementTemplates(ast);
        processStyles(ast, null);
        return new ProcessedAST(ast, globalStyles);
    }

    private void expandElementTemplates(List<BaseNode> nodes) {
        List<BaseNode> newChildren = new ArrayList<>();
        boolean wasModified = false;
        for (BaseNode child : nodes) {
            if (child instanceof ElementTemplateUsageNode usageNode) {
                wasModified = true;
                definitionManager.getElementTemplate(usageNode.getNamespace(), usageNode.getTemplateName()).ifPresent(template -> {
                    List<BaseNode> clonedNodes = new ArrayList<>();
                    for (BaseNode templateChild : template.getChildren()) {
                        clonedNodes.add(templateChild.clone());
                    }

                    applySpecializations(clonedNodes, usageNode.getSpecializationInstructions());

                    newChildren.addAll(clonedNodes);
                });
            } else {
                newChildren.add(child);
                if (child instanceof ElementNode elementNode) {
                    expandElementTemplates(elementNode.getChildren());
                }
            }
        }
        if (wasModified) {
            nodes.clear();
            nodes.addAll(newChildren);
            expandElementTemplates(nodes);
        }
    }

    private void applySpecializations(List<BaseNode> targetNodes, List<BaseNode> instructions) {
        for (BaseNode instruction : instructions) {
            if (instruction instanceof DeleteInstructionNode deleteInstruction) {
                applyDeleteInstruction(targetNodes, deleteInstruction);
            } else if (instruction instanceof InsertInstructionNode insertInstruction) {
                applyInsertInstruction(targetNodes, insertInstruction);
            }
        }
    }

    private void applyDeleteInstruction(List<BaseNode> targetNodes, DeleteInstructionNode instruction) {
        String selector = instruction.getSelector();
        Pattern indexedSelector = Pattern.compile("(\\w+)\\[(\\d+)\\]");
        Matcher matcher = indexedSelector.matcher(selector);

        if (matcher.matches()) {
            String tagName = matcher.group(1);
            int index = Integer.parseInt(matcher.group(2));
            int count = 0;

            Iterator<BaseNode> iterator = targetNodes.iterator();
            while(iterator.hasNext()) {
                BaseNode node = iterator.next();
                if (node instanceof ElementNode element && element.getTagName().equals(tagName)) {
                    if (count == index) {
                        iterator.remove();
                        break;
                    }
                    count++;
                }
            }
        } else {
            targetNodes.removeIf(node -> node instanceof ElementNode element && element.getTagName().equals(selector));
        }
    }

    private void applyInsertInstruction(List<BaseNode> targetNodes, InsertInstructionNode instruction) {
        List<BaseNode> nodesToInsert = instruction.getNodesToInsert();
        if (instruction.getMode() == InsertInstructionNode.Mode.AT_TOP) {
            targetNodes.addAll(0, nodesToInsert);
            return;
        }
        if (instruction.getMode() == InsertInstructionNode.Mode.AT_BOTTOM) {
            targetNodes.addAll(nodesToInsert);
            return;
        }

        String selector = instruction.getSelector();
        Pattern indexedSelector = Pattern.compile("(\\w+)\\[(\\d+)\\]");
        Matcher matcher = indexedSelector.matcher(selector);

        int targetIndex = -1;

        if (matcher.matches()) {
            String tagName = matcher.group(1);
            int index = Integer.parseInt(matcher.group(2));
            int count = 0;
            for (int i = 0; i < targetNodes.size(); i++) {
                BaseNode node = targetNodes.get(i);
                if (node instanceof ElementNode element && element.getTagName().equals(tagName)) {
                    if (count == index) {
                        targetIndex = i;
                        break;
                    }
                    count++;
                }
            }
        } else {
            for (int i = 0; i < targetNodes.size(); i++) {
                 BaseNode node = targetNodes.get(i);
                 if (node instanceof ElementNode element && element.getTagName().equals(selector)) {
                     targetIndex = i;
                     break;
                 }
            }
        }

        if (targetIndex != -1) {
            switch(instruction.getMode()) {
                case BEFORE:
                    targetNodes.addAll(targetIndex, nodesToInsert);
                    break;
                case AFTER:
                    targetNodes.addAll(targetIndex + 1, nodesToInsert);
                    break;
                case REPLACE:
                    targetNodes.remove(targetIndex);
                    targetNodes.addAll(targetIndex, nodesToInsert);
                    break;
            }
        }
    }

    private void processStyles(List<BaseNode> nodes, ElementNode parent) {
        List<BaseNode> nodesToRemove = new ArrayList<>();
        for (BaseNode node : nodes) {
            if (node instanceof StyleNode styleNode) {
                processStyleNode(styleNode, parent);
                nodesToRemove.add(node);
            } else if (node instanceof ElementNode elementNode) {
                processStyles(elementNode.getChildren(), elementNode);
            }
        }
        nodes.removeAll(nodesToRemove);
    }

    private Map<String, String> getResolvedStyleProperties(StyleTemplateUsageNode usage) {
        String templateName = usage.getTemplateName();
        String namespace = usage.getNamespace();
        Map<String, String> resolvedProperties = new HashMap<>();

        Optional<CustomStyleNode> customStyleOpt = definitionManager.getCustomStyle(namespace, templateName);
        if (customStyleOpt.isPresent()) {
            CustomStyleNode customNode = customStyleOpt.get();
            resolvedProperties.putAll(customNode.getProperties());
            for (String valuelessProp : customNode.getValuelessProperties()) {
                if (usage.getSpecializationProperties().containsKey(valuelessProp)) {
                    resolvedProperties.put(valuelessProp, usage.getSpecializationProperties().get(valuelessProp));
                }
            }
        }

        Optional<StyleTemplateNode> templateOpt = definitionManager.getStyleTemplate(namespace, templateName);
        if (templateOpt.isPresent()) {
            StyleTemplateNode template = templateOpt.get();
            for (String parentName : template.getParentTemplates()) {
                resolvedProperties.putAll(getResolvedStyleProperties(new StyleTemplateUsageNode(parentName, namespace)));
            }
            resolvedProperties.putAll(template.getProperties());
        }

        resolvedProperties.putAll(usage.getSpecializationProperties());

        for (String propToDelete : usage.getDeletedProperties()) {
            resolvedProperties.remove(propToDelete);
        }

        return resolvedProperties;
    }

    private void processStyleNode(StyleNode styleNode, ElementNode parent) {
        if (parent == null) {
            globalStyles.addAll(styleNode.getRules());
            return;
        }

        Map<String, String> allInlineProperties = new HashMap<>();
        for (StyleTemplateUsageNode usage : styleNode.getTemplateUsages()) {
            allInlineProperties.putAll(getResolvedStyleProperties(usage));
        }
        allInlineProperties.putAll(styleNode.getInlineProperties());

        if (!allInlineProperties.isEmpty()) {
            String existingStyle = parent.getAttributes().getOrDefault("style", "");
            StringBuilder newStyleBuilder = new StringBuilder(existingStyle);
            if (!existingStyle.isEmpty() && !existingStyle.trim().endsWith(";")) {
                newStyleBuilder.append(";");
            }
            for (Map.Entry<String, String> entry : allInlineProperties.entrySet()) {
                newStyleBuilder.append(entry.getKey()).append(":").append(entry.getValue()).append(";");
            }
            parent.addAttribute("style", newStyleBuilder.toString());
        }

        String primarySelector = findPrimarySelector(styleNode, parent);

        for (StyleRule rule : styleNode.getRules()) {
            String selector = rule.getSelector().trim();
            String finalSelector;
            if (selector.startsWith("&")) {
                if (primarySelector == null) {
                    finalSelector = "/* CHTL Error: Could not resolve '&' for selector '" + selector + "' */";
                } else {
                    finalSelector = selector.replace("&", primarySelector);
                }
            } else {
                finalSelector = selector;
            }
            StyleRule finalRule = new StyleRule(finalSelector);
            rule.getProperties().forEach(finalRule::addProperty);
            globalStyles.add(finalRule);

            if (selector.startsWith(".")) {
                String className = selector.split("[:\\s]")[0].substring(1);
                String existingClasses = parent.getAttributes().getOrDefault("class", "");
                if (!List.of(existingClasses.split(" ")).contains(className)) {
                    parent.addAttribute("class", (existingClasses.isEmpty() ? "" : existingClasses + " ") + className);
                }
            } else if (selector.startsWith("#")) {
                String idName = selector.split("[:\\s]")[0].substring(1);
                parent.addAttribute("id", idName);
            }
        }
    }

    private String findPrimarySelector(StyleNode styleNode, ElementNode parent) {
        for (StyleRule rule : styleNode.getRules()) {
            String selector = rule.getSelector().trim();
            if (selector.startsWith(".") && !selector.contains(":")) {
                return selector.split("[:\\s]")[0];
            }
        }
        if (parent.getAttributes().containsKey("class")) {
            return "." + parent.getAttributes().get("class").split(" ")[0];
        }
        for (StyleRule rule : styleNode.getRules()) {
            String selector = rule.getSelector().trim();
            if (selector.startsWith("#") && !selector.contains(":")) {
                return selector.split("[:\\s]")[0];
            }
        }
        if (parent.getAttributes().containsKey("id")) {
            return "#" + parent.getAttributes().get("id");
        }
        return null;
    }
}

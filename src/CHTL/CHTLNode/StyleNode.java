package CHTL.CHTLNode;

import java.util.List;
import java.util.Map;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Represents a style {} block in the AST. It can contain both
 * properties to be applied inline to the parent element, and
 * full style rules (e.g., with class/ID selectors) that will be
 * hoisted to a global style sheet.
 */
public class StyleNode implements BaseNode {
    private final Map<String, String> inlineProperties;
    private final List<StyleRule> rules;
    private final List<StyleTemplateUsageNode> templateUsages;

    public StyleNode() {
        this.inlineProperties = new HashMap<>();
        this.rules = new ArrayList<>();
        this.templateUsages = new ArrayList<>();
    }

    public void addInlineProperty(String key, String value) {
        inlineProperties.put(key, value);
    }

    public void addRule(StyleRule rule) {
        rules.add(rule);
    }

    public void addTemplateUsage(StyleTemplateUsageNode usage) {
        templateUsages.add(usage);
    }

    public Map<String, String> getInlineProperties() {
        return inlineProperties;
    }

    public List<StyleRule> getRules() {
        return rules;
    }

    public List<StyleTemplateUsageNode> getTemplateUsages() {
        return templateUsages;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitStyleNode(this);
    }

    @Override
    public BaseNode clone() {
        StyleNode cloned = new StyleNode();
        this.getInlineProperties().forEach(cloned::addInlineProperty);
        this.getRules().forEach(rule -> {
            // Need to deep clone rules too
            StyleRule clonedRule = new StyleRule(rule.getSelector());
            rule.getProperties().forEach(clonedRule::addProperty);
            cloned.addRule(clonedRule);
        });
        this.getTemplateUsages().forEach(usage -> cloned.addTemplateUsage((StyleTemplateUsageNode) usage.clone()));
        return cloned;
    }
}

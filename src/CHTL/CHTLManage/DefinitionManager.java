package CHTL.CHTLManage;

import CHTL.CHTLNode.*;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

public class DefinitionManager {
    public static final String DEFAULT_NAMESPACE = "_default";

    private final Map<String, Map<String, StyleTemplateNode>> styleTemplates = new HashMap<>();
    private final Map<String, Map<String, ElementTemplateNode>> elementTemplates = new HashMap<>();
    private final Map<String, Map<String, VarTemplateNode>> varTemplates = new HashMap<>();

    private final Map<String, Map<String, CustomStyleNode>> customStyles = new HashMap<>();
    private final Map<String, Map<String, CustomElementNode>> customElements = new HashMap<>();
    private final Map<String, Map<String, CustomVarNode>> customVars = new HashMap<>();

    public DefinitionManager() {
        styleTemplates.put(DEFAULT_NAMESPACE, new HashMap<>());
        elementTemplates.put(DEFAULT_NAMESPACE, new HashMap<>());
        varTemplates.put(DEFAULT_NAMESPACE, new HashMap<>());
        customStyles.put(DEFAULT_NAMESPACE, new HashMap<>());
        customElements.put(DEFAULT_NAMESPACE, new HashMap<>());
        customVars.put(DEFAULT_NAMESPACE, new HashMap<>());
    }

    public void registerStyleTemplate(String namespace, StyleTemplateNode node) {
        styleTemplates.computeIfAbsent(namespace, k -> new HashMap<>()).put(node.getTemplateName(), node);
    }
    public void registerElementTemplate(String namespace, ElementTemplateNode node) {
        elementTemplates.computeIfAbsent(namespace, k -> new HashMap<>()).put(node.getTemplateName(), node);
    }
    public void registerVarTemplate(String namespace, VarTemplateNode node) {
        varTemplates.computeIfAbsent(namespace, k -> new HashMap<>()).put(node.getTemplateName(), node);
    }
    public void registerCustomStyle(String namespace, CustomStyleNode node) {
        customStyles.computeIfAbsent(namespace, k -> new HashMap<>()).put(node.getCustomName(), node);
    }
    public void registerCustomElement(String namespace, CustomElementNode node) {
        customElements.computeIfAbsent(namespace, k -> new HashMap<>()).put(node.getCustomName(), node);
    }
    public void registerCustomVar(String namespace, CustomVarNode node) {
        customVars.computeIfAbsent(namespace, k -> new HashMap<>()).put(node.getCustomName(), node);
    }

    public Optional<StyleTemplateNode> getStyleTemplate(String namespace, String name) {
        return Optional.ofNullable(styleTemplates.getOrDefault(namespace, new HashMap<>()).get(name));
    }
    public Optional<ElementTemplateNode> getElementTemplate(String namespace, String name) {
        return Optional.ofNullable(elementTemplates.getOrDefault(namespace, new HashMap<>()).get(name));
    }
    public Optional<VarTemplateNode> getVarTemplate(String namespace, String name) {
        return Optional.ofNullable(varTemplates.getOrDefault(namespace, new HashMap<>()).get(name));
    }

    public Optional<CustomStyleNode> getCustomStyle(String namespace, String name) {
        return Optional.ofNullable(customStyles.getOrDefault(namespace, new HashMap<>()).get(name));
    }
    public Optional<CustomElementNode> getCustomElement(String namespace, String name) {
        return Optional.ofNullable(customElements.getOrDefault(namespace, new HashMap<>()).get(name));
    }
    public Optional<CustomVarNode> getCustomVar(String namespace, String name) {
        return Optional.ofNullable(customVars.getOrDefault(namespace, new HashMap<>()).get(name));
    }
}

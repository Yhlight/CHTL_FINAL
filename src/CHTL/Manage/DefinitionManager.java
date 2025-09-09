package CHTL.Manage;

import CHTL.Node.*;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

public class DefinitionManager {
    public static final String DEFAULT_NAMESPACE = "_default";

    private final Map<String, Map<String, BaseNode>> definitions = new HashMap<>();

    public DefinitionManager() {
        definitions.put(DEFAULT_NAMESPACE, new HashMap<>());
    }

    private String getKey(Class<?> type, String name) {
        return type.getSimpleName() + "::" + name;
    }

    public void register(String namespace, String name, BaseNode node) {
        String key = getKey(node.getClass(), name);
        definitions.computeIfAbsent(namespace, k -> new HashMap<>()).put(key, node);
    }

    public Optional<BaseNode> get(String namespace, Class<?> type, String name) {
        String key = getKey(type, name);
        BaseNode node = definitions.getOrDefault(namespace, new HashMap<>()).get(key);
        if (node != null) {
            return Optional.of(node);
        }
        // Fallback to default namespace
        return Optional.ofNullable(definitions.getOrDefault(DEFAULT_NAMESPACE, new HashMap<>()).get(key));
    }

    public Optional<String> getVariableValue(String namespace, String templateName, String varName) {
        Optional<BaseNode> templateNodeOpt = get(namespace, VarTemplateNode.class, templateName);
        if (templateNodeOpt.isEmpty()) {
            templateNodeOpt = get(namespace, CustomVarNode.class, templateName);
        }

        if (templateNodeOpt.isPresent() && templateNodeOpt.get() instanceof VarTemplateNode) {
            VarTemplateNode varTemplate = (VarTemplateNode) templateNodeOpt.get();
            return Optional.ofNullable(varTemplate.getVariables().get(varName));
        }

        return Optional.empty();
    }
}

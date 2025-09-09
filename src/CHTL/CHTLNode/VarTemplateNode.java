package CHTL.CHTLNode;

import java.util.Map;
import java.util.HashMap;

/**
 * Represents a [Template] @Var declaration in the AST.
 * It holds a named, reusable group of key-value pairs for variable substitution.
 */
public class VarTemplateNode implements TemplateNode {
    private final String templateName;
    private final Map<String, String> variables;

    public VarTemplateNode(String templateName) {
        this.templateName = templateName;
        this.variables = new HashMap<>();
    }

    public void addVariable(String key, String value) {
        this.variables.put(key, value);
    }

    @Override
    public String getTemplateName() {
        return templateName;
    }

    public Map<String, String> getVariables() {
        return variables;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitVarTemplateNode(this);
    }

    @Override
    public BaseNode clone() {
        VarTemplateNode cloned = new VarTemplateNode(this.templateName);
        for (Map.Entry<String, String> entry : this.variables.entrySet()) {
            cloned.addVariable(entry.getKey(), entry.getValue());
        }
        return cloned;
    }
}

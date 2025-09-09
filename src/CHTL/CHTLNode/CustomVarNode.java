package CHTL.CHTLNode;

import java.util.HashMap;
import java.util.Map;

/**
 * Represents a [Custom] @Var declaration in the AST.
 */
public class CustomVarNode implements CustomNode {
    private final String customName;
    private final Map<String, String> variables;

    public CustomVarNode(String customName) {
        this.customName = customName;
        this.variables = new HashMap<>();
    }

    public void addVariable(String key, String value) {
        this.variables.put(key, value);
    }

    @Override
    public String getCustomName() {
        return customName;
    }

    public Map<String, String> getVariables() {
        return variables;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitCustomVarNode(this);
    }

    @Override
    public BaseNode clone() {
        CustomVarNode cloned = new CustomVarNode(this.customName);
        this.variables.forEach(cloned::addVariable);
        return cloned;
    }
}

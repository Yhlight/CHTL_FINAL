package CHTL.Node;

import java.util.Map;
import java.util.HashMap;

public class VarTemplateNode extends TemplateNode {
    private final Map<String, String> variables;

    public VarTemplateNode(String name) {
        super(name);
        this.variables = new HashMap<>();
    }

    public void addVariable(String key, String value) { this.variables.put(key, value); }
    public Map<String, String> getVariables() { return variables; }

    @Override
    public BaseNode clone() {
        VarTemplateNode cloned = new VarTemplateNode(getName());
        this.variables.forEach(cloned::addVariable);
        return cloned;
    }
}

package CHTL.Node;

import java.util.List;
import java.util.ArrayList;

public class ElementTemplateUsageNode implements BaseNode {
    private final String templateName;
    private final String namespace;
    private final List<BaseNode> specializationInstructions;

    public ElementTemplateUsageNode(String templateName, String namespace) {
        this.templateName = templateName;
        this.namespace = namespace;
        this.specializationInstructions = new ArrayList<>();
    }

    public void addInstruction(BaseNode instruction) { this.specializationInstructions.add(instruction); }
    public String getTemplateName() { return templateName; }
    public String getNamespace() { return namespace; }
    public List<BaseNode> getSpecializationInstructions() { return specializationInstructions; }
}

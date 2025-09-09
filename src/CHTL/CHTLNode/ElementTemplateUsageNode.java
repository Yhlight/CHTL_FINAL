package CHTL.CHTLNode;

import java.util.ArrayList;
import java.util.List;

public class ElementTemplateUsageNode implements BaseNode {
    private final String templateName;
    private final String namespace; // Can be null if not specified
    private final List<BaseNode> specializationInstructions;

    public ElementTemplateUsageNode(String templateName, String namespace) {
        this.templateName = templateName;
        this.namespace = namespace;
        this.specializationInstructions = new ArrayList<>();
    }

    public void addInstruction(BaseNode instruction) {
        this.specializationInstructions.add(instruction);
    }

    public String getTemplateName() {
        return templateName;
    }

    public String getNamespace() {
        return namespace;
    }

    public List<BaseNode> getSpecializationInstructions() {
        return specializationInstructions;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitElementTemplateUsageNode(this);
    }

    @Override
    public BaseNode clone() {
        ElementTemplateUsageNode cloned = new ElementTemplateUsageNode(this.templateName, this.namespace);
        for (BaseNode instruction : this.specializationInstructions) {
            cloned.addInstruction(instruction.clone());
        }
        return cloned;
    }
}

package com.chtholly.chthl.ast.custom;

import com.chtholly.chthl.ast.Node;
import java.util.ArrayList;
import java.util.List;

/**
 * Represents the customization block `{...}` that can follow a template usage.
 * It contains a list of modification instructions.
 */
public class CustomizationBlockNode implements Node {
    public final List<ModificationNode> modifications;

    public CustomizationBlockNode(List<ModificationNode> modifications) {
        this.modifications = modifications;
    }

    @Override
    public <R> R accept(com.chtholly.chthl.ast.Visitor<R> visitor) {
        return visitor.visitCustomizationBlockNode(this);
    }

    @Override
    public Node clone() {
        List<ModificationNode> clonedModifications = new ArrayList<>();
        for (ModificationNode mod : this.modifications) {
            clonedModifications.add((ModificationNode) mod.clone());
        }
        return new CustomizationBlockNode(clonedModifications);
    }
}

package com.chtholly.chthl.ast.custom;

import com.chtholly.chthl.ast.Node;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * Represents the customization block `{...}` that can follow a template usage.
 * It contains a list of modification instructions.
 */
public class CustomizationBlockNode implements Node {
    public final List<Node> modifications;

    public CustomizationBlockNode(List<Node> modifications) {
        this.modifications = modifications;
    }

    @Override
    public <R> R accept(com.chtholly.chthl.ast.Visitor<R> visitor) {
        return visitor.visitCustomizationBlockNode(this);
    }

    @Override
    public Node clone() {
        List<Node> clonedModifications = modifications.stream()
                .map(Node::clone)
                .collect(Collectors.toList());
        return new CustomizationBlockNode(clonedModifications);
    }
}

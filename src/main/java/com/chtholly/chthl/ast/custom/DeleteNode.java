package com.chtholly.chthl.ast.custom;

import com.chtholly.chthl.lexer.Token;
import java.util.ArrayList;
import java.util.List;

/**
 * Represents a 'delete' instruction within a customization block.
 * e.g., <code>delete line-height, border;</code>
 */
public class DeleteNode implements ModificationNode {
    // A list of targets to delete. Each target is represented by its own list of tokens.
    // e.g., [[line-height], [border]] or [[@Style, WhiteText]]
    public final List<List<Token>> targets;

    public DeleteNode(List<List<Token>> targets) {
        this.targets = targets;
    }

    @Override
    public <R> R accept(com.chtholly.chthl.ast.Visitor<R> visitor) {
        return visitor.visitDeleteNode(this);
    }

    @Override
    public com.chtholly.chthl.ast.Node clone() {
        // List of lists of tokens. Tokens are immutable, so we just need to clone the lists.
        List<List<Token>> clonedTargets = new ArrayList<>();
        for (List<Token> target : this.targets) {
            clonedTargets.add(new ArrayList<>(target));
        }
        return new DeleteNode(clonedTargets);
    }
}

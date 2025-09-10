package com.chtholly.chthl.ast.custom;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.lexer.Token;
import java.util.ArrayList;
import java.util.List;

/**
 * Represents an 'insert' instruction within a customization block.
 * e.g., <code>insert after div[0] { ... }</code>
 */
public class InsertNode implements ModificationNode {
    public final Token position; // after, before, replace, etc.
    public final List<Token> target; // e.g., [div, [, 0, ]]
    public final List<Node> body;

    public InsertNode(Token position, List<Token> target, List<Node> body) {
        this.position = position;
        this.target = target;
        this.body = body;
    }

    @Override
    public <R> R accept(com.chtholly.chthl.ast.Visitor<R> visitor) {
        return visitor.visitInsertNode(this);
    }

    @Override
    public Node clone() {
        // Tokens are immutable, so a shallow copy of the list is fine.
        List<Token> clonedTarget = new ArrayList<>(this.target);
        List<Node> clonedBody = new ArrayList<>();
        for (Node node : this.body) {
            clonedBody.add(node.clone());
        }
        return new InsertNode(this.position, clonedTarget, clonedBody);
    }
}

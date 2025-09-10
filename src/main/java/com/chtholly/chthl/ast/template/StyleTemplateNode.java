package com.chtholly.chthl.ast.template;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.lexer.Token;
import java.util.ArrayList;
import java.util.List;

/**
 * Represents a [Template] @Style definition in the AST.
 * Its body can contain style properties and other style template usages for inheritance.
 */
public class StyleTemplateNode extends TemplateNode {
    public final List<Node> body;

    public StyleTemplateNode(Token name, List<Node> body) {
        super(name);
        this.body = body;
    }

    @Override
    public <R> R accept(com.chtholly.chthl.ast.Visitor<R> visitor) {
        return visitor.visitStyleTemplateNode(this);
    }

    @Override
    public Node clone() {
        List<Node> clonedBody = new ArrayList<>();
        for (Node node : this.body) {
            clonedBody.add(node.clone());
        }
        return new StyleTemplateNode(this.name, clonedBody);
    }
}

package com.chtholly.chthl.ast.template;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.lexer.Token;
import java.util.List;

/**
 * Represents a [Template] @Element definition in the AST.
 */
public class ElementTemplateNode extends TemplateNode {
    public final List<Node> body;

    public ElementTemplateNode(Token name, List<Node> body) {
        super(name);
        this.body = body;
    }

    @Override
    public <R> R accept(com.chtholly.chthl.ast.Visitor<R> visitor) {
        return visitor.visitElementTemplateNode(this);
    }
}

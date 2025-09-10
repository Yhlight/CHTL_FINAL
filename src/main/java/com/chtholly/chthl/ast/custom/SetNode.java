package com.chtholly.chthl.ast.custom;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.ast.Visitor;
import com.chtholly.chthl.ast.expr.Expression;
import com.chtholly.chthl.lexer.Token;

public class SetNode implements ModificationNode {
    public final Token name;
    public final Expression value;

    public SetNode(Token name, Expression value) {
        this.name = name;
        this.value = value;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitSetNode(this);
    }

    @Override
    public Node clone() {
        return new SetNode(name, value.clone());
    }
}

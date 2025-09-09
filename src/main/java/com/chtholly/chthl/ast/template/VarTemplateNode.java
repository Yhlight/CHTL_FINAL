package com.chtholly.chthl.ast.template;

import com.chtholly.chthl.ast.expr.Expression;
import com.chtholly.chthl.lexer.Token;
import java.util.Map;

/**
 * Represents a [Template] @Var definition in the AST.
 */
public class VarTemplateNode extends TemplateNode {
    public final Map<String, Expression> variables;

    public VarTemplateNode(Token name, Map<String, Expression> variables) {
        super(name);
        this.variables = variables;
    }

    @Override
    public <R> R accept(com.chtholly.chthl.ast.Visitor<R> visitor) {
        return visitor.visitVarTemplateNode(this);
    }
}

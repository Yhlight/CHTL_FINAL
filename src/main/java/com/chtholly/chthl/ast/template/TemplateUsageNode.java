package com.chtholly.chthl.ast.template;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.lexer.Token;

/**
 * Represents the usage of a template in the code, e.g., <code>@Style DefaultText;</code>
 */
public class TemplateUsageNode implements Node {
    public final Token type; // e.g., the @Style token, though it may be just an IDENTIFIER
    public final Token name;

    public TemplateUsageNode(Token type, Token name) {
        this.type = type;
        this.name = name;
    }

    @Override
    public <R> R accept(com.chtholly.chthl.ast.Visitor<R> visitor) {
        return visitor.visitTemplateUsageNode(this);
    }
}

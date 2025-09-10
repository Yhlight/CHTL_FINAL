package com.chtholly.chthl.ast.template;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.lexer.Token;

/**
 * The abstract base class for all template definition nodes in the AST.
 * e.g., [Template] @Style MyTemplate { ... }
 */
public abstract class TemplateNode implements Node {
    public final Token name;

    public TemplateNode(Token name) {
        this.name = name;
    }

    @Override
    public abstract Node clone();
}

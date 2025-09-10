package com.chtholly.chthl.ast.template;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.ast.custom.CustomizationBlockNode;
import com.chtholly.chthl.lexer.Token;

/**
 * Represents the usage of a template in the code, e.g., <code>@Style DefaultText;</code>
 * It may optionally include a customization block.
 */
public class TemplateUsageNode implements Node {
    public final Token type;
    public final Token name;
    public final Token fromNamespace; // Can be null
    public final CustomizationBlockNode customization; // Can be null

    public TemplateUsageNode(Token type, Token name, Token fromNamespace, CustomizationBlockNode customization) {
        this.type = type;
        this.name = name;
        this.fromNamespace = fromNamespace;
        this.customization = customization;
    }

    @Override
    public <R> R accept(com.chtholly.chthl.ast.Visitor<R> visitor) {
        return visitor.visitTemplateUsageNode(this);
    }

    @Override
    public com.chtholly.chthl.ast.Node clone() {
        return new TemplateUsageNode(
            this.type,
            this.name,
            this.fromNamespace,
            this.customization == null ? null : (CustomizationBlockNode) this.customization.clone()
        );
    }
}

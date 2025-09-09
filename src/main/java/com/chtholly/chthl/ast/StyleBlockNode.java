package com.chtholly.chthl.ast;

import com.chtholly.chthl.ast.template.StyleTemplateNode;
import java.util.List;

/**
 * Represents a style block node in the CHTL AST, e.g., <code>style { ... }</code>.
 * This node can contain both direct properties and nested selector blocks.
 * The list of direct properties can also contain template usages.
 */
public class StyleBlockNode implements Node {
    public final List<Node> directPropertiesAndUsages;
    public final List<SelectorBlockNode> selectorBlocks;

    public StyleBlockNode(List<Node> directPropertiesAndUsages, List<SelectorBlockNode> selectorBlocks) {
        this.directPropertiesAndUsages = directPropertiesAndUsages;
        this.selectorBlocks = selectorBlocks;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitStyleBlockNode(this);
    }
}

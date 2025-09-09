package com.chtholly.chthl.ast;

/**
 * The Visitor interface for the AST nodes.
 * This allows for clean separation of operations on the tree structure.
 * @param <R> The return type of the visit methods.
 */
import com.chtholly.chthl.ast.template.ElementTemplateNode;
import com.chtholly.chthl.ast.template.StyleTemplateNode;
import com.chtholly.chthl.ast.template.TemplateUsageNode;
import com.chtholly.chthl.ast.template.VarTemplateNode;

public interface Visitor<R> {
    R visitElementNode(ElementNode node);
    R visitTextNode(TextNode node);
    R visitCommentNode(CommentNode node);

    default R visitStyleBlockNode(StyleBlockNode node) { return null; }
    default R visitStylePropertyNode(StylePropertyNode node) { return null; }
    default R visitSelectorBlockNode(SelectorBlockNode node) { return null; }

    // Template-related nodes
    default R visitStyleTemplateNode(StyleTemplateNode node) { return null; }
    default R visitElementTemplateNode(ElementTemplateNode node) { return null; }
    default R visitVarTemplateNode(VarTemplateNode node) { return null; }
    default R visitTemplateUsageNode(TemplateUsageNode node) { return null; }
}

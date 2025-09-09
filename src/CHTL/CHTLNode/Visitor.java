package CHTL.CHTLNode;

/**
 * Defines the Visitor interface for traversing the CHTL AST.
 * Each node type in the AST will have a corresponding visit method.
 * This allows for clean separation of algorithms (like code generation)
 * from the node classes themselves.
 * @param <R> The return type of the visit methods.
 */
public interface Visitor<R> {
    R visitElementNode(ElementNode node);
    R visitTextNode(TextNode node);
    R visitCommentNode(CommentNode node);
    R visitStyleNode(StyleNode node);
    R visitStyleTemplateNode(StyleTemplateNode node);
    R visitElementTemplateNode(ElementTemplateNode node);
    R visitVarTemplateNode(VarTemplateNode node);
    R visitStyleTemplateUsageNode(StyleTemplateUsageNode node);
    R visitElementTemplateUsageNode(ElementTemplateUsageNode node);
    R visitCustomStyleNode(CustomStyleNode node);
    R visitCustomElementNode(CustomElementNode node);
    R visitCustomVarNode(CustomVarNode node);
    R visitDeleteInstructionNode(DeleteInstructionNode node);
    R visitInsertInstructionNode(InsertInstructionNode node);
    R visitImportNode(ImportNode node);
}

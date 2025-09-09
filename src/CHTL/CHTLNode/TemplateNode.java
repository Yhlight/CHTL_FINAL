package CHTL.CHTLNode;

/**
 * A marker interface for all Template-related nodes in the AST.
 * e.g., StyleTemplateNode, ElementTemplateNode
 */
public interface TemplateNode extends BaseNode {
    String getTemplateName();
}

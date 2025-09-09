package CHTL.CHTLNode;

/**
 * A marker interface for all Custom-related nodes in the AST.
 * e.g., CustomStyleNode, CustomElementNode
 */
public interface CustomNode extends BaseNode {
    String getCustomName();
}

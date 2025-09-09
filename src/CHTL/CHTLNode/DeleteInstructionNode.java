package CHTL.CHTLNode;

/**
 * Represents a 'delete' instruction within an element specialization block.
 * e.g., delete span; or delete div[1];
 */
public class DeleteInstructionNode implements BaseNode {
    private final String selector;

    public DeleteInstructionNode(String selector) {
        this.selector = selector;
    }

    public String getSelector() {
        return selector;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitDeleteInstructionNode(this);
    }

    @Override
    public BaseNode clone() {
        return new DeleteInstructionNode(this.selector);
    }
}

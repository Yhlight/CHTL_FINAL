package CHTL.Node;

public class DeleteInstructionNode implements BaseNode {
    private final String selector;

    public DeleteInstructionNode(String selector) {
        this.selector = selector;
    }

    public String getSelector() {
        return selector;
    }
}

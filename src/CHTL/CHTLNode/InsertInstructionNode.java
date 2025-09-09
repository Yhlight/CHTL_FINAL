package CHTL.CHTLNode;

import java.util.ArrayList;
import java.util.List;

/**
 * Represents an 'insert' instruction within an element specialization block.
 */
public class InsertInstructionNode implements BaseNode {

    public enum Mode {
        AFTER,
        BEFORE,
        REPLACE,
        AT_TOP,
        AT_BOTTOM
    }

    private final Mode mode;
    private final String selector;
    private final List<BaseNode> nodesToInsert;

    public InsertInstructionNode(Mode mode, String selector, List<BaseNode> nodesToInsert) {
        this.mode = mode;
        this.selector = selector;
        this.nodesToInsert = nodesToInsert;
    }

    public InsertInstructionNode(Mode mode, List<BaseNode> nodesToInsert) {
        this(mode, null, nodesToInsert); // For AT_TOP and AT_BOTTOM where selector is optional
    }

    public Mode getMode() {
        return mode;
    }

    public String getSelector() {
        return selector;
    }

    public List<BaseNode> getNodesToInsert() {
        return nodesToInsert;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitInsertInstructionNode(this);
    }

    @Override
    public BaseNode clone() {
        List<BaseNode> clonedNodes = new ArrayList<>();
        for (BaseNode node : this.nodesToInsert) {
            clonedNodes.add(node.clone());
        }
        return new InsertInstructionNode(this.mode, this.selector, clonedNodes);
    }
}

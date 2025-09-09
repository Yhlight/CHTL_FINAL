package CHTL.Node;

import java.util.List;

public class InsertInstructionNode implements BaseNode {
    public enum Mode { AFTER, BEFORE, REPLACE, AT_TOP, AT_BOTTOM }

    private final Mode mode;
    private final String selector; // Can be null for at_top/at_bottom
    private final List<BaseNode> nodesToInsert;

    public InsertInstructionNode(Mode mode, String selector, List<BaseNode> nodesToInsert) {
        this.mode = mode;
        this.selector = selector;
        this.nodesToInsert = nodesToInsert;
    }

    public Mode getMode() { return mode; }
    public String getSelector() { return selector; }
    public List<BaseNode> getNodesToInsert() { return nodesToInsert; }
}

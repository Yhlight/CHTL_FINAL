package CHTL.CHTLNode;

/**
 * Represents a top-level 'use' statement in the AST.
 * e.g., use html5; or use @Config MyConfig;
 */
public class UseNode implements BaseNode {

    private final String target;

    public UseNode(String target) {
        this.target = target;
    }

    public String getTarget() {
        return target;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitUseNode(this);
    }

    @Override
    public BaseNode clone() {
        return new UseNode(this.target);
    }
}

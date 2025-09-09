package CHTL.CHTLNode;

/**
 * Represents a comment node in the AST.
 * This is for comments that are intended to be rendered in the output file,
 * such as the '--' generator comments.
 */
public class CommentNode implements BaseNode {
    private final String comment;

    public CommentNode(String comment) {
        this.comment = comment;
    }

    public String getComment() {
        return comment;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitCommentNode(this);
    }

    @Override
    public BaseNode clone() {
        return new CommentNode(this.comment);
    }
}

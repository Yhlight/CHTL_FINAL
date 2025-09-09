package CHTL.CHTLNode;

/**
 * Represents an [Import] statement in the AST.
 * This node is processed by the parser itself to load other files,
 * and may not exist in the final AST that goes to the processor.
 */
public class ImportNode implements BaseNode {

    private final String path;

    public ImportNode(String path) {
        this.path = path;
    }

    public String getPath() {
        return path;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitImportNode(this);
    }

    @Override
    public BaseNode clone() {
        return new ImportNode(path);
    }
}

package CHTL.Node;

// Represents an [Import] statement
public class ImportNode implements BaseNode {
    // This will be a complex node. For now, a placeholder.
    // It needs to handle all the variations from the spec.
    private final String path;

    public ImportNode(String path) {
        this.path = path;
    }

    public String getPath() {
        return path;
    }

    @Override
    public BaseNode clone() {
        return new ImportNode(path);
    }
}

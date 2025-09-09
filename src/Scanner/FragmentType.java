package Scanner;

public enum FragmentType {
    CHTL,          // Standard CHTL code
    GLOBAL_CSS,    // A top-level style {} block
    GLOBAL_JS,     // A top-level script {} block
    CHTL_JS        // A local script {} block inside an element
}

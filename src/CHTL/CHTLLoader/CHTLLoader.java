package CHTL.CHTLLoader;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

/**
 * Responsible for loading the content of files for the [Import] system.
 */
public class CHTLLoader {

    /**
     * Reads the content of a file given a path.
     * @param path The path to the file.
     * @return The content of the file as a string.
     * @throws IOException If the file cannot be read.
     */
    public String loadFile(String path) throws IOException {
        // This is a simplified implementation. The full implementation will
        // need to handle the complex path resolution logic described in CHTL.md
        // (e.g., searching in module folders).
        return new String(Files.readAllBytes(Paths.get(path)));
    }
}

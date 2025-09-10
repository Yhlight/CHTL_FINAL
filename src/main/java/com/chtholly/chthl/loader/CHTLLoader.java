package com.chtholly.chthl.loader;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

/**
 * Handles loading of CHTL source files from the filesystem.
 */
public class CHTLLoader {

    /**
     * Custom exception for file loading errors.
     */
    public static class LoadError extends RuntimeException {
        public LoadError(String message) {
            super(message);
        }
    }

    /**
     * Loads the content of a CHTL file.
     *
     * @param importPath The path to the file to import, from the import statement.
     * @param currentFilePath The path of the file that contains the import statement. This is needed to resolve relative paths.
     * @return The content of the imported file as a string.
     * @throws LoadError if the file cannot be found or read.
     */
    public String load(String importPath, String currentFilePath) {
        try {
            Path parentDir;
            if (currentFilePath == null || Paths.get(currentFilePath).getParent() == null) {
                // If the base path is not available or has no parent, resolve from the current working directory.
                parentDir = Paths.get("").toAbsolutePath();
            } else {
                parentDir = Paths.get(currentFilePath).getParent();
            }

            Path fileToLoad = parentDir.resolve(importPath).normalize();

            if (!Files.exists(fileToLoad)) {
                throw new LoadError("Cannot find imported file at path: " + fileToLoad);
            }
            if (!Files.isReadable(fileToLoad)) {
                throw new LoadError("Cannot read imported file (check permissions): " + fileToLoad);
            }

            return new String(Files.readAllBytes(fileToLoad), StandardCharsets.UTF_8);
        } catch (IOException e) {
            throw new LoadError("Could not read imported file '" + importPath + "'. Reason: " + e.getMessage());
        } catch (java.nio.file.InvalidPathException e) {
            throw new LoadError("Invalid import path: '" + importPath + "'. Reason: " + e.getMessage());
        }
    }
}

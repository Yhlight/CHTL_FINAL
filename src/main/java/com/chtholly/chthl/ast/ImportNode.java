package com.chtholly.chthl.ast;

import com.chtholly.chthl.lexer.Token;

/**
 * Represents an [Import] node in the CHTL AST.
 * e.g., <code>[Import] @Chtl from "path/to/file.chtl"</code>
 */
public class ImportNode implements Node {

    public final Token type; // e.g., @Chtl
    public final String path;
    // 'as' alias will be added later.

    public ImportNode(Token type, String path) {
        this.type = type;
        this.path = path;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitImportNode(this);
    }

    @Override
    public Node clone() {
        // ImportNode is immutable, so we can return a new instance with the same fields.
        return new ImportNode(this.type, this.path);
    }
}

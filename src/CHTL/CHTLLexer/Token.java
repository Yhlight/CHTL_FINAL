package CHTL.CHTLLexer;

public class Token {
    private final TokenType type;
    private final String value;
    private final int line;
    private final int column;
    private final int startOffset; // Character offset of the start of this token in the source.
    private final int endOffset;   // Character offset of the end of this token in the source.

    public Token(TokenType type, String value, int line, int column, int startOffset, int endOffset) {
        this.type = type;
        this.value = value;
        this.line = line;
        this.column = column;
        this.startOffset = startOffset;
        this.endOffset = endOffset;
    }

    public TokenType getType() {
        return type;
    }

    public String getValue() {
        return value;
    }

    public int getLine() {
        return line;
    }

    public int getColumn() {
        return column;
    }

    public int getStartOffset() {
        return startOffset;
    }

    public int getEndOffset() {
        return endOffset;
    }

    @Override
    public String toString() {
        return "Token{" +
                "type=" + type +
                ", value='" + value + '\'' +
                ", line=" + line +
                ", column=" + column +
                '}';
    }
}

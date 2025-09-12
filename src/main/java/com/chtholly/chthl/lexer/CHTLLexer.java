package com.chtholly.chthl.lexer;

public class CHTLLexer {
    private final String input;
    private int position;      // current position in input (points to current char)
    private int readPosition;  // current reading position in input (after current char)
    private char ch;           // current char under examination
    private int line;          // current line number
    private int column;        // current column number

    public CHTLLexer(String input) {
        this.input = input;
        this.line = 1;
        this.column = 0;
        readChar(); // Initialize ch, position, readPosition, and line numbers
    }

    private void readChar() {
        if (this.ch == '\n') {
            this.line++;
            this.column = 0;
        }

        if (this.readPosition >= this.input.length()) {
            this.ch = 0; // NUL character, signifies EOF
        } else {
            this.ch = this.input.charAt(this.readPosition);
        }
        this.position = this.readPosition;
        this.readPosition++;
        this.column++;
    }

    private char peekChar() {
        if (this.readPosition >= this.input.length()) {
            return 0;
        } else {
            return this.input.charAt(this.readPosition);
        }
    }

    public Token nextToken() {
        Token tok;
        skipWhitespaceAndComments();

        int startLine = this.line;
        int startCol = this.column;

        switch (this.ch) {
            case '=':
                tok = new Token(TokenType.EQUAL, String.valueOf(this.ch), startLine, startCol);
                break;
            case ';':
                tok = new Token(TokenType.SEMICOLON, String.valueOf(this.ch), startLine, startCol);
                break;
            case ':':
                tok = new Token(TokenType.COLON, String.valueOf(this.ch), startLine, startCol);
                break;
            case '{':
                tok = new Token(TokenType.LEFT_BRACE, String.valueOf(this.ch), startLine, startCol);
                break;
            case '}':
                tok = new Token(TokenType.RIGHT_BRACE, String.valueOf(this.ch), startLine, startCol);
                break;
            case '"':
            case '\'':
                String str = readString(this.ch);
                tok = new Token(TokenType.STRING, str, startLine, startCol);
                break;
            case 0:
                tok = new Token(TokenType.EOF, "", startLine, startCol);
                break;
            default:
                if (isLetter(this.ch)) {
                    String literal = readIdentifier();
                    tok = new Token(TokenType.IDENTIFIER, literal, startLine, startCol);
                    return tok; // readIdentifier advances cursor, so return early
                } else {
                    tok = new Token(TokenType.ILLEGAL, String.valueOf(this.ch), startLine, startCol);
                }
                break;
        }

        readChar(); // Move to the next character for single-char tokens
        return tok;
    }

    private void skipWhitespaceAndComments() {
        while (true) {
            if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
                readChar();
            } else if (ch == '/' && peekChar() == '/') {
                skipSingleLineComment();
            } else if (ch == '/' && peekChar() == '*') {
                skipMultiLineComment();
            } else {
                break;
            }
        }
    }

    private void skipSingleLineComment() {
        while (ch != '\n' && ch != 0) {
            readChar();
        }
    }

    private void skipMultiLineComment() {
        readChar(); // consume '/'
        readChar(); // consume '*'
        while (!(ch == '*' && peekChar() == '/')) {
            if (ch == 0) { // Unterminated comment
                return;
            }
            readChar();
        }
        readChar(); // consume '*'
        readChar(); // consume '/'
    }

    private String readIdentifier() {
        int startPosition = this.position;
        while (isLetter(this.ch) || isDigit(this.ch)) {
            readChar();
        }
        return this.input.substring(startPosition, this.position);
    }

    private String readString(char quoteType) {
        int startPosition = this.position + 1; // Skip the opening quote
        do {
            readChar();
            // Note: Does not yet support escape sequences like \"
        } while (this.ch != quoteType && this.ch != 0);
        return this.input.substring(startPosition, this.position);
    }

    private boolean isLetter(char c) {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
    }

    private boolean isDigit(char c) {
        return '0' <= c && c <= '9';
    }
}

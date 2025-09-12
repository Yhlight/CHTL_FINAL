package com.chtholly.chthl.parser;

import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;
import com.chtholly.chthl.node.DocumentNode;
import com.chtholly.chthl.node.ElementNode;
import com.chtholly.chthl.node.Node;
import com.chtholly.chthl.node.TextNode;

import java.util.ArrayList;
import java.util.List;

public class CHTLParser {
    private final CHTLLexer lexer;
    private Token curToken;
    private Token peekToken;
    private final List<String> errors = new ArrayList<>();

    public CHTLParser(CHTLLexer lexer) {
        this.lexer = lexer;
        // Read two tokens, so curToken and peekToken are both set
        nextToken();
        nextToken();
    }

    private void nextToken() {
        this.curToken = this.peekToken;
        this.peekToken = this.lexer.nextToken();
    }

    public DocumentNode parseDocument() {
        DocumentNode document = new DocumentNode();
        while (!curTokenIs(TokenType.EOF)) {
            Node statement = parseStatement();
            if (statement != null) {
                document.addChild(statement);
            }
            nextToken();
        }
        return document;
    }

    private Node parseStatement() {
        if (curTokenIs(TokenType.IDENTIFIER)) {
            if ("text".equals(this.curToken.literal)) {
                return parseTextNode();
            } else {
                return parseElementNode();
            }
        }
        return null;
    }

    private ElementNode parseElementNode() {
        ElementNode node = new ElementNode(this.curToken);

        if (!expectPeek(TokenType.LEFT_BRACE)) {
            return null;
        }

        nextToken(); // Consume '{'

        while (!curTokenIs(TokenType.RIGHT_BRACE) && !curTokenIs(TokenType.EOF)) {
            Node statement = parseStatement();
            if (statement != null) {
                node.addChild(statement);
            }
            nextToken();
        }

        if (!curTokenIs(TokenType.RIGHT_BRACE)) {
            peekError(TokenType.RIGHT_BRACE);
            return null;
        }

        return node;
    }

    private TextNode parseTextNode() {
        // Current token is IDENTIFIER("text")
        if (!expectPeek(TokenType.LEFT_BRACE)) {
            return null;
        }
        nextToken(); // Consume '{'

        if (!curTokenIs(TokenType.STRING)) {
            errors.add("Expected next token to be STRING, got " + curToken.type + " instead.");
            return null;
        }

        TextNode textNode = new TextNode(this.curToken);

        if (!expectPeek(TokenType.RIGHT_BRACE)) {
            return null;
        }

        return textNode;
    }

    private boolean curTokenIs(TokenType t) {
        return this.curToken.type == t;
    }

    private boolean peekTokenIs(TokenType t) {
        return this.peekToken.type == t;
    }

    private boolean expectPeek(TokenType t) {
        if (peekTokenIs(t)) {
            nextToken();
            return true;
        } else {
            peekError(t);
            return false;
        }
    }

    private void peekError(TokenType t) {
        errors.add("Expected next token to be " + t + ", got " + this.peekToken.type + " instead.");
    }

    public List<String> getErrors() {
        return errors;
    }
}

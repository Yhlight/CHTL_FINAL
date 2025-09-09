package CHTL.Parser;

import CHTL.Lexer.CHTLLexer;
import CHTL.Lexer.Token;
import CHTL.Lexer.TokenType;
import CHTL.Node.BaseNode;
import CHTL.Node.ElementNode;
import CHTL.Node.StyleNode;
import CHTL.Node.TextNode;

import java.util.ArrayList;
import java.util.List;

public class CHTLParser {
    private final List<Token> tokens;
    private int current = 0;

    public CHTLParser(List<Token> tokens) {
        this.tokens = tokens;
    }

    public List<BaseNode> parse() {
        List<BaseNode> nodes = new ArrayList<>();
        while (!isAtEnd()) {
            nodes.add(declaration());
        }
        return nodes;
    }

    private BaseNode declaration() {
        if (check(TokenType.IDENTIFIER)) {
            return element();
        }
        if (check(TokenType.TEXT)) {
            return textBlock();
        }
        if (check(TokenType.STYLE)) {
            return parseStyleBlock();
        }
        throw new RuntimeException("Unexpected token: " + peek().value());
    }

    private ElementNode element() {
        Token name = advance();
        ElementNode node = new ElementNode(name.value());
        consume(TokenType.LBRACE, "Expect '{' after element name.");
        while (!check(TokenType.RBRACE)) {
            if (isAtEnd()) throw new RuntimeException("Unterminated element block.");

            if (check(TokenType.STYLE)) {
                node.addChild(parseStyleBlock());
            } else if (check(TokenType.IDENTIFIER) && (lookahead().type() == TokenType.COLON || lookahead().type() == TokenType.EQUALS)) {
                parseAttribute(node);
            } else {
                node.addChild(declaration());
            }
        }
        consume(TokenType.RBRACE, "Expect '}' to close element block.");
        return node;
    }

    private void parseAttribute(ElementNode node) {
        Token key = advance();
        match(TokenType.COLON, TokenType.EQUALS);
        // The lexer produces a single STRING token for the whole raw value
        Token value = consume(TokenType.STRING, "Expect attribute value.");
        node.addAttribute(key.value(), value.value());
        consume(TokenType.SEMICOLON, "Expect ';' after attribute value.");
    }

    private TextNode textBlock() {
        consume(TokenType.TEXT, "Expect 'text' keyword.");
        consume(TokenType.LBRACE, "Expect '{' after 'text'.");
        Token content = consume(TokenType.STRING, "Expect string literal in text block.");
        consume(TokenType.RBRACE, "Expect '}' after text content.");
        return new TextNode(content.value());
    }

    private StyleNode parseStyleBlock() {
        consume(TokenType.STYLE, "Expect 'style' keyword.");
        consume(TokenType.LBRACE, "Expect '{' after 'style'.");
        StyleNode styleNode = new StyleNode();
        while(!check(TokenType.RBRACE)) {
            Token property = consume(TokenType.IDENTIFIER, "Expect property name.");
            match(TokenType.COLON, TokenType.EQUALS);
            // This is where we store the raw expression string
            String value = consume(TokenType.STRING, "Expect property value.").value();
            styleNode.addProperty(property.value(), value);
            consume(TokenType.SEMICOLON, "Expect ';' after property value.");
        }
        consume(TokenType.RBRACE, "Expect '}' to close style block.");
        return styleNode;
    }

    // Utility methods
    private Token peek() { return tokens.get(current); }
    private Token lookahead() { return tokens.get(current + 1); }
    private Token previous() { return tokens.get(current - 1); }
    private boolean isAtEnd() { return peek().type() == TokenType.EOF; }
    private boolean check(TokenType type) { if (isAtEnd()) return false; return peek().type() == type; }
    private Token advance() { if (!isAtEnd()) current++; return previous(); }
    private boolean match(TokenType... types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }
    private Token consume(TokenType type, String message) {
        if (check(type)) return advance();
        throw new RuntimeException(message + " Got " + peek().type() + " instead.");
    }
}

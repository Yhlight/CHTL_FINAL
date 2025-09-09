package CHTL_JS.Parser;

import CHTL_JS.Lexer.Token;
import CHTL_JS.Lexer.TokenType;
import CHTL_JS.Node.CHTLJSBaseNode;
import CHTL_JS.Node.ListenNode;

import java.util.ArrayList;
import java.util.List;

public class CHTLJSParser {
    private final List<Token> tokens;
    private int current = 0;

    public CHTLJSParser(List<Token> tokens) {
        this.tokens = tokens;
    }

    public List<CHTLJSBaseNode> parse() {
        List<CHTLJSBaseNode> nodes = new ArrayList<>();
        while (!isAtEnd()) {
            nodes.add(statement());
        }
        return nodes;
    }

    private CHTLJSBaseNode statement() {
        if (check(TokenType.L_CURLY_BRACE)) {
            return listenStatement();
        }
        throw new RuntimeException("Unexpected token in CHTL JS: " + peek().value());
    }

    private ListenNode listenStatement() {
        consume(TokenType.L_CURLY_BRACE, "Expect '{{' to start a selector.");
        Token selector = consume(TokenType.SELECTOR, "Expect a selector inside braces.");
        consume(TokenType.R_CURLY_BRACE, "Expect '}}' to close a selector.");

        consume(TokenType.ARROW, "Expect '->' after selector.");
        consume(TokenType.LISTEN, "Expect 'listen' keyword.");
        consume(TokenType.LBRACE, "Expect '{' for listen block.");

        ListenNode listenNode = new ListenNode(selector.value());

        while (!check(TokenType.RBRACE)) {
            Token eventName = consume(TokenType.IDENTIFIER, "Expect event name (e.g., click).");
            consume(TokenType.COLON, "Expect ':' after event name.");

            // This is highly simplified. It doesn't parse the JS function body,
            // it just consumes tokens until the next event handler or the end of the block.
            String handlerBody = consumeUntil(TokenType.COMMA, TokenType.RBRACE);
            listenNode.addEventHandler(eventName.value(), handlerBody);

            if (check(TokenType.COMMA)) {
                advance(); // Consume the comma
            }
        }

        consume(TokenType.RBRACE, "Expect '}' to close listen block.");
        return listenNode;
    }

    private String consumeUntil(TokenType... terminators) {
        StringBuilder builder = new StringBuilder();
        while (!isAtEnd()) {
            for (TokenType terminator : terminators) {
                if (check(terminator)) {
                    return builder.toString().trim();
                }
            }
            builder.append(peek().value()).append(" ");
            advance();
        }
        return builder.toString().trim();
    }

    // Utility methods
    private Token peek() { return tokens.get(current); }
    private Token previous() { return tokens.get(current - 1); }
    private boolean isAtEnd() { return peek().type() == TokenType.EOF; }
    private boolean check(TokenType type) { if (isAtEnd()) return false; return peek().type() == type; }
    private Token advance() { if (!isAtEnd()) current++; return previous(); }
    private Token consume(TokenType type, String message) {
        if (check(type)) return advance();
        throw new RuntimeException(message + " Got " + peek().type() + " instead.");
    }
}

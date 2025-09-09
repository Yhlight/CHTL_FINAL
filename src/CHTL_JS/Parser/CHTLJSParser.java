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
            // This is a statement starting with a selector, e.g., {{...}} -> listen
            String selector = parseSelector();
            consume(TokenType.ARROW, "Expect '->'.");
            Token keyword = consume(TokenType.IDENTIFIER, "Expect keyword like 'listen' or 'delegate'.");
            if (keyword.value().equals("listen")) {
                return parseListenBlock(selector);
            } else if (keyword.value().equals("delegate")) {
                return parseDelegateBlock(selector);
            }
        } else if (check(TokenType.IDENTIFIER)) {
            // This is a statement starting with a keyword, e.g., animate { ... }
            Token keyword = advance();
            if (keyword.value().equals("animate")) {
                return parseAnimateBlock();
            } else if (keyword.value().equals("router")) {
                return parseRouterBlock();
            }
        }
        throw new RuntimeException("Unexpected token in CHTL JS: " + peek().value());
    }

    private String parseSelector() {
        consume(TokenType.L_CURLY_BRACE, "Expect '{{'.");
        String selector = consume(TokenType.SELECTOR, "Expect selector.").value();
        consume(TokenType.R_CURLY_BRACE, "Expect '}}'.");
        return selector;
    }

    private ListenNode parseListenBlock(String selector) {
        consume(TokenType.LBRACE, "Expect '{' for listen block.");

        ListenNode listenNode = new ListenNode(selector);

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
        int braceDepth = 0;
        while (!isAtEnd()) {
            if (braceDepth == 0) {
                for (TokenType terminator : terminators) {
                    if (check(terminator)) {
                        return builder.toString().trim();
                    }
                }
            }
            if (check(TokenType.LBRACE)) braceDepth++;
            if (check(TokenType.RBRACE)) braceDepth--;

            builder.append(peek().value()).append(" ");
            advance();
        }
        return builder.toString().trim();
    }

    private DelegateNode parseDelegateBlock(String selector) {
        consume(TokenType.LBRACE, "Expect '{' for delegate block.");
        DelegateNode node = new DelegateNode(selector);
        // Simplified parsing
        consume(TokenType.RBRACE, "Expect '}'.");
        return node;
    }

    private AnimateNode parseAnimateBlock() {
        consume(TokenType.LBRACE, "Expect '{' for animate block.");
        AnimateNode node = new AnimateNode();
        // Simplified parsing
        consume(TokenType.RBRACE, "Expect '}'.");
        return node;
    }

    private RouterNode parseRouterBlock() {
        consume(TokenType.LBRACE, "Expect '{' for router block.");
        RouterNode node = new RouterNode();
        // Simplified parsing
        consume(TokenType.RBRACE, "Expect '}'.");
        return node;
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

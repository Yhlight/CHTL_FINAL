package CHTL_JS.Parser;

import CHTL_JS.Lexer.Token;
import CHTL_JS.Lexer.TokenType;
import CHTL_JS.Node.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;

public class CHTLJSParser {
    private final List<Token> tokens;
    private int current = 0;

    public CHTLJSParser(List<Token> tokens) { this.tokens = tokens; }

    public List<CHTLJSBaseNode> parse() {
        List<CHTLJSBaseNode> nodes = new ArrayList<>();
        while (!isAtEnd()) {
            nodes.add(statement());
        }
        return nodes;
    }

    private CHTLJSBaseNode statement() {
        if (check(TokenType.L_CURLY_BRACE)) {
            String selector = parseSelector();
            consume(TokenType.ARROW, "Expect '->'.");
            Token keyword = consume(TokenType.IDENTIFIER, "Expect keyword.");
            if (keyword.value().equals("listen")) return parseListenBlock(selector);
            if (keyword.value().equals("delegate")) return parseDelegateBlock(selector);
        } else if (check(TokenType.IDENTIFIER)) {
            Token keyword = advance();
            if (keyword.value().equals("animate")) return parseAnimateBlock();
            if (keyword.value().equals("router")) return parseRouterBlock();
        }
        throw new RuntimeException("Unexpected token in CHTL JS: " + peek().value());
    }

    private String parseSelector() {
        consume(TokenType.L_CURLY_BRACE, "Expect '{{'.");
        String selector = consume(TokenType.SELECTOR, "Expect selector.").value();
        consume(TokenType.R_CURLY_BRACE, "Expect '}}'.");
        return selector;
    }

    private Map<String, String> parseKeyValueBlock() {
        Map<String, String> map = new HashMap<>();
        consume(TokenType.LBRACE, "Expect '{'.");
        while (!check(TokenType.RBRACE)) {
            Token key = consume(TokenType.IDENTIFIER, "Expect property key.");
            consume(TokenType.COLON, "Expect ':'.");
            String value = consumeUntil(TokenType.COMMA, TokenType.RBRACE);
            map.put(key.value(), value);
            if (check(TokenType.COMMA)) advance();
        }
        consume(TokenType.RBRACE, "Expect '}'.");
        return map;
    }

    private ListenNode parseListenBlock(String selector) {
        ListenNode node = new ListenNode(selector);
        Map<String, String> handlers = parseKeyValueBlock();
        handlers.forEach(node::addEventHandler);
        return node;
    }

    private DelegateNode parseDelegateBlock(String selector) {
        DelegateNode node = new DelegateNode(selector);
        Map<String, String> events = parseKeyValueBlock();
        events.forEach(node::addDelegatedEvent);
        return node;
    }

    private AnimateNode parseAnimateBlock() {
        AnimateNode node = new AnimateNode();
        Map<String, String> properties = parseKeyValueBlock();
        properties.forEach(node::addProperty);
        return node;
    }

    private RouterNode parseRouterBlock() {
        RouterNode node = new RouterNode();
        Map<String, String> routes = parseKeyValueBlock();
        routes.forEach(node::addRoute);
        return node;
    }

    private String consumeUntil(TokenType... terminators) {
        StringBuilder builder = new StringBuilder();
        int braceDepth = 0;
        while (!isAtEnd()) {
            if (braceDepth == 0) {
                for (TokenType terminator : terminators) {
                    if (check(terminator)) return builder.toString().trim();
                }
            }
            if (check(TokenType.LBRACE)) braceDepth++;
            if (check(TokenType.RBRACE)) braceDepth--;
            builder.append(peek().value()).append(" ");
            advance();
        }
        return builder.toString().trim();
    }

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

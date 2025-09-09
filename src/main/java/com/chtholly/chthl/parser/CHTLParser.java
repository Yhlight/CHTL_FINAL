package com.chtholly.chthl.parser;

import com.chtholly.chthl.ast.*;
import com.chtholly.chthl.ast.expr.Expression;
import com.chtholly.chthl.ast.template.*;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class CHTLParser {

    private static class ParseError extends RuntimeException {
        ParseError(Token token, String message) {
            super("[line " + token.getLine() + "] Error at '" + token.getLexeme() + "': " + message);
        }
    }

    private final List<Token> tokens;
    private int current = 0;
    private final Map<String, TemplateNode> templateTable = new HashMap<>();

    public CHTLParser(List<Token> tokens) {
        this.tokens = tokens;
    }

    public List<Node> getAst() {
        List<Node> nodes = new ArrayList<>();
        while (!isAtEnd()) {
            Node node = declaration();
            if (node != null) {
                nodes.add(node);
            }
        }
        return nodes;
    }

    public Map<String, TemplateNode> getTemplateTable() {
        return templateTable;
    }

    private Node declaration() {
        try {
            if (check(TokenType.LEFT_BRACKET) && peekNext().getType() == TokenType.TEMPLATE) {
                parseTemplateDefinition();
                return null;
            }
            if (check(TokenType.IDENTIFIER) && peek().getLexeme().startsWith("@")) {
                return parseTemplateUsage();
            }
            if (peek().getType() == TokenType.TEXT) return textDeclaration();
            if (peek().getType() == TokenType.IDENTIFIER) return elementDeclaration();

            throw new ParseError(peek(), "Expected a declaration or definition.");
        } catch (ParseError error) {
            System.err.println(error.getMessage());
            synchronize();
            return null;
        }
    }

    private TemplateUsageNode parseTemplateUsage() {
        Token type = consume(TokenType.IDENTIFIER, "Expect template type like '@Style'.");
        Token name = consume(TokenType.IDENTIFIER, "Expect template name.");
        consume(TokenType.SEMICOLON, "Expect ';' after template usage.");
        return new TemplateUsageNode(type, name);
    }

    private void parseTemplateDefinition() {
        consume(TokenType.LEFT_BRACKET, "Expect '['.");
        consume(TokenType.TEMPLATE, "Expect 'Template' keyword.");
        consume(TokenType.RIGHT_BRACKET, "Expect ']'.");
        Token type = consume(TokenType.IDENTIFIER, "Expect template type like '@Style'.");
        Token name = consume(TokenType.IDENTIFIER, "Expect template name.");

        switch (type.getLexeme()) {
            case "@Style":
                templateTable.put(name.getLexeme(), parseStyleTemplate(name));
                break;
            case "@Element":
                templateTable.put(name.getLexeme(), parseElementTemplate(name));
                break;
            case "@Var":
                templateTable.put(name.getLexeme(), parseVarTemplate(name));
                break;
            default:
                throw new ParseError(type, "Unknown template type.");
        }
    }

    private StyleTemplateNode parseStyleTemplate(Token name) {
        consume(TokenType.LEFT_BRACE, "Expect '{' to start style template body.");
        List<Node> body = new ArrayList<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType.IDENTIFIER) && peek().getLexeme().startsWith("@")) {
                body.add(parseTemplateUsage());
            } else {
                body.add(parseStyleProperty());
            }
        }
        consume(TokenType.RIGHT_BRACE, "Expect '}' to end style template body.");
        return new StyleTemplateNode(name, body);
    }

    private ElementTemplateNode parseElementTemplate(Token name) {
        consume(TokenType.LEFT_BRACE, "Expect '{' to start element template body.");
        List<Node> body = new ArrayList<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            Node node = declaration();
            if (node != null) {
                body.add(node);
            }
        }
        consume(TokenType.RIGHT_BRACE, "Expect '}' to end element template body.");
        return new ElementTemplateNode(name, body);
    }

    private VarTemplateNode parseVarTemplate(Token name) {
        consume(TokenType.LEFT_BRACE, "Expect '{' to start var template body.");
        Map<String, Expression> variables = new HashMap<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            Token varName = consume(TokenType.IDENTIFIER, "Expect variable name.");
            consume(TokenType.COLON, "Expect ':' after variable name.");
            List<Token> valueTokens = new ArrayList<>();
            while (!check(TokenType.SEMICOLON) && !isAtEnd()) {
                valueTokens.add(advance());
            }
            consume(TokenType.SEMICOLON, "Expect ';' after variable value.");
            variables.put(varName.getLexeme(), new ExpressionParser(valueTokens).parse());
        }
        consume(TokenType.RIGHT_BRACE, "Expect '}' to end var template body.");
        return new VarTemplateNode(name, variables);
    }


    private Node elementDeclaration() {
        Token name = consume(TokenType.IDENTIFIER, "Expect element name.");
        consume(TokenType.LEFT_BRACE, "Expect '{' after element name.");
        List<Node> children = new ArrayList<>();
        Map<String, String> attributes = new HashMap<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType.IDENTIFIER) && peekNext().getType() == TokenType.COLON) {
                attribute(attributes);
            } else if (check(TokenType.STYLE)) {
                children.add(styleBlock());
            } else {
                Node child = declaration();
                if (child != null) children.add(child);
            }
        }
        consume(TokenType.RIGHT_BRACE, "Expect '}' after element block.");
        return new ElementNode(name.getLexeme(), attributes, children);
    }

    private void attribute(Map<String, String> attributes) {
        Token name = consume(TokenType.IDENTIFIER, "Expect attribute name.");
        consume(TokenType.COLON, "Expect ':' after attribute name.");
        Token valueToken;
        if (match(TokenType.STRING)) valueToken = previous();
        else if (match(TokenType.IDENTIFIER)) valueToken = previous();
        else throw new ParseError(peek(), "Expect attribute value.");
        String value = (valueToken.getLiteral() != null) ? valueToken.getLiteral().toString() : valueToken.getLexeme();
        attributes.put(name.getLexeme(), value);
        consume(TokenType.SEMICOLON, "Expect ';' after attribute value.");
    }

    private Node styleBlock() {
        consume(TokenType.STYLE, "Expect 'style' keyword.");
        consume(TokenType.LEFT_BRACE, "Expect '{' after 'style'.");
        List<Node> directPropertiesAndUsages = new ArrayList<>();
        List<SelectorBlockNode> selectorBlocks = new ArrayList<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType.IDENTIFIER) && peek().getLexeme().startsWith("@")) {
                directPropertiesAndUsages.add(parseTemplateUsage());
            } else if (check(TokenType.IDENTIFIER) && peekNext().getType() == TokenType.COLON) {
                directPropertiesAndUsages.add(parseStyleProperty());
            } else {
                selectorBlocks.add(parseSelectorBlock());
            }
        }
        consume(TokenType.RIGHT_BRACE, "Expect '}' after style block.");
        return new StyleBlockNode(directPropertiesAndUsages, selectorBlocks);
    }

    private StylePropertyNode parseStyleProperty() {
        Token name = consume(TokenType.IDENTIFIER, "Expect style property name.");
        consume(TokenType.COLON, "Expect ':' after style property name.");
        List<Token> valueTokens = new ArrayList<>();
        while (!check(TokenType.SEMICOLON) && !isAtEnd()) {
            valueTokens.add(advance());
        }
        if (valueTokens.isEmpty()) throw new ParseError(peek(), "Style property value cannot be empty.");
        consume(TokenType.SEMICOLON, "Expect ';' after style property value.");
        return new StylePropertyNode(name.getLexeme(), new ExpressionParser(valueTokens).parse());
    }

    private SelectorBlockNode parseSelectorBlock() {
        StringBuilder selectorBuilder = new StringBuilder();
        while (!check(TokenType.LEFT_BRACE) && !isAtEnd()) {
            selectorBuilder.append(advance().getLexeme());
        }
        String selector = selectorBuilder.toString().trim();
        if (selector.isEmpty()) throw new ParseError(peek(), "Selector cannot be empty.");
        consume(TokenType.LEFT_BRACE, "Expect '{' after selector.");
        List<Node> body = new ArrayList<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType.IDENTIFIER) && peek().getLexeme().startsWith("@")) {
                body.add(parseTemplateUsage());
            } else {
                body.add(parseStyleProperty());
            }
        }
        consume(TokenType.RIGHT_BRACE, "Expect '}' after selector block.");
        return new SelectorBlockNode(selector, body);
    }

    private Node textDeclaration() {
        consume(TokenType.TEXT, "Expect 'text' keyword.");
        consume(TokenType.LEFT_BRACE, "Expect '{' after 'text' keyword.");
        Token content = consume(TokenType.STRING, "Expect a string literal inside a text block.");
        consume(TokenType.RIGHT_BRACE, "Expect '}' after text block.");
        return new TextNode((String) content.getLiteral());
    }

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
        throw new ParseError(peek(), message);
    }

    private boolean check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().getType() == type;
    }

    private Token peekNext() {
        if (current + 1 >= tokens.size()) return tokens.get(tokens.size() - 1);
        return tokens.get(current + 1);
    }

    private Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    private boolean isAtEnd() {
        return peek().getType() == TokenType.EOF;
    }

    private Token peek() {
        return tokens.get(current);
    }

    private Token previous() {
        return tokens.get(current - 1);
    }

    private void synchronize() {
        advance();
        while (!isAtEnd()) {
            if (previous().getType() == TokenType.RIGHT_BRACE) return;
            switch (peek().getType()) {
                case IDENTIFIER:
                case TEXT:
                case STYLE:
                case LEFT_BRACKET:
                    return;
            }
            advance();
        }
    }
}

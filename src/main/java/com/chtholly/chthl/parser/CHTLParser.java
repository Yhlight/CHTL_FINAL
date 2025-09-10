package com.chtholly.chthl.parser;

import com.chtholly.chthl.ast.*;
import com.chtholly.chthl.ast.custom.CustomizationBlockNode;
import com.chtholly.chthl.ast.custom.DeleteNode;
import com.chtholly.chthl.ast.custom.InsertNode;
import com.chtholly.chthl.ast.custom.ModificationNode;
import com.chtholly.chthl.ast.custom.SetNode;
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
    private final Map<String, OriginNode> originTable = new HashMap<>();

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

    public Map<String, OriginNode> getOriginTable() {
        return originTable;
    }

    private Node declaration() {
        try {
            if (check(TokenType.LEFT_BRACKET) && peekNext().getType() == TokenType.TEMPLATE) {
                parseTemplateDefinition();
                return null;
            }
            if (check(TokenType.LEFT_BRACKET) && peekNext().getType() == TokenType.ORIGIN) {
                return parseOriginBlock();
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

    private Node parseOriginBlock() {
        consume(TokenType.LEFT_BRACKET, "Expect '['.");
        consume(TokenType.ORIGIN, "Expect 'Origin' keyword.");
        consume(TokenType.RIGHT_BRACKET, "Expect ']'.");

        Token type = consume(TokenType.IDENTIFIER, "Expect origin type like '@Html'.");
        Token name = null;
        if (check(TokenType.IDENTIFIER)) {
            name = advance();
        }

        if (match(TokenType.LEFT_BRACE)) {
            // It's a definition
            Token contentToken = consume(TokenType.STRING, "Expect a string literal inside an origin block.");
            String content = (String) contentToken.getLiteral();
            consume(TokenType.RIGHT_BRACE, "Expect '}' after origin block.");

            OriginNode node = new OriginNode(type, name, content, false);
            if (name != null) {
                originTable.put(name.getLexeme(), node);
                return null; // Definitions are not part of the render tree
            }
            return node; // Anonymous origin block
        } else {
            // It's a usage
            consume(TokenType.SEMICOLON, "Expect ';' after named origin usage.");
            if (name == null) {
                throw new ParseError(previous(), "Expect a name for origin usage.");
            }
            return new OriginNode(type, name, null, true);
        }
    }

    private TemplateUsageNode parseTemplateUsage() {
        Token type = consume(TokenType.IDENTIFIER, "Expect template type like '@Style'.");
        Token name = consume(TokenType.IDENTIFIER, "Expect template name.");

        CustomizationBlockNode customization = null;
        if (check(TokenType.LEFT_BRACE)) {
            customization = parseCustomizationBlock();
        } else {
            consume(TokenType.SEMICOLON, "Expect ';' after template usage without a customization block.");
        }

        return new TemplateUsageNode(type, name, customization);
    }

    private CustomizationBlockNode parseCustomizationBlock() {
        consume(TokenType.LEFT_BRACE, "Expect '{' to start customization block.");
        List<Node> modifications = new ArrayList<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            if (match(TokenType.DELETE)) {
                modifications.add(parseDelete());
            } else if (match(TokenType.INSERT)) {
                modifications.add(parseInsert());
            } else if (match(TokenType.SET)) {
                modifications.add(parseSet());
            } else if (check(TokenType.IDENTIFIER) && (peekNext().getType() == TokenType.COLON || peekNext().getType() == TokenType.EQUAL)) {
                // This handles property definitions inside a customization block
                modifications.add(parseStyleProperty());
            } else {
                throw new ParseError(peek(), "Expected a modification keyword or property definition.");
            }
        }
        consume(TokenType.RIGHT_BRACE, "Expect '}' to end customization block.");
        return new CustomizationBlockNode(modifications);
    }

    private DeleteNode parseDelete() {
        List<List<Token>> allTargets = new ArrayList<>();
        do {
            List<Token> currentTarget = new ArrayList<>();
            while (!check(TokenType.COMMA) && !check(TokenType.SEMICOLON) && !isAtEnd()) {
                currentTarget.add(advance());
            }
            if(currentTarget.isEmpty()) throw new ParseError(peek(), "Expect a target for 'delete'.");
            allTargets.add(currentTarget);
        } while (match(TokenType.COMMA));

        consume(TokenType.SEMICOLON, "Expect ';' after delete statement.");
        return new DeleteNode(allTargets);
    }

    private InsertNode parseInsert() {
        Token position;
        List<Token> target = new ArrayList<>();

        if (match(TokenType.AFTER, TokenType.BEFORE, TokenType.REPLACE, TokenType.INTO)) {
            position = previous();
        } else {
            throw new ParseError(peek(), "Expect a position keyword like 'after', 'before', 'replace', or 'into' after 'insert'.");
        }

        while (!check(TokenType.LEFT_BRACE) && !isAtEnd()) {
            target.add(advance());
        }
        if (target.isEmpty()) {
            throw new ParseError(peek(), "Expect a target selector for '" + position.getLexeme() + "'.");
        }

        consume(TokenType.LEFT_BRACE, "Expect '{' for insert body.");

        List<Node> body = new ArrayList<>();

        boolean isInsertIntoStyle = position.getType() == TokenType.INTO &&
                                    target.size() == 1 &&
                                    target.get(0).getType() == TokenType.STYLE;

        if (isInsertIntoStyle) {
            while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
                body.add(parseStyleProperty());
            }
        } else {
            while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
                Node node = declaration();
                if (node != null) {
                    body.add(node);
                }
            }
        }

        consume(TokenType.RIGHT_BRACE, "Expect '}' to end insert body.");

        return new InsertNode(position, target, body);
    }

    private SetNode parseSet() {
        Token name = consume(TokenType.IDENTIFIER, "Expect variable name after 'set'.");
        consume(TokenType.EQUAL, "Expect '=' after variable name.");

        List<Token> valueTokens = new ArrayList<>();
        while (!check(TokenType.SEMICOLON) && !isAtEnd()) {
            valueTokens.add(advance());
        }

        if (valueTokens.isEmpty()) {
            throw new ParseError(peek(), "Variable value cannot be empty.");
        }

        consume(TokenType.SEMICOLON, "Expect ';' after variable value.");

        Expression value = new ExpressionParser(valueTokens).parse();
        return new SetNode(name, value);
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
                case DELETE:
                case INSERT:
                case SET:
                    return;
            }
            advance();
        }
    }
}

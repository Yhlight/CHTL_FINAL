package com.chtholly.chthl.parser;

import com.chtholly.chthl.ast.*;
import com.chtholly.chthl.ast.custom.CustomizationBlockNode;
import com.chtholly.chthl.ast.custom.DeleteNode;
import com.chtholly.chthl.ast.custom.InsertNode;
import com.chtholly.chthl.ast.custom.ModificationNode;
import com.chtholly.chthl.ast.custom.SetNode;
import com.chtholly.chthl.ast.expr.Expression;
import com.chtholly.chthl.ast.template.*;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;
import com.chtholly.chthl.loader.CHTLLoader;

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

    private static final String GLOBAL_NAMESPACE = "__global__";

    private final List<Token> tokens;
    private final String filePath;
    private int current = 0;
    private String currentNamespace = GLOBAL_NAMESPACE;

    private final Map<String, Map<String, TemplateNode>> templateTable = new HashMap<>();
    private final Map<String, Map<String, OriginNode>> originTable = new HashMap<>();
    private ConfigurationNode configuration = null;

    public CHTLParser(List<Token> tokens, String filePath) {
        this.tokens = tokens;
        this.filePath = filePath;
        this.templateTable.put(GLOBAL_NAMESPACE, new HashMap<>());
        this.originTable.put(GLOBAL_NAMESPACE, new HashMap<>());
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

    public Map<String, Map<String, TemplateNode>> getTemplateTable() {
        return templateTable;
    }

    public Map<String, Map<String, OriginNode>> getOriginTable() {
        return originTable;
    }

    public ConfigurationNode getConfiguration() {
        return configuration;
    }

    private Node declaration() {
        try {
            if (check(TokenType.LEFT_BRACKET) && peekNext().getType() == TokenType.CONFIGURATION) {
                parseConfigurationBlock();
                return null;
            }
            if (check(TokenType.LEFT_BRACKET) && peekNext().getType() == TokenType.TEMPLATE) {
                parseTemplateDefinition();
                return null;
            }
            if (check(TokenType.LEFT_BRACKET) && peekNext().getType() == TokenType.ORIGIN) {
                return parseOriginBlock();
            }
            if (check(TokenType.LEFT_BRACKET) && peekNext().getType() == TokenType.IMPORT) {
                parseImportStatement();
                return null;
            }
            if (check(TokenType.LEFT_BRACKET) && peekNext().getType() == TokenType.NAMESPACE) {
                return parseNamespaceNode();
            }
            if (check(TokenType.AT_SIGN)) {
                return parseTemplateUsage();
            }
            if (peek().getType() == TokenType.TEXT) return textDeclaration();
            if (peek().getType() == TokenType.IDENTIFIER) return elementDeclaration();

            throw new ParseError(peek(), "Expected a declaration or definition.");
        } catch (ParseError error) {
            System.err.println("CAUGHT PARSE ERROR: " + error.getMessage());
            System.err.println(error.getMessage());
            synchronize();
            return null;
        }
    }

    private void parseConfigurationBlock() {
        consume(TokenType.LEFT_BRACKET, "Expect '['.");
        consume(TokenType.CONFIGURATION, "Expect 'Configuration' keyword.");
        consume(TokenType.RIGHT_BRACKET, "Expect ']'.");

        // Optional name for named config blocks, not used for now
        if (check(TokenType.IDENTIFIER)) {
            advance();
        }

        consume(TokenType.LEFT_BRACE, "Expect '{' to start configuration block.");
        Map<String, Object> settings = new HashMap<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            Token key = consume(TokenType.IDENTIFIER, "Expect setting key.");
            consume(TokenType.EQUAL, "Expect '=' after setting key.");

            Token valueToken;
            if (match(TokenType.IDENTIFIER)) { // e.g., false, 0
                valueToken = previous();
                 try {
                    settings.put(key.getLexeme(), Integer.parseInt(valueToken.getLexeme()));
                } catch (NumberFormatException e) {
                    settings.put(key.getLexeme(), Boolean.parseBoolean(valueToken.getLexeme()));
                }
            } else if (match(TokenType.STRING)) {
                valueToken = previous();
                settings.put(key.getLexeme(), valueToken.getLiteral());
            } else {
                throw new ParseError(peek(), "Expect a literal value for setting.");
            }

            consume(TokenType.SEMICOLON, "Expect ';' after setting value.");
        }
        consume(TokenType.RIGHT_BRACE, "Expect '}' to end configuration block.");

        if (this.configuration == null) {
            this.configuration = new ConfigurationNode(settings);
        } else {
            // Handle merging or error on re-definition if necessary
            this.configuration.settings.putAll(settings);
        }
    }

    private void parseImportStatement() {
        consume(TokenType.LEFT_BRACKET, "Expect '['.");
        consume(TokenType.IMPORT, "Expect 'Import' keyword.");
        consume(TokenType.RIGHT_BRACKET, "Expect ']'.");

        consume(TokenType.AT_SIGN, "Expect '@' before import type.");
        Token type = consume(TokenType.IDENTIFIER, "Expect import type like 'Chtl'.");

        if (!type.getLexeme().equals("Chtl")) {
            throw new ParseError(type, "Unsupported import type. Only '@Chtl' is supported for now.");
        }

        consume(TokenType.FROM, "Expect 'from' keyword.");

        Token pathToken;
        if (match(TokenType.STRING)) {
            pathToken = previous();
        } else {
            throw new ParseError(peek(), "Expect a string literal for the import path.");
        }
        String importPath = (String) pathToken.getLiteral();

        consume(TokenType.SEMICOLON, "Expect ';' after import statement.");

        try {
            CHTLLoader loader = new CHTLLoader();
            String content = loader.load(importPath, this.filePath);

            CHTLLexer lexer = new CHTLLexer(content);
            List<Token> importedTokens = lexer.scanTokens();

            // Create a new parser for the imported file. Pass the imported file's path for nested imports.
            CHTLParser importedParser = new CHTLParser(importedTokens, importPath);
            importedParser.getAst(); // This parses the file and populates the tables.

            // Merge the template tables.
            boolean hasExplicitNamespace = importedParser.getTemplateTable().keySet().stream().anyMatch(k -> !k.equals(GLOBAL_NAMESPACE));
            if (hasExplicitNamespace) {
                importedParser.getTemplateTable().forEach((namespace, table) -> {
                    this.templateTable.computeIfAbsent(namespace, k -> new HashMap<>()).putAll(table);
                });
            } else {
                String defaultNamespace = deriveNamespaceFromPath(importPath);
                Map<String, TemplateNode> globalTemplates = importedParser.getTemplateTable().get(GLOBAL_NAMESPACE);
                if (globalTemplates != null && !globalTemplates.isEmpty()) {
                    this.templateTable.computeIfAbsent(defaultNamespace, k -> new HashMap<>()).putAll(globalTemplates);
                }
            }

            // Merge the origin tables.
            hasExplicitNamespace = importedParser.getOriginTable().keySet().stream().anyMatch(k -> !k.equals(GLOBAL_NAMESPACE));
            if (hasExplicitNamespace) {
                importedParser.getOriginTable().forEach((namespace, table) -> {
                    this.originTable.computeIfAbsent(namespace, k -> new HashMap<>()).putAll(table);
                });
            } else {
                String defaultNamespace = deriveNamespaceFromPath(importPath);
                 Map<String, OriginNode> globalOrigins = importedParser.getOriginTable().get(GLOBAL_NAMESPACE);
                if (globalOrigins != null && !globalOrigins.isEmpty()) {
                    this.originTable.computeIfAbsent(defaultNamespace, k -> new HashMap<>()).putAll(globalOrigins);
                }
            }

        } catch (CHTLLoader.LoadError e) {
            throw new ParseError(pathToken, "Failed to import file: " + e.getMessage());
        }
    }

    private NamespaceNode parseNamespaceNode() {
        consume(TokenType.LEFT_BRACKET, "Expect '['.");
        consume(TokenType.NAMESPACE, "Expect 'Namespace' keyword.");
        consume(TokenType.RIGHT_BRACKET, "Expect ']'.");

        Token name = consume(TokenType.IDENTIFIER, "Expect namespace name.");
        String namespaceName = name.getLexeme();

        // Ensure the maps for this namespace exist
        templateTable.putIfAbsent(namespaceName, new HashMap<>());
        originTable.putIfAbsent(namespaceName, new HashMap<>());

        List<Node> children = new ArrayList<>();
        if (match(TokenType.LEFT_BRACE)) {
            String previousNamespace = this.currentNamespace;
            this.currentNamespace = namespaceName;

            while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
                Node node = declaration();
                if (node != null) {
                    children.add(node);
                }
            }
            consume(TokenType.RIGHT_BRACE, "Expect '}' after namespace block.");

            this.currentNamespace = previousNamespace; // Restore previous namespace
        } else {
            // For single-line namespace declarations, it applies to subsequent definitions.
            this.currentNamespace = namespaceName;
        }

        return new NamespaceNode(name.getLexeme(), children);
    }

    private Node parseOriginBlock() {
        consume(TokenType.LEFT_BRACKET, "Expect '['.");
        consume(TokenType.ORIGIN, "Expect 'Origin' keyword.");
        consume(TokenType.RIGHT_BRACKET, "Expect ']'.");

        Token at = consume(TokenType.AT_SIGN, "Expect '@' for origin type.");
        Token typeIdentifier = consume(TokenType.IDENTIFIER, "Expect origin type like 'Html'.");

        // Manually create a token for the full type, e.g., "@Html"
        Token fullType = new Token(
            TokenType.IDENTIFIER,
            "@" + typeIdentifier.getLexeme(),
            null,
            at.getLine()
        );

        Token name = null;
        if (check(TokenType.IDENTIFIER)) {
            name = advance();
        }

        if (match(TokenType.LEFT_BRACE)) {
            // It's a definition
            Token contentToken = consume(TokenType.STRING, "Expect a string literal inside an origin block.");
            String content = (String) contentToken.getLiteral();
            consume(TokenType.RIGHT_BRACE, "Expect '}' after origin block.");

            OriginNode node = new OriginNode(fullType, name, content, false, null); // No 'from' in definition
            if (name != null) {
                originTable.get(currentNamespace).put(name.getLexeme(), node);
                return null; // Definitions are not part of the render tree
            }
            return node; // Anonymous origin block
        } else {
            // It's a usage
            Token fromNamespace = null;
            if (match(TokenType.FROM)) {
                fromNamespace = consume(TokenType.IDENTIFIER, "Expect namespace name after 'from'.");
            }
            consume(TokenType.SEMICOLON, "Expect ';' after named origin usage.");
            if (name == null) {
                throw new ParseError(previous(), "Expect a name for origin usage.");
            }
            return new OriginNode(fullType, name, null, true, fromNamespace);
        }
    }

    private TemplateUsageNode parseTemplateUsage() {
        Token at = consume(TokenType.AT_SIGN, "Expect '@' to start a template usage.");
        Token typeIdentifier = consume(TokenType.IDENTIFIER, "Expect template type like 'Style'.");

        // Manually create a token for the full type, e.g., "@Style"
        Token fullType = new Token(
            TokenType.IDENTIFIER,
            "@" + typeIdentifier.getLexeme(),
            null,
            at.getLine()
        );

        Token name = consume(TokenType.IDENTIFIER, "Expect template name.");

        Token fromNamespace = null;
        if (match(TokenType.FROM)) {
            fromNamespace = consume(TokenType.IDENTIFIER, "Expect namespace name after 'from'.");
        }

        CustomizationBlockNode customization = null;
        if (check(TokenType.LEFT_BRACE)) {
            customization = parseCustomizationBlock();
        } else {
            consume(TokenType.SEMICOLON, "Expect ';' after template usage.");
        }

        return new TemplateUsageNode(fullType, name, fromNamespace, customization);
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
            } else {
                throw new ParseError(peek(), "Expected a modification keyword like 'delete', 'insert', or 'set'.");
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

        consume(TokenType.AT_SIGN, "Expect '@' for template type.");
        Token type = consume(TokenType.IDENTIFIER, "Expect template type like 'Style'.");
        Token name = consume(TokenType.IDENTIFIER, "Expect template name.");

        switch (type.getLexeme()) {
            case "Style":
                templateTable.get(currentNamespace).put(name.getLexeme(), parseStyleTemplate(name));
                break;
            case "Element":
                templateTable.get(currentNamespace).put(name.getLexeme(), parseElementTemplate(name));
                break;
            case "Var":
                templateTable.get(currentNamespace).put(name.getLexeme(), parseVarTemplate(name));
                break;
            default:
                throw new ParseError(type, "Unknown template type.");
        }
    }

    private StyleTemplateNode parseStyleTemplate(Token name) {
        consume(TokenType.LEFT_BRACE, "Expect '{' to start style template body.");
        List<Node> body = new ArrayList<>();
        while (!check(TokenType.RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType.AT_SIGN)) {
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
            if (check(TokenType.AT_SIGN)) {
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
            if (check(TokenType.AT_SIGN)) {
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
                case MINUS_MINUS:
                case CONFIGURATION:
                    return;
            }
            advance();
        }
    }

    private String deriveNamespaceFromPath(String path) {
        // Simple implementation: get filename without extension.
        // e.g., "path/to/my-theme.chtl" -> "my-theme"
        String filename = new java.io.File(path).getName();
        int lastDot = filename.lastIndexOf('.');
        if (lastDot > 0) {
            return filename.substring(0, lastDot);
        }
        return filename;
    }
}

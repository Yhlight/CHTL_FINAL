package CHTL.CHTLParser;

import CHTL.CHTLLexer.CHTLLexer;
import CHTL.CHTLLexer.Token;
import CHTL.CHTLLexer.TokenType;
import CHTL.CHTLLoader.CHTLLoader;
import CHTL.CHTLManage.DefinitionManager;
import CHTL.CHTLNode.*;

import java.io.IOException;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class CHTLParser {
    private final String source;
    private final List<Token> tokens;
    private final DefinitionManager definitionManager;
    private final String namespace;
    private final CHTLLoader loader;
    private int current = 0;

    private static class ParseError extends RuntimeException {
        ParseError(Token token, String message) {
            super("Parse Error at line " + token.getLine() + " col " + token.getColumn() + ": " + message);
        }
    }

    public CHTLParser(String source, List<Token> tokens, DefinitionManager definitionManager, String namespace, CHTLLoader loader) {
        this.source = source;
        this.tokens = tokens;
        this.definitionManager = definitionManager;
        this.namespace = namespace;
        this.loader = loader;
    }

    public List<BaseNode> parse() {
        List<BaseNode> nodes = new ArrayList<>();
        if (check(TokenType.USE)) {
            nodes.add(parseUseStatement());
        }
        while (!isAtEnd()) {
            nodes.add(declaration());
        }
        return nodes;
    }

    public List<ConfigurationNode> preScanForConfiguration() {
        List<ConfigurationNode> configs = new ArrayList<>();
        while (!isAtEnd()) {
            if (check(TokenType.LBRACKET) && tokenAt(current + 1) != null && tokenAt(current + 1).getType() == TokenType.CONFIGURATION) {
                configs.add(parseConfigurationBlock());
            } else {
                if (check(TokenType.LBRACE)) {
                    skipBlock();
                } else {
                    advance();
                }
            }
        }
        return configs;
    }

    private void skipBlock() {
        consume(TokenType.LBRACE, "Expect '{'.");
        int braceDepth = 1;
        while(braceDepth > 0 && !isAtEnd()) {
            if (check(TokenType.LBRACE)) {
                braceDepth++;
                advance();
            } else if (check(TokenType.RBRACE)) {
                braceDepth--;
                advance();
            } else {
                advance();
            }
        }
    }

    private String sourceText(int startOffset, int endOffset) {
        if (startOffset < 0 || endOffset > source.length() || startOffset > endOffset) return "";
        return source.substring(startOffset, endOffset);
    }

    private BaseNode declaration() {
        if (check(TokenType.LBRACKET)) {
            Token next = tokenAt(current + 1);
            if (next == null) throw new ParseError(peek(), "Unexpected end of file after '['.");
            switch(next.getType()) {
                case IMPORT: return parseImportDeclaration();
                case TEMPLATE: case CUSTOM: return parseBracketedDefinition();
                case ORIGIN: return parseOriginBlock();
                case CONFIGURATION: return parseConfigurationBlock();
                default: break;
            }
        }
        if (check(TokenType.AT)) return parseTemplateUsage();
        if (check(TokenType.IDENTIFIER)) return element(advance().getValue());
        if (check(TokenType.TEXT)) { advance(); return textBlock(); }
        if (check(TokenType.STYLE)) return parseStyleBlock();
        if (match(TokenType.GENERATOR_COMMENT)) return new CommentNode(previous().getValue());
        throw new ParseError(peek(), "Expect a declaration.");
    }

    private UseNode parseUseStatement() {
        consume(TokenType.USE, "Expect 'use' keyword.");
        Token target = consume(TokenType.IDENTIFIER, "Expect target after 'use'.");
        consume(TokenType.SEMICOLON, "Expect ';' after use statement.");
        return new UseNode(target.getValue());
    }

    private OriginNode parseOriginBlock() {
        consume(TokenType.LBRACKET, "Expect '['."); consume(TokenType.ORIGIN, "Expect 'Origin' keyword."); consume(TokenType.RBRACKET, "Expect ']'.");
        consume(TokenType.AT, "Expect '@' before origin type.");
        String type = consume(TokenType.IDENTIFIER, "Expect origin type.").getValue();
        consume(TokenType.LBRACE, "Expect '{' to start origin block.");
        Token startBrace = previous();
        int contentStartIndex = startBrace.getEndOffset();
        int braceDepth = 1;
        while (braceDepth > 0 && !isAtEnd()) {
            if (check(TokenType.LBRACE)) braceDepth++;
            else if (check(TokenType.RBRACE)) braceDepth--;
            if (braceDepth > 0) advance();
        }
        Token endBrace = peek();
        int contentEndIndex = endBrace.getStartOffset();
        String content = sourceText(contentStartIndex, contentEndIndex);
        consume(TokenType.RBRACE, "Expect '}' to end origin block.");
        return new OriginNode(type, content);
    }

    private ConfigurationNode parseConfigurationBlock() {
        consume(TokenType.LBRACKET, "Expect '['."); consume(TokenType.CONFIGURATION, "Expect 'Configuration' keyword."); consume(TokenType.RBRACKET, "Expect ']'.");
        ConfigurationNode node = new ConfigurationNode(null);
        consume(TokenType.LBRACE, "Expect '{' to start configuration block.");
        while(!check(TokenType.RBRACE) && !isAtEnd()) {
            if(match(TokenType.LBRACKET)) {
                if (match(TokenType.NAME)) {
                    consume(TokenType.RBRACKET, "Expect ']' to end name keyword.");
                    parseNameBlock(node);
                } else {
                    throw new ParseError(previous(), "Unsupported block in [Configuration].");
                }
            } else {
                Token key = consume(TokenType.IDENTIFIER, "Expect setting key.");
                consume(TokenType.EQUALS, "Expect '=' after setting key.");
                Token value = advance();
                consume(TokenType.SEMICOLON, "Expect ';' after setting value.");
                node.addSetting(key.getValue(), value.getValue());
            }
        }
        consume(TokenType.RBRACE, "Expect '}' to end configuration block.");
        return node;
    }

    private void parseNameBlock(ConfigurationNode configNode) {
        consume(TokenType.LBRACE, "Expect '{' to start name block.");
        while(!check(TokenType.RBRACE) && !isAtEnd()) {
            Token key = consume(TokenType.IDENTIFIER, "Expect keyword variable name.");
            consume(TokenType.EQUALS, "Expect '='.");

            // The value can be complex. For now, just grab the next token's value.
            Token value = advance();

            consume(TokenType.SEMICOLON, "Expect ';'.");
            configNode.addSetting(key.getValue(), value.getValue());
        }
        consume(TokenType.RBRACE, "Expect '}' to end name block.");
    }

    private BaseNode parseImportDeclaration() {
        consume(TokenType.LBRACKET, "Expect '['."); consume(TokenType.IMPORT, "Expect 'Import' keyword."); consume(TokenType.RBRACKET, "Expect ']'.");
        consume(TokenType.AT, "Expect '@' before import type.");
        Token type = consume(TokenType.IDENTIFIER, "Expect import type (e.g., Chtl).");
        consume(TokenType.FROM, "Expect 'from' keyword.");
        Token pathToken = consume(TokenType.STRING, "Expect a string literal for the path.");
        String path = pathToken.getValue();
        if (!type.getValue().equals("Chtl")) throw new ParseError(type, "Only '@Chtl' imports are currently supported.");
        try {
            String content = loader.loadFile(path);
            String newNamespace = Paths.get(path).getFileName().toString().replace(".chtl", "");
            CHTLLexer importedLexer = new CHTLLexer(content);
            List<Token> importedTokens = importedLexer.tokenize();
            CHTLParser importedParser = new CHTLParser(content, importedTokens, this.definitionManager, newNamespace, this.loader);
            importedParser.parse();
        } catch (IOException e) {
            throw new ParseError(pathToken, "Could not load file: " + path);
        }
        return new ImportNode(path);
    }

    private BaseNode parseBracketedDefinition() {
        consume(TokenType.LBRACKET, "Expect '['."); Token declarationType = advance(); consume(TokenType.RBRACKET, "Expect ']'.");
        consume(TokenType.AT, "Expect '@' before type.");
        Token type = consume(TokenType.IDENTIFIER, "Expect type (Style, Element, Var).");
        boolean isCustom = declarationType.getType() == TokenType.CUSTOM;
        switch (type.getValue()) {
            case "Style": return isCustom ? parseCustomStyle() : parseStyleTemplate();
            case "Element": return isCustom ? parseCustomElement() : parseElementTemplate();
            case "Var": return isCustom ? parseCustomVar() : parseVarTemplate();
            default: throw new ParseError(type, "Unknown definition type: " + type.getValue());
        }
    }

    private BaseNode parseTemplateUsage() {
        consume(TokenType.AT, "Expect '@' to start template usage.");
        Token type = consume(TokenType.IDENTIFIER, "Expect template type (Element).");
        if (!type.getValue().equals("Element")) throw new ParseError(type, "Only @Element is allowed at this level.");
        Token name = consume(TokenType.IDENTIFIER, "Expect template name.");
        String fromNamespace = this.namespace;
        if (match(TokenType.FROM)) fromNamespace = consume(TokenType.IDENTIFIER, "Expect namespace name after 'from'.").getValue();
        ElementTemplateUsageNode usageNode = new ElementTemplateUsageNode(name.getValue(), fromNamespace);
        if (match(TokenType.LBRACE)) {
            while (!check(TokenType.RBRACE) && !isAtEnd()) {
                if (check(TokenType.DELETE)) usageNode.addInstruction(parseDeleteInstruction());
                else if (check(TokenType.INSERT)) usageNode.addInstruction(parseInsertInstruction());
                else throw new ParseError(peek(), "Unsupported instruction in element specialization block.");
            }
            consume(TokenType.RBRACE, "Expect '}' to end element specialization block.");
        } else {
            consume(TokenType.SEMICOLON, "Expect ';' after element template usage.");
        }
        return usageNode;
    }

    private DeleteInstructionNode parseDeleteInstruction() {
        consume(TokenType.DELETE, "Expect 'delete' keyword.");
        StringBuilder selectorBuilder = new StringBuilder();
        selectorBuilder.append(consume(TokenType.IDENTIFIER, "Expect selector after delete.").getValue());
        if(match(TokenType.LBRACKET)) {
            selectorBuilder.append("[");
            selectorBuilder.append(consume(TokenType.NUMBER, "Expect index in brackets.").getValue());
            selectorBuilder.append("]");
            consume(TokenType.RBRACKET, "Expect ']' after index.");
        }
        consume(TokenType.SEMICOLON, "Expect ';' after delete instruction.");
        return new DeleteInstructionNode(selectorBuilder.toString());
    }

    private InsertInstructionNode parseInsertInstruction() {
        consume(TokenType.INSERT, "Expect 'insert' keyword.");
        InsertInstructionNode.Mode mode;
        Token modeToken = advance();
        switch(modeToken.getType()) {
            case AFTER: mode = InsertInstructionNode.Mode.AFTER; break;
            case BEFORE: mode = InsertInstructionNode.Mode.BEFORE; break;
            case REPLACE: mode = InsertInstructionNode.Mode.REPLACE; break;
            default:
                if (modeToken.getValue().equals("at")) {
                    Token next = consume(TokenType.IDENTIFIER, "Expect 'top' or 'bottom' after 'at'.");
                    if (next.getValue().equals("top")) mode = InsertInstructionNode.Mode.AT_TOP;
                    else if (next.getValue().equals("bottom")) mode = InsertInstructionNode.Mode.AT_BOTTOM;
                    else throw new ParseError(next, "Invalid insert position.");
                } else throw new ParseError(modeToken, "Invalid insert position keyword.");
                break;
        }
        String selector = null;
        if (mode == InsertInstructionNode.Mode.AFTER || mode == InsertInstructionNode.Mode.BEFORE || mode == InsertInstructionNode.Mode.REPLACE) {
            StringBuilder selectorBuilder = new StringBuilder();
            selectorBuilder.append(consume(TokenType.IDENTIFIER, "Expect selector for insert.").getValue());
             if(match(TokenType.LBRACKET)) {
                selectorBuilder.append("[");
                selectorBuilder.append(consume(TokenType.NUMBER, "Expect index in brackets.").getValue());
                selectorBuilder.append("]");
                consume(TokenType.RBRACKET, "Expect ']' after index.");
            }
            selector = selectorBuilder.toString();
        }
        List<BaseNode> nodesToInsert = new ArrayList<>();
        consume(TokenType.LBRACE, "Expect '{' to start insert block.");
        while(!check(TokenType.RBRACE) && !isAtEnd()) nodesToInsert.add(declaration());
        consume(TokenType.RBRACE, "Expect '}' to end insert block.");
        return new InsertInstructionNode(mode, selector, nodesToInsert);
    }

    private StyleTemplateNode parseStyleTemplate() {
        Token name = consume(TokenType.IDENTIFIER, "Expect template name.");
        StyleTemplateNode template = new StyleTemplateNode(name.getValue());
        consume(TokenType.LBRACE, "Expect '{' to start style template block.");
        while (!check(TokenType.RBRACE) && !isAtEnd()) {
            if (check(TokenType.AT) || check(TokenType.INHERIT)) {
                if (match(TokenType.INHERIT)) {}
                consume(TokenType.AT, "Expect '@' for template inheritance.");
                consume(TokenType.IDENTIFIER, "Expect 'Style' keyword for inheritance.");
                Token parentName = consume(TokenType.IDENTIFIER, "Expect parent template name.");
                consume(TokenType.SEMICOLON, "Expect ';' after parent template name.");
                template.addParentTemplate(parentName.getValue());
            } else {
                Token key = consume(TokenType.IDENTIFIER, "Expect property name.");
                consume(TokenType.COLON, "Expect ':' after property name.");
                StringBuilder valueBuilder = new StringBuilder();
                while (!check(TokenType.SEMICOLON) && !isAtEnd()) {
                    valueBuilder.append(advance().getValue());
                    if (!check(TokenType.SEMICOLON)) valueBuilder.append(" ");
                }
                consume(TokenType.SEMICOLON, "Expect ';' after property value.");
                template.addProperty(key.getValue(), valueBuilder.toString().trim());
            }
        }
        consume(TokenType.RBRACE, "Expect '}' to end style template block.");
        definitionManager.registerStyleTemplate(this.namespace, template);
        return template;
    }

    private ElementTemplateNode parseElementTemplate() {
        Token name = consume(TokenType.IDENTIFIER, "Expect template name.");
        ElementTemplateNode template = new ElementTemplateNode(name.getValue());
        consume(TokenType.LBRACE, "Expect '{' to start element template block.");
        while (!check(TokenType.RBRACE) && !isAtEnd()) template.addChild(declaration());
        consume(TokenType.RBRACE, "Expect '}' to end element template block.");
        definitionManager.registerElementTemplate(this.namespace, template);
        return template;
    }

    private VarTemplateNode parseVarTemplate() {
        Token name = consume(TokenType.IDENTIFIER, "Expect template name.");
        VarTemplateNode template = new VarTemplateNode(name.getValue());
        consume(TokenType.LBRACE, "Expect '{' to start var template block.");
        while (!check(TokenType.RBRACE) && !isAtEnd()) {
            Token key = consume(TokenType.IDENTIFIER, "Expect variable name.");
            consume(TokenType.COLON, "Expect ':' after variable name.");
            Token value = consume(TokenType.STRING, "Expect string literal for variable value.");
            consume(TokenType.SEMICOLON, "Expect ';' after variable value.");
            template.addVariable(key.getValue(), value.getValue());
        }
        consume(TokenType.RBRACE, "Expect '}' to end var template block.");
        definitionManager.registerVarTemplate(this.namespace, template);
        return template;
    }

    private CustomStyleNode parseCustomStyle() {
        Token name = consume(TokenType.IDENTIFIER, "Expect custom style name.");
        CustomStyleNode custom = new CustomStyleNode(name.getValue());
        consume(TokenType.LBRACE, "Expect '{' to start custom style block.");
        while (!check(TokenType.RBRACE) && !isAtEnd()) {
            Token key = consume(TokenType.IDENTIFIER, "Expect property name.");
            if (match(TokenType.COLON)) {
                StringBuilder valueBuilder = new StringBuilder();
                while (!check(TokenType.SEMICOLON) && !isAtEnd()) {
                    valueBuilder.append(advance().getValue());
                    if (!check(TokenType.SEMICOLON)) valueBuilder.append(" ");
                }
                consume(TokenType.SEMICOLON, "Expect ';' after property value.");
                custom.addProperty(key.getValue(), valueBuilder.toString().trim());
            } else {
                custom.addValuelessProperty(key.getValue());
                if (check(TokenType.COMMA)) advance();
                else consume(TokenType.SEMICOLON, "Expect ';' or ',' after valueless property.");
            }
        }
        consume(TokenType.RBRACE, "Expect '}' to end custom style block.");
        definitionManager.registerCustomStyle(this.namespace, custom);
        return custom;
    }

    private CustomElementNode parseCustomElement() {
        Token name = consume(TokenType.IDENTIFIER, "Expect custom element name.");
        CustomElementNode custom = new CustomElementNode(name.getValue());
        consume(TokenType.LBRACE, "Expect '{' to start custom element block.");
        while (!check(TokenType.RBRACE) && !isAtEnd()) custom.addChild(declaration());
        consume(TokenType.RBRACE, "Expect '}' to end custom element block.");
        definitionManager.registerCustomElement(this.namespace, custom);
        return custom;
    }

    private CustomVarNode parseCustomVar() {
        Token name = consume(TokenType.IDENTIFIER, "Expect custom var name.");
        CustomVarNode custom = new CustomVarNode(name.getValue());
        consume(TokenType.LBRACE, "Expect '{' to start custom var block.");
        while (!check(TokenType.RBRACE) && !isAtEnd()) {
            Token key = consume(TokenType.IDENTIFIER, "Expect variable name.");
            consume(TokenType.COLON, "Expect ':' after variable name.");
            Token value = consume(TokenType.STRING, "Expect string literal for variable value.");
            consume(TokenType.SEMICOLON, "Expect ';' after variable value.");
            custom.addVariable(key.getValue(), value.getValue());
        }
        consume(TokenType.RBRACE, "Expect '}' to end custom var block.");
        definitionManager.registerCustomVar(this.namespace, custom);
        return custom;
    }

    private BaseNode textBlock() {
        consume(TokenType.LBRACE, "Expect '{' after 'text'.");
        Token content = consume(TokenType.STRING, "Expect string literal inside text block.");
        consume(TokenType.RBRACE, "Expect '}' after text block content.");
        return new TextNode(content.getValue());
    }

    private ElementNode element(String tagName) {
        consume(TokenType.LBRACE, "Expect '{' after element name.");
        ElementNode node = new ElementNode(tagName);
        while (!check(TokenType.RBRACE) && !isAtEnd()) {
            if ((check(TokenType.IDENTIFIER) || check(TokenType.TEXT)) && (tokenAt(current + 1).getType() == TokenType.COLON || tokenAt(current + 1).getType() == TokenType.EQUALS)) {
                parseAttribute(node);
            } else {
                node.addChild(declaration());
            }
        }
        consume(TokenType.RBRACE, "Expect '}' after element block.");
        return node;
    }

    private void parseAttribute(ElementNode node) {
        Token key = advance();
        match(TokenType.COLON, TokenType.EQUALS);
        Token value;
        if (match(TokenType.STRING, TokenType.IDENTIFIER, TokenType.NUMBER)) value = previous();
        else throw new ParseError(peek(), "Expect attribute value.");
        consume(TokenType.SEMICOLON, "Expect ';' after attribute value.");
        if (key.getType() == TokenType.TEXT) node.addChild(new TextNode(value.getValue()));
        else node.addAttribute(key.getValue(), value.getValue());
    }

    private StyleNode parseStyleBlock() {
        consume(TokenType.STYLE, "Expect 'style' keyword.");
        consume(TokenType.LBRACE, "Expect '{' after 'style'.");
        StyleNode styleNode = new StyleNode();
        while (!check(TokenType.RBRACE) && !isAtEnd()) {
            if (check(TokenType.HASH) || check(TokenType.DOT) || check(TokenType.AMPERSAND)) styleNode.addRule(parseStyleRule());
            else if (check(TokenType.AT)) parseStyleTemplateUsage(styleNode);
            else if (check(TokenType.IDENTIFIER)) parseInlineStyleProperty(styleNode);
            else throw new ParseError(peek(), "Expect a style property, rule, or template usage.");
        }
        consume(TokenType.RBRACE, "Expect '}' after style block.");
        return styleNode;
    }

    private void parseStyleTemplateUsage(StyleNode styleNode) {
        consume(TokenType.AT, "Expect '@' to start template usage.");
        consume(TokenType.IDENTIFIER, "Expect 'Style' keyword for style template usage.");
        Token templateName = consume(TokenType.IDENTIFIER, "Expect template name.");
        String fromNamespace = this.namespace;
        if (match(TokenType.FROM)) fromNamespace = consume(TokenType.IDENTIFIER, "Expect namespace name after 'from'.").getValue();
        StyleTemplateUsageNode usageNode = new StyleTemplateUsageNode(templateName.getValue(), fromNamespace);
        if (match(TokenType.LBRACE)) {
            while (!check(TokenType.RBRACE) && !isAtEnd()) {
                if (match(TokenType.DELETE)) {
                    do {
                        Token propToDelete = consume(TokenType.IDENTIFIER, "Expect property name to delete.");
                        usageNode.addDeletedProperty(propToDelete.getValue());
                    } while (match(TokenType.COMMA));
                    consume(TokenType.SEMICOLON, "Expect ';' after delete statement.");
                } else {
                    Token key = consume(TokenType.IDENTIFIER, "Expect property name in specialization block.");
                    consume(TokenType.COLON, "Expect ':' after property name.");
                    StringBuilder valueBuilder = new StringBuilder();
                    while (!check(TokenType.SEMICOLON) && !isAtEnd()) {
                        valueBuilder.append(advance().getValue());
                        if (!check(TokenType.SEMICOLON)) valueBuilder.append(" ");
                    }
                    consume(TokenType.SEMICOLON, "Expect ';' after property value.");
                    usageNode.addSpecializationProperty(key.getValue(), valueBuilder.toString().trim());
                }
            }
            consume(TokenType.RBRACE, "Expect '}' to end specialization block.");
        } else {
            consume(TokenType.SEMICOLON, "Expect ';' after template usage.");
        }
        styleNode.addTemplateUsage(usageNode);
    }

    private void parseInlineStyleProperty(StyleNode styleNode) {
        Token key = consume(TokenType.IDENTIFIER, "Expect style property name.");
        consume(TokenType.COLON, "Expect ':' after property name.");
        StringBuilder valueBuilder = new StringBuilder();
        while (!check(TokenType.SEMICOLON) && !isAtEnd()) {
            valueBuilder.append(advance().getValue());
            if (!check(TokenType.SEMICOLON)) valueBuilder.append(" ");
        }
        consume(TokenType.SEMICOLON, "Expect ';' after property value.");
        styleNode.addInlineProperty(key.getValue(), valueBuilder.toString().trim());
    }

    private StyleRule parseStyleRule() {
        StringBuilder selectorBuilder = new StringBuilder();
        while (!check(TokenType.LBRACE) && !isAtEnd()) selectorBuilder.append(advance().getValue());
        String selector = selectorBuilder.toString().trim();
        if (selector.isEmpty()) throw new ParseError(peek(), "Expect a selector before '{'.");
        StyleRule rule = new StyleRule(selector);
        consume(TokenType.LBRACE, "Expect '{' after selector.");
        while (!check(TokenType.RBRACE) && !isAtEnd()) {
            Token key = consume(TokenType.IDENTIFIER, "Expect style property name.");
            consume(TokenType.COLON, "Expect ':' after property name.");
            StringBuilder valueBuilder = new StringBuilder();
            while (!check(TokenType.SEMICOLON) && !isAtEnd()) {
                valueBuilder.append(advance().getValue());
                if (!check(TokenType.SEMICOLON)) valueBuilder.append(" ");
            }
            consume(TokenType.SEMICOLON, "Expect ';' after property value.");
            rule.addProperty(key.getValue(), valueBuilder.toString().trim());
        }
        consume(TokenType.RBRACE, "Expect '}' after rule block.");
        return rule;
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

    private Token tokenAt(int index) {
        if (index >= tokens.size()) return tokens.get(tokens.size() - 1);
        return tokens.get(index);
    }
}

package CHTL.Parser;

import CHTL.Lexer.Token;
import CHTL.Lexer.TokenType;
import CHTL.Manage.DefinitionManager;
import CHTL.Node.*;
import java.util.ArrayList;
import java.util.List;

public class CHTLParser {
    private final List<Token> tokens;
    private int current = 0;
    private final DefinitionManager definitionManager;
    private String currentNamespace = DefinitionManager.DEFAULT_NAMESPACE;

    public CHTLParser(List<Token> tokens, DefinitionManager definitionManager) {
        this.tokens = tokens;
        this.definitionManager = definitionManager;
    }

    public List<BaseNode> parse() {
        List<BaseNode> nodes = new ArrayList<>();
        while (!isAtEnd()) {
            nodes.add(declaration());
        }
        return nodes;
    }

    private BaseNode declaration() {
        if (check(TokenType.LBRACKET)) return bracketedDeclaration();
        if (check(TokenType.AT)) return elementTemplateUsage();
        return element();
    }

    private BaseNode bracketedDeclaration() {
        consume(TokenType.LBRACKET, "Expect '['.");
        Token type = advance();
        consume(TokenType.RBRACKET, "Expect ']'.");
        switch (type.type()) {
            case TEMPLATE: return templateDefinition(false);
            case CUSTOM: return templateDefinition(true);
            default: throw new RuntimeException("Unexpected token in bracket declaration: " + type.value());
        }
    }

    private BaseNode templateDefinition(boolean isCustom) {
        consume(TokenType.AT, "Expect '@'.");
        Token templateType = consume(TokenType.IDENTIFIER, "Expect template type.");
        Token templateName = consume(TokenType.IDENTIFIER, "Expect template name.");
        switch (templateType.value()) {
            case "Style": return isCustom ? parseCustomStyle(templateName.value()) : parseStyleTemplate(templateName.value());
            case "Element": return isCustom ? parseCustomElement(templateName.value()) : parseElementTemplate(templateName.value());
            case "Var": return isCustom ? parseCustomVar(templateName.value()) : parseVarTemplate(templateName.value());
            default: throw new RuntimeException("Unknown template type: " + templateType.value());
        }
    }

    private StyleTemplateNode parseStyleTemplate(String name) {
        StyleTemplateNode node = new StyleTemplateNode(name);
        consume(TokenType.LBRACE, "Expect '{'.");
        while (!check(TokenType.RBRACE)) parseStylePropertyOrInheritance(node);
        consume(TokenType.RBRACE, "Expect '}'.");
        definitionManager.register(currentNamespace, name, node);
        return node;
    }

    private void parseStylePropertyOrInheritance(StyleTemplateNode node) {
        if (match(TokenType.AT) || match(TokenType.INHERIT)) {
            consume(TokenType.AT, "Expect '@'.");
            consume(TokenType.IDENTIFIER, "Expect 'Style'.");
            node.addParentTemplate(consume(TokenType.IDENTIFIER, "Expect parent name.").value());
            consume(TokenType.SEMICOLON, "Expect ';'.");
        } else {
            Token key = consume(TokenType.IDENTIFIER, "Expect property name.");
            match(TokenType.COLON, TokenType.EQUALS);
            node.addProperty(key.value(), consume(TokenType.STRING, "Expect property value.").value());
            consume(TokenType.SEMICOLON, "Expect ';'.");
        }
    }

    private CustomStyleNode parseCustomStyle(String name) {
        CustomStyleNode node = new CustomStyleNode(name);
        consume(TokenType.LBRACE, "Expect '{'.");
        while (!check(TokenType.RBRACE)) {
            if (check(TokenType.EXCEPT)) {
                advance();
                consume(TokenType.LBRACE, "Expect '{'.");
                do { node.addConstrainedProperty(consume(TokenType.IDENTIFIER, "Expect property name.").value()); } while (match(TokenType.COMMA));
                consume(TokenType.SEMICOLON, "Expect ';'.");
                consume(TokenType.RBRACE, "Expect '}'.");
            } else if (check(TokenType.AT) || check(TokenType.INHERIT)){
                parseStylePropertyOrInheritance(node);
            }
            else {
                Token key = consume(TokenType.IDENTIFIER, "Expect property name.");
                if (match(TokenType.COLON, TokenType.EQUALS)) {
                    node.addProperty(key.value(), consume(TokenType.STRING, "Expect value.").value());
                    consume(TokenType.SEMICOLON, "Expect ';'.");
                } else {
                    node.addValuelessProperty(key.value());
                    if(!check(TokenType.RBRACE)) consume(TokenType.COMMA, "Expect ',' or '}'.");
                }
            }
        }
        consume(TokenType.RBRACE, "Expect '}'.");
        definitionManager.register(currentNamespace, name, node);
        return node;
    }

    private ElementTemplateNode parseElementTemplate(String name) {
        ElementTemplateNode node = new ElementTemplateNode(name);
        consume(TokenType.LBRACE, "Expect '{'.");
        while (!check(TokenType.RBRACE)) node.addChild(declaration());
        consume(TokenType.RBRACE, "Expect '}'.");
        definitionManager.register(currentNamespace, name, node);
        return node;
    }

    private CustomElementNode parseCustomElement(String name) {
        CustomElementNode node = new CustomElementNode(name);
        consume(TokenType.LBRACE, "Expect '{'.");
        while (!check(TokenType.RBRACE)) node.addChild(declaration());
        consume(TokenType.RBRACE, "Expect '}'.");
        definitionManager.register(currentNamespace, name, node);
        return node;
    }

    private VarTemplateNode parseVarTemplate(String name) {
        VarTemplateNode node = new VarTemplateNode(name);
        consume(TokenType.LBRACE, "Expect '{'.");
        while(!check(TokenType.RBRACE)) {
            Token key = consume(TokenType.IDENTIFIER, "Expect var name.");
            match(TokenType.COLON, TokenType.EQUALS);
            node.addVariable(key.value(), consume(TokenType.STRING, "Expect var value.").value());
            consume(TokenType.SEMICOLON, "Expect ';'.");
        }
        consume(TokenType.RBRACE, "Expect '}'.");
        definitionManager.register(currentNamespace, name, node);
        return node;
    }

    private CustomVarNode parseCustomVar(String name) {
        CustomVarNode node = new CustomVarNode(name);
        consume(TokenType.LBRACE, "Expect '{'.");
        while(!check(TokenType.RBRACE)) {
            Token key = consume(TokenType.IDENTIFIER, "Expect var name.");
            match(TokenType.COLON, TokenType.EQUALS);
            node.addVariable(key.value(), consume(TokenType.STRING, "Expect var value.").value());
            consume(TokenType.SEMICOLON, "Expect ';'.");
        }
        consume(TokenType.RBRACE, "Expect '}'.");
        definitionManager.register(currentNamespace, name, node);
        return node;
    }

    private ElementTemplateUsageNode elementTemplateUsage() {
        consume(TokenType.AT, "Expect '@'.");
        consume(TokenType.IDENTIFIER, "Expect 'Element'.");
        Token templateName = consume(TokenType.IDENTIFIER, "Expect template name.");
        String namespace = currentNamespace;
        if (match(TokenType.FROM)) namespace = consume(TokenType.IDENTIFIER, "Expect namespace.").value();

        ElementTemplateUsageNode node = new ElementTemplateUsageNode(templateName.value(), namespace);

        if (match(TokenType.LBRACE)) {
            while(!check(TokenType.RBRACE)) node.addInstruction(parseSpecializationInstruction());
            consume(TokenType.RBRACE, "Expect '}'.");
        } else {
            consume(TokenType.SEMICOLON, "Expect ';'.");
        }
        return node;
    }

    private BaseNode parseSpecializationInstruction() {
        if(check(TokenType.DELETE)) return parseDeleteInstruction();
        if(check(TokenType.INSERT)) return parseInsertInstruction();
        return parseAddStyleInstruction();
    }

    private DeleteInstructionNode parseDeleteInstruction() {
        consume(TokenType.DELETE, "Expect 'delete'.");
        String selector = consume(TokenType.IDENTIFIER, "Expect selector.").value();
        consume(TokenType.SEMICOLON, "Expect ';'.");
        return new DeleteInstructionNode(selector);
    }

    private InsertInstructionNode parseInsertInstruction() {
        consume(TokenType.INSERT, "Expect 'insert'.");
        Token modeToken = advance();
        InsertInstructionNode.Mode mode = InsertInstructionNode.Mode.valueOf(modeToken.value().toUpperCase());
        String selector = consume(TokenType.IDENTIFIER, "Expect selector.").value();
        List<BaseNode> nodesToInsert = new ArrayList<>();
        consume(TokenType.LBRACE, "Expect '{'.");
        while(!check(TokenType.RBRACE)) nodesToInsert.add(declaration());
        consume(TokenType.RBRACE, "Expect '}'.");
        return new InsertInstructionNode(mode, selector, nodesToInsert);
    }

    private AddStyleInstructionNode parseAddStyleInstruction() {
        String selector = consume(TokenType.IDENTIFIER, "Expect selector.").value();
        StyleNode styleNode = parseStyleBlock();
        return new AddStyleInstructionNode(selector, styleNode);
    }

    private ElementNode element() {
        Token name = consume(TokenType.IDENTIFIER, "Expect element name.");
        ElementNode node = new ElementNode(name.value());
        if (match(TokenType.LBRACE)) {
             while (!check(TokenType.RBRACE)) {
                if (check(TokenType.STYLE)) node.addChild(parseStyleBlock());
                else if (check(TokenType.IDENTIFIER) && (lookahead().type() == TokenType.COLON || lookahead().type() == TokenType.EQUALS)) parseAttribute(node);
                else node.addChild(declaration());
            }
            consume(TokenType.RBRACE, "Expect '}' to close element block.");
        } else {
            consume(TokenType.SEMICOLON, "Expect ';' after single-line element.");
        }
        return node;
    }

    private void parseAttribute(ElementNode node) {
        Token key = advance();
        match(TokenType.COLON, TokenType.EQUALS);
        node.addAttribute(key.value(), consume(TokenType.STRING, "Expect attribute value.").value());
        consume(TokenType.SEMICOLON, "Expect ';' after attribute value.");
    }

    private StyleNode parseStyleBlock() {
        consume(TokenType.STYLE, "Expect 'style' keyword.");
        consume(TokenType.LBRACE, "Expect '{' after 'style'.");
        StyleNode styleNode = new StyleNode();
        while(!check(TokenType.RBRACE)) {
            if (check(TokenType.AT)) {
                styleNode.addUsage(parseStyleTemplateUsage());
            } else {
                Token property = consume(TokenType.IDENTIFIER, "Expect property name.");
                match(TokenType.COLON, TokenType.EQUALS);
                styleNode.addProperty(property.value(), consume(TokenType.STRING, "Expect property value.").value());
                consume(TokenType.SEMICOLON, "Expect ';' after property value.");
            }
        }
        consume(TokenType.RBRACE, "Expect '}' to close style block.");
        return styleNode;
    }

    private StyleTemplateUsageNode parseStyleTemplateUsage() {
        consume(TokenType.AT, "Expect '@'.");
        consume(TokenType.IDENTIFIER, "Expect 'Style'.");
        Token templateName = consume(TokenType.IDENTIFIER, "Expect template name.");
        String namespace = currentNamespace;
        if(match(TokenType.FROM)) namespace = consume(TokenType.IDENTIFIER, "Expect namespace.").value();
        StyleTemplateUsageNode usage = new StyleTemplateUsageNode(templateName.value(), namespace);
        if(match(TokenType.LBRACE)) {
            while(!check(TokenType.RBRACE)) {
                if(match(TokenType.DELETE)) {
                    do {
                        usage.addDeletion(consume(TokenType.IDENTIFIER, "Expect property to delete.").value());
                    } while(match(TokenType.COMMA));
                    consume(TokenType.SEMICOLON, "Expect ';'.");
                } else {
                    Token key = consume(TokenType.IDENTIFIER, "Expect property name.");
                    match(TokenType.COLON, TokenType.EQUALS);
                    usage.addSpecialization(key.value(), consume(TokenType.STRING, "Expect value.").value());
                    consume(TokenType.SEMICOLON, "Expect ';'.");
                }
            }
            consume(TokenType.RBRACE, "Expect '}'.");
        } else {
            consume(TokenType.SEMICOLON, "Expect ';'.");
        }
        return usage;
    }

    // Utility methods
    private Token peek() { return tokens.get(current); }
    private Token lookahead() { if (current + 1 >= tokens.size()) return tokens.get(tokens.size() - 1); return tokens.get(current + 1); }
    private Token previous() { return tokens.get(current - 1); }
    private boolean isAtEnd() { return peek().type() == TokenType.EOF; }
    private boolean check(TokenType type) { if (isAtEnd()) return false; return peek().type() == type; }
    private Token advance() { if (!isAtEnd()) current++; return previous(); }
    private boolean match(TokenType... types) {
        for (TokenType type : types) { if (check(type)) { advance(); return true; } }
        return false;
    }
    private Token consume(TokenType type, String message) {
        if (check(type)) return advance();
        throw new RuntimeException(message + " Got " + peek().type() + " instead.");
    }
}

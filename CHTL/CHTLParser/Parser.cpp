#include "Parser.h"
#include <string>

Parser::Parser(Lexer& lexer, Context& context) : lexer(lexer), context(context) {
    // Initialize with the first two tokens
    nextToken();
    nextToken();
}

// Advance the tokens
void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.nextToken();
}

bool Parser::currentTokenIs(TokenType type) const {
    return currentToken.type == type;
}

bool Parser::peekTokenIs(TokenType type) const {
    return peekToken.type == type;
}

// "Assertion" for the next token. If it's not what we expect, add an error.
bool Parser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    } else {
        peekError(type);
        return false;
    }
}

void Parser::peekError(TokenType type) {
    errors.push_back("Expected next token to be " + tokenTypeToString(type) +
                     ", but got " + tokenTypeToString(peekToken.type) + " instead.");
}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

// Main entry point for parsing.
std::shared_ptr<Program> Parser::parseProgram() {
    auto program = std::make_shared<Program>();
    while (!currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(stmt);
        }
        nextToken();
    }
    return program;
}

// Forward declaration for the new helper
std::shared_ptr<TextNode> parseTextAttributeAsNode();

// Dispatcher for different statement types.
std::shared_ptr<Statement> Parser::parseStatement() {
    if (currentTokenIs(TokenType::LEFT_BRACKET)) {
        return parseTemplateStatement();
    }

    if (currentTokenIs(TokenType::HASH_COMMENT)) {
        auto node = std::make_shared<CommentNode>(currentToken, currentToken.literal);
        nextToken(); // Consume comment
        return node;
    }

    if (currentTokenIs(TokenType::IDENTIFIER)) {
        if (peekTokenIs(TokenType::LEFT_BRACE)) {
            // Block statements are differentiated by the identifier's value.
            if (currentToken.literal == "text") {
                return parseTextNode();
            } else if (currentToken.literal == "style") {
                return parseStyleNode();
            } else {
                // Any other identifier followed by a brace is an element.
                return parseElementNode();
            }
        }
    }

    return nullptr; // Not a statement we recognize at this level.
}

// Helper to parse `text: "value";` into a TextNode
std::shared_ptr<TextNode> Parser::parseTextAttributeAsNode() {
    Token textKeywordToken = currentToken; // "text"
    nextToken(); // consume "text"

    if (!currentTokenIs(TokenType::COLON) && !currentTokenIs(TokenType::EQUALS)) {
        errors.push_back("Expected ':' or '=' after 'text' attribute.");
        return nullptr;
    }
    nextToken(); // consume ":" or "="

    auto expr = parseExpression();
    if (!expr) {
        errors.push_back("Expected an expression after 'text:'.");
        return nullptr;
    }

    std::string textContent;
    if (auto strExpr = std::dynamic_pointer_cast<StringLiteralNode>(expr)) {
        textContent = strExpr->value;
    } else if (auto identExpr = std::dynamic_pointer_cast<IdentifierNode>(expr)) {
        textContent = identExpr->value;
    }

    nextToken(); // consume expression token

    if (currentTokenIs(TokenType::SEMICOLON)) {
        nextToken(); // consume optional semicolon
    }

    return std::make_shared<TextNode>(textKeywordToken, textContent);
}


std::shared_ptr<ElementNode> Parser::parseElementNode() {
    auto node = std::make_shared<ElementNode>(currentToken, currentToken.literal);
    nextToken(); // consume tag name

    if (!currentTokenIs(TokenType::LEFT_BRACE)) {
        errors.push_back("Expected '{' after element name.");
        return nullptr;
    }
    nextToken(); // consume '{'

    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
            if (currentToken.literal == "text") {
                auto textNode = parseTextAttributeAsNode();
                if (textNode) {
                    node->children.push_back(textNode);
                }
            } else {
                auto attr = parseAttributeNode();
                if (attr) {
                    node->attributes.push_back(attr);
                }
            }
        } else {
            auto stmt = parseStatement();
            if (stmt) {
                node->children.push_back(stmt);
            } else {
                errors.push_back("Invalid token inside element block: " + currentToken.literal);
                nextToken(); // To prevent infinite loops on invalid tokens
            }
        }
    }

    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
       errors.push_back("Missing '}' after element block.");
       return nullptr;
    }
    nextToken(); // consume '}'

    return node;
}

std::shared_ptr<AttributeNode> Parser::parseAttributeNode() {
    Token keyToken = currentToken;
    nextToken(); // consume key

    if (!currentTokenIs(TokenType::COLON) && !currentTokenIs(TokenType::EQUALS)) {
        errors.push_back("Expected ':' or '=' after attribute key.");
        return nullptr;
    }
    nextToken(); // consume ':' or '='

    auto value = parseExpression();
    if (!value) {
        errors.push_back("Expected expression for attribute value.");
        return nullptr;
    }
    nextToken(); // consume value expression

    if (currentTokenIs(TokenType::SEMICOLON)) {
        nextToken(); // consume optional semicolon
    }

    return std::make_shared<AttributeNode>(keyToken, keyToken.literal, value);
}

std::shared_ptr<Expression> Parser::parseExpression() {
    if (currentTokenIs(TokenType::STRING_LITERAL)) {
        return std::make_shared<StringLiteralNode>(currentToken, currentToken.literal);
    }
    if (currentTokenIs(TokenType::IDENTIFIER) || currentTokenIs(TokenType::UNQUOTED_LITERAL)) {
        return std::make_shared<IdentifierNode>(currentToken, currentToken.literal);
    }
    return nullptr;
}


std::shared_ptr<TextNode> Parser::parseTextNode() {
    Token textToken = currentToken;
    nextToken(); // consume 'text'

    if (!currentTokenIs(TokenType::LEFT_BRACE)) {
        errors.push_back("Expected '{' after 'text' keyword.");
        return nullptr;
    }
    nextToken(); // consume '{'

    std::string content;
    if (currentTokenIs(TokenType::STRING_LITERAL) || currentTokenIs(TokenType::UNQUOTED_LITERAL) || currentTokenIs(TokenType::IDENTIFIER)) {
        content = currentToken.literal;
        nextToken(); // consume content
    } else {
        errors.push_back("Expected a literal inside text block.");
    }

    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
        errors.push_back("Missing '}' after text block.");
        return nullptr;
    }
    nextToken(); // consume '}'

    return std::make_shared<TextNode>(textToken, content);
}

// Parses a selector string, e.g., ".class", "#id", "&:hover"
std::string Parser::parseSelector() {
    std::string selector;
    // A selector can be a combination of tokens, e.g., & : : hover
    while (!peekTokenIs(TokenType::LEFT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        selector += currentToken.literal;
        nextToken();
    }
    selector += currentToken.literal; // append the last part
    nextToken(); // consume last part of selector
    return selector;
}


std::shared_ptr<TemplateUsageNode> Parser::parseTemplateUsageNode() {
    // Expects current token to be @
    nextToken(); // consume @
    if (!currentTokenIs(TokenType::IDENTIFIER)) return nullptr;
    Token typeToken = currentToken; // e.g. Style
    nextToken();

    if (!currentTokenIs(TokenType::IDENTIFIER)) return nullptr;
    Token nameToken = currentToken; // e.g. MyTemplate
    nextToken();

    if (!currentTokenIs(TokenType::SEMICOLON)) return nullptr;
    nextToken(); // consume ;

    return std::make_shared<TemplateUsageNode>(typeToken, nameToken.literal);
}


std::shared_ptr<StyleNode> Parser::parseStyleNode() {
    auto node = std::make_shared<StyleNode>(currentToken);
    nextToken(); // consume 'style'

    if (!currentTokenIs(TokenType::LEFT_BRACE)) {
        errors.push_back("Expected '{' after 'style' keyword.");
        return nullptr;
    }
    nextToken(); // consume '{'

    std::vector<std::shared_ptr<AttributeNode>> inlineProps;

    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::AT_SIGN)) {
            auto usage = parseTemplateUsageNode();
            if (usage) node->statements.push_back(usage);
        }
        else if (currentTokenIs(TokenType::DOT) || currentTokenIs(TokenType::POUND) || currentTokenIs(TokenType::AMPERSAND)) {
            // It's a nested rule
            auto selector = parseSelector();
            auto ruleNode = std::make_shared<CssRuleNode>(selector);

            if (!currentTokenIs(TokenType::LEFT_BRACE)) {
                errors.push_back("Expected '{' after selector.");
                while(!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) nextToken();
                continue;
            }
            nextToken(); // consume '{'

            while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
                if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
                    ruleNode->properties.push_back(parseAttributeNode());
                } else {
                    nextToken();
                }
            }
            nextToken(); // consume '}'
            node->statements.push_back(ruleNode);

        } else if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
            // It's a direct/inline property
            inlineProps.push_back(parseAttributeNode());
        } else {
            nextToken(); // Skip comments, etc.
        }
    }

    if (!inlineProps.empty()) {
        auto inlineRule = std::make_shared<CssRuleNode>("&");
        inlineRule->properties = inlineProps;
        node->statements.insert(node->statements.begin(), inlineRule);
    }

    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
        errors.push_back("Missing '}' after style block.");
        return nullptr;
    }
    nextToken(); // consume '}'

    return node;
}

std::shared_ptr<Statement> Parser::parseTemplateStatement() {
    // Expect [Template]
    if (!currentTokenIs(TokenType::LEFT_BRACKET)) return nullptr;
    nextToken(); // consume [
    if (currentToken.literal != "Template") return nullptr;
    nextToken(); // consume Template
    if (!currentTokenIs(TokenType::RIGHT_BRACKET)) return nullptr;
    nextToken(); // consume ]

    // Expect @Type
    if (!currentTokenIs(TokenType::AT_SIGN)) return nullptr;
    nextToken(); // consume @
    if (!currentTokenIs(TokenType::IDENTIFIER)) return nullptr;
    Token typeToken = currentToken;
    nextToken(); // consume Type

    // Expect Name
    if (!currentTokenIs(TokenType::IDENTIFIER)) return nullptr;
    Token nameToken = currentToken;
    nextToken(); // consume Name

    if (!currentTokenIs(TokenType::LEFT_BRACE)) return nullptr;
    nextToken(); // consume {

    if (typeToken.literal == "Style") {
        auto node = std::make_shared<StyleTemplateNode>(nameToken, nameToken.literal);
        while(!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
            if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
                node->properties.push_back(parseAttributeNode());
            } else {
                nextToken();
            }
        }
        context.addStyleTemplate(nameToken.literal, node);
        if (!currentTokenIs(TokenType::RIGHT_BRACE)) return nullptr;
        nextToken(); // consume }
        return node;

    } else if (typeToken.literal == "Element") {
        auto node = std::make_shared<ElementTemplateNode>(nameToken, nameToken.literal);
        while(!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
            auto stmt = parseStatement();
            if (stmt) {
                node->children.push_back(stmt);
            } else {
                nextToken();
            }
        }
        context.addElementTemplate(nameToken.literal, node);
        if (!currentTokenIs(TokenType::RIGHT_BRACE)) return nullptr;
        nextToken(); // consume }
        return node;

    } else if (typeToken.literal == "Var") {
        auto node = std::make_shared<VarTemplateNode>(nameToken, nameToken.literal);
         while(!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
            if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
                node->variables.push_back(parseAttributeNode());
            } else {
                nextToken();
            }
        }
        context.addVarTemplate(nameToken.literal, node);
        if (!currentTokenIs(TokenType::RIGHT_BRACE)) return nullptr;
        nextToken(); // consume }
        return node;
    }

    // Unknown template type, skip block
    while(!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        nextToken();
    }
    if (!currentTokenIs(TokenType::RIGHT_BRACE)) return nullptr;
    nextToken(); // consume }
    return nullptr;
}

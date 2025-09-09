#include "Parser.h"
#include "Parser.h" // Recursive include
#include "../CHTLLexer/Lexer.h"   // For recursive parsing
#include "../CHTLLoader/Loader.h" // To load imported files
#include <stdexcept>
#include <iostream>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens, CHTLContext& context)
    : m_tokens(tokens), m_context(context) {}

// --- Token Management (omitted for brevity, no changes) ---
bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}
Token Parser::peek() const {
    return m_tokens[m_current];
}
Token Parser::peekNext() const {
    if (isAtEnd() || m_current + 1 >= m_tokens.size()) return peek();
    return m_tokens[m_current + 1];
}
Token Parser::advance() {
    if (!isAtEnd()) m_current++;
    return m_tokens[m_current - 1];
}
bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}
Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error("Parse Error: " + message + " at line " + std::to_string(peek().line) + ". Got " + peek().lexeme);
}
// --- End Token Management ---


// --- Parsing Logic ---

std::string Parser::parseCssValue() {
    std::string value;
    while (!check(TokenType::SEMICOLON) && !check(TokenType::RBRACE) && !isAtEnd()) {
        if (!value.empty()) {
            value += " "; // Add space between tokens
        }
        value += advance().lexeme;
    }
    return value;
}

DocumentNode Parser::parse() {
    DocumentNode statements;
    while (!isAtEnd()) {
        auto statement = parseStatement();
        if (statement) {
            statements.push_back(std::move(statement));
        }
    }
    return statements;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (check(TokenType::KEYWORD_IMPORT)) {
        return parseImportStatement();
    }
    if (check(TokenType::KEYWORD_TEMPLATE)) {
        return parseTemplateDefinition();
    }
    if (check(TokenType::KEYWORD_TEXT)) {
        return parseText();
    }
    if (check(TokenType::KEYWORD_STYLE)) {
        return parseStyleBlock();
    }
    if (check(TokenType::IDENTIFIER)) {
        return parseElement();
    }

    std::cerr << "Warning: Skipping unexpected token '" << peek().lexeme
              << "' at line " << std::to_string(peek().line) << std::endl;
    advance();
    return nullptr;
}

std::unique_ptr<ImportNode> Parser::parseImportStatement() {
    consume(TokenType::KEYWORD_IMPORT, "Expect '[Import]' keyword.");
    consume(TokenType::TYPE_CHTL, "Expect '@Chtl' type for import statement.");
    consume(TokenType::KEYWORD_FROM, "Expect 'from' keyword.");

    Token path_token = consume(TokenType::STRING_LITERAL, "Expect file path string literal.");
    std::string path = path_token.lexeme;

    if (!m_context.hasBeenImported(path)) {
        m_context.addImportedPath(path);
        std::string imported_source = Loader::loadFile(path);

        Lexer imported_lexer(imported_source);
        auto imported_tokens = imported_lexer.tokenize();

        Parser imported_parser(imported_tokens, m_context);
        imported_parser.parse();
    }

    auto importNode = std::make_unique<ImportNode>();
    importNode->path = path;

    match(TokenType::SEMICOLON);

    return importNode;
}


std::unique_ptr<StyleTemplateDefinitionNode> Parser::parseTemplateDefinition() {
    consume(TokenType::KEYWORD_TEMPLATE, "Expect '[Template]' keyword.");
    consume(TokenType::TYPE_STYLE, "Expect '@Style' type for template definition.");

    auto defNode = std::make_unique<StyleTemplateDefinitionNode>();
    defNode->name = consume(TokenType::IDENTIFIER, "Expect template name.").lexeme;

    consume(TokenType::LBRACE, "Expect '{' after template name.");

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        CssPropertyNode prop;
        prop.key = consume(TokenType::IDENTIFIER, "Expect CSS property key.").lexeme;
        consume(TokenType::COLON, "Expect ':' after CSS property key.");
        prop.value = parseCssValue();
        defNode->properties.push_back(prop);

        if (!check(TokenType::RBRACE)) {
            consume(TokenType::SEMICOLON, "Expect ';' after CSS property value.");
        }
    }
    consume(TokenType::RBRACE, "Expect '}' after template definition.");

    StyleTemplate st;
    st.name = defNode->name;
    st.properties = defNode->properties;
    m_context.addStyleTemplate(st.name, std::move(st));

    return defNode;
}

std::unique_ptr<BaseNode> Parser::parseStyleBlockItem() {
    if (check(TokenType::TYPE_STYLE)) {
        advance();
        auto usageNode = std::make_unique<StyleTemplateUsageNode>();
        usageNode->name = consume(TokenType::IDENTIFIER, "Expect template name after '@Style'.").lexeme;
        consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
        return usageNode;
    }

    auto propNode = std::make_unique<CssPropertyNode>();
    propNode->key = consume(TokenType::IDENTIFIER, "Expect CSS property key.").lexeme;
    consume(TokenType::COLON, "Expect ':' after CSS property key.");
    propNode->value = parseCssValue();

    if (!check(TokenType::RBRACE)) {
        consume(TokenType::SEMICOLON, "Expect ';' after CSS property value.");
    }
    return propNode;
}

std::unique_ptr<StyleBlockNode> Parser::parseStyleBlock() {
    consume(TokenType::KEYWORD_STYLE, "Expect 'style' keyword.");
    consume(TokenType::LBRACE, "Expect '{' after 'style'.");

    auto styleNode = std::make_unique<StyleBlockNode>();

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        styleNode->items.push_back(parseStyleBlockItem());
    }

    consume(TokenType::RBRACE, "Expect '}' after style block.");
    return styleNode;
}


AttributeNode Parser::parseAttribute() {
    AttributeNode attr;
    attr.key = consume(TokenType::IDENTIFIER, "Expect attribute key.").lexeme;

    if (!match(TokenType::COLON) && !match(TokenType::EQUAL)) {
        throw std::runtime_error("Parse Error: Expect ':' or '=' after attribute key '" + attr.key + "'.");
    }

    if (check(TokenType::STRING_LITERAL) || check(TokenType::IDENTIFIER)) {
        attr.value = advance().lexeme;
    } else {
        throw std::runtime_error("Parse Error: Expected attribute value for key '" + attr.key + "'.");
    }

    match(TokenType::SEMICOLON);
    return attr;
}

std::unique_ptr<TextNode> Parser::parseText() {
    consume(TokenType::KEYWORD_TEXT, "Expect 'text' keyword.");
    consume(TokenType::LBRACE, "Expect '{' after 'text'.");

    auto textNode = std::make_unique<TextNode>();
    if (check(TokenType::STRING_LITERAL)) {
        textNode->content = advance().lexeme;
    }

    consume(TokenType::RBRACE, "Expect '}' after text block content.");
    return textNode;
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = consume(TokenType::IDENTIFIER, "Expect element tag name.").lexeme;

    consume(TokenType::LBRACE, "Expect '{' after element tag name.");

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::IDENTIFIER) && (peekNext().type == TokenType::COLON || peekNext().type == TokenType::EQUAL)) {
            element->attributes.push_back(parseAttribute());
        }
        else {
            auto child = parseStatement();
            if (child) {
                element->children.push_back(std::move(child));
            }
        }
    }

    consume(TokenType::RBRACE, "Expect '}' after element block.");
    return element;
}

} // namespace CHTL

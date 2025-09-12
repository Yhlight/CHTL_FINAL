#include "Parser.h"
#include <iostream>
#include <sstream>
#include "../../Util/StringUtil/StringUtil.h"

namespace CHTL
{
    Parser::Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)), m_position(0), m_hasError(false) {}

    Token& Parser::currentToken() {
        return m_tokens[m_position];
    }

    Token& Parser::peekToken() {
        if (m_position + 1 < m_tokens.size()) {
            return m_tokens[m_position + 1];
        }
        return m_tokens.back(); // Return EOF token
    }

    void Parser::advance() {
        if (m_position < m_tokens.size()) {
            m_position++;
        }
    }

    bool Parser::expect(TokenType type) {
        if (currentToken().type == type) {
            advance();
            return true;
        }
        reportError("Expected " + tokenTypeToString(type) + " but got " + tokenTypeToString(currentToken().type));
        return false;
    }

    void Parser::reportError(const std::string& message) {
        m_hasError = true;
        std::cerr << "Parse Error [line " << currentToken().line << "]: " << message << std::endl;
    }

    std::unique_ptr<ProgramNode> Parser::parse() {
        auto program = std::make_unique<ProgramNode>();
        while (currentToken().type != TokenType::END_OF_FILE) {
            auto statement = parseStatement();
            if (statement) {
                program->children.push_back(std::move(statement));
            } else if (!m_hasError) {
                // If parseStatement returns null and there's no error, it means we've hit a token
                // that doesn't start a statement, which is an error.
                reportError("Unexpected token: " + currentToken().literal);
                advance(); // Move past the unexpected token to avoid an infinite loop
            } else {
                // An error was already reported. Advance to try to recover.
                advance();
            }
        }
        return program;
    }

    std::unique_ptr<BaseNode> Parser::parseStatement() {
        // Handle `key: value;` statements, like `text: "hello";`
        if (currentToken().type == TokenType::IDENTIFIER && peekToken().type == TokenType::COLON) {
            if (currentToken().literal == "text") {
                advance(); // consume 'text'
                advance(); // consume ':'

                if (currentToken().type != TokenType::STRING && currentToken().type != TokenType::IDENTIFIER && currentToken().type != TokenType::NUMBER) {
                     reportError("Expected value for text property.");
                     return nullptr;
                }

                auto textNode = std::make_unique<TextNode>(currentToken().literal);
                advance(); // consume value

                if (currentToken().type == TokenType::SEMICOLON) {
                    advance(); // consume optional semicolon
                }

                return textNode;
            }
            // In the future, this is where we would handle other properties like `color: red;`
        }

        // Handle element declarations (`div {}`) or text blocks (`text {}` or `style {}`)
        if (currentToken().type == TokenType::IDENTIFIER) {
            if (currentToken().literal == "text") {
                return parseText();
            }
            if (currentToken().literal == "style") {
                return parseStyle();
            }
            return parseElement();
        }
        if (currentToken().type == TokenType::SINGLE_LINE_COMMENT) {
            auto node = std::make_unique<CommentNode>(currentToken().literal, ChtlCommentType::Regular);
            advance();
            return node;
        }
        if (currentToken().type == TokenType::MULTI_LINE_COMMENT) {
            auto node = std::make_unique<CommentNode>(currentToken().literal, ChtlCommentType::MultiLine);
            advance();
            return node;
        }
        if (currentToken().type == TokenType::GENERATOR_COMMENT) {
            auto node = std::make_unique<CommentNode>(currentToken().literal, ChtlCommentType::Generator);
            advance();
            return node;
        }
        return nullptr;
    }

    std::unique_ptr<ElementNode> Parser::parseElement() {
        if (currentToken().type != TokenType::IDENTIFIER) {
            return nullptr;
        }

        auto element = std::make_unique<ElementNode>(currentToken().literal);
        advance();

        // Loop to parse attributes
        while (currentToken().type == TokenType::IDENTIFIER && peekToken().type == TokenType::COLON) {
            std::string key = currentToken().literal;
            advance(); // consume key
            advance(); // consume colon

            if (currentToken().type != TokenType::STRING && currentToken().type != TokenType::IDENTIFIER && currentToken().type != TokenType::NUMBER) {
                reportError("Expected attribute value (string, identifier, or number).");
                break;
            }

            element->addAttribute(key, currentToken().literal);
            advance(); // consume value

            if (currentToken().type == TokenType::SEMICOLON) {
                advance(); // consume semicolon
            } else if (currentToken().type != TokenType::LEFT_BRACE) {
                // Semicolon is optional only if it's the last thing before the body
                reportError("Expected semicolon after attribute or opening brace for element body.");
                break;
            }
        }

        if (currentToken().type != TokenType::LEFT_BRACE) {
            // This is a self-closing tag or an element without a body.
            // A semicolon here would be for something like `br;` which we can support later.
            return element;
        }

        expect(TokenType::LEFT_BRACE);

        while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
            auto child = parseStatement();
            if (child) {
                element->addChild(std::move(child));
            } else {
                 // Error or just a comment that was skipped
                if (!m_hasError) {
                    reportError("Invalid statement inside element block.");
                }
                advance();
            }
        }

        expect(TokenType::RIGHT_BRACE);

        return element;
    }

    std::unique_ptr<TextNode> Parser::parseText() {
        expect(TokenType::IDENTIFIER); // consume "text"

        if (!expect(TokenType::LEFT_BRACE)) return nullptr;

        std::string content;
        // Handle a single quoted string for `text { "Hello" }`
        if (currentToken().type == TokenType::STRING) {
            content = currentToken().literal;
            advance();
        } else {
            // Handle unquoted literals like `text { Hello World }`
            size_t startPos = m_position;
            while(currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                advance();
            }
            if (m_position > startPos) {
                 // Join the literals of the tokens to form the content.
                 std::stringstream ss;
                 for(size_t i = startPos; i < m_position; ++i) {
                     ss << m_tokens[i].literal;
                     // Add a space between tokens, except for the last one.
                     if (i < m_position - 1) {
                        ss << " ";
                     }
                 }
                 content = ss.str();
            }
        }

        if (!expect(TokenType::RIGHT_BRACE)) return nullptr;

        // The content might have leading/trailing whitespace from the block formatting.
        // Let's trim it.
        StringUtil::trim(content);

        return std::make_unique<TextNode>(content);
    }

    std::unique_ptr<StyleNode> Parser::parseStyle() {
        expect(TokenType::IDENTIFIER); // consume "style"
        if (!expect(TokenType::LEFT_BRACE)) return nullptr;

        auto styleNode = std::make_unique<StyleNode>();

        while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
            if (currentToken().type == TokenType::DOT || currentToken().type == TokenType::AMPERSAND || currentToken().literal == "#") {
                CssRule rule;
                std::stringstream selector_ss;
                if (currentToken().literal == "#") {
                    selector_ss << "#";
                    advance();
                }
                while (currentToken().type != TokenType::LEFT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                    selector_ss << currentToken().literal;
                    advance();
                }
                rule.selector = selector_ss.str();
                if (!expect(TokenType::LEFT_BRACE)) break;
                while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                    if (currentToken().type != TokenType::IDENTIFIER || peekToken().type != TokenType::COLON) {
                        reportError("Expected a CSS property inside rule block.");
                        advance(); continue;
                    }
                    std::string key = currentToken().literal;
                    advance(); advance();
                    std::vector<Token> value_tokens;
                    while (currentToken().type != TokenType::SEMICOLON && currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                        value_tokens.push_back(currentToken());
                        advance();
                    }
                    try {
                        ExpressionParser exprParser(value_tokens);
                        rule.properties[key] = exprParser.parse();
                    } catch (const std::runtime_error& e) {
                        std::stringstream value_str_ss;
                        for(const auto& t : value_tokens) { value_str_ss << t.literal; }
                        rule.properties[key] = std::make_unique<StringLiteralNode>(value_str_ss.str());
                    }
                    if (currentToken().type == TokenType::SEMICOLON) advance();
                }
                if (!expect(TokenType::RIGHT_BRACE)) break;
                styleNode->addRule(std::move(rule));
            } else if (currentToken().type == TokenType::IDENTIFIER) {
                std::string key = currentToken().literal;
                advance();
                if (!expect(TokenType::COLON)) break;
                std::vector<Token> value_tokens;
                while (currentToken().type != TokenType::SEMICOLON && currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                    value_tokens.push_back(currentToken());
                    advance();
                }
                try {
                    ExpressionParser exprParser(value_tokens);
                    styleNode->addInlineProperty(key, exprParser.parse());
                } catch (const std::runtime_error& e) {
                    std::stringstream value_str_ss;
                    for(const auto& t : value_tokens) { value_str_ss << t.literal; }
                    styleNode->addInlineProperty(key, std::make_unique<StringLiteralNode>(value_str_ss.str()));
                }
                if (currentToken().type == TokenType::SEMICOLON) advance();
            } else {
                reportError("Expected a style property or selector rule.");
                advance();
            }
        }
        if (!expect(TokenType::RIGHT_BRACE)) return nullptr;
        return styleNode;
    }
}

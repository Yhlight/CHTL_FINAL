#include "ValueParser.h"
#include <stdexcept>

ValueParser::ValueParser(const std::vector<ValueToken>& tokens) : tokens(tokens) {}

std::shared_ptr<ExprNode> ValueParser::parse() {
    return parseExpression();
}

int ValueParser::getPrecedence(ValueTokenType type) {
    switch (type) {
        case ValueTokenType::TOKEN_QUESTION: return 1;
        case ValueTokenType::TOKEN_LOGICAL_OR: return 2;
        case ValueTokenType::TOKEN_LOGICAL_AND: return 3;
        case ValueTokenType::TOKEN_EQUAL_EQUAL:
        case ValueTokenType::TOKEN_NOT_EQUAL: return 4;
        case ValueTokenType::TOKEN_GREATER:
        case ValueTokenType::TOKEN_GREATER_EQUAL:
        case ValueTokenType::TOKEN_LESS:
        case ValueTokenType::TOKEN_LESS_EQUAL: return 5;
        case ValueTokenType::TOKEN_PLUS:
        case ValueTokenType::TOKEN_MINUS: return 6;
        case ValueTokenType::TOKEN_STAR:
        case ValueTokenType::TOKEN_SLASH: return 7;
        case ValueTokenType::TOKEN_POWER: return 8;
        default: return 0;
    }
}

std::shared_ptr<ExprNode> ValueParser::parsePrefix() {
    ValueToken token = advance();

    if (token.type == ValueTokenType::TOKEN_STRING) {
        auto node = std::make_shared<StringLiteralNode>();
        node->value = token.text;
        return node;
    }

    if (token.type == ValueTokenType::TOKEN_IDENTIFIER) {
        auto node = std::make_shared<PropertyReferenceNode>();
        if (peek().type == ValueTokenType::TOKEN_DOT) {
            node->selector = token.text;
            advance();
            node->propertyName = advance().text;
        } else {
            node->selector = "";
            node->propertyName = token.text;
        }
        return node;
    }

    if (token.type == ValueTokenType::TOKEN_NUMBER) {
        auto node = std::make_shared<NumericLiteralNode>();
        size_t unit_start = std::string::npos;
        for (size_t i = 0; i < token.text.length(); ++i) {
            if (isalpha(token.text[i]) || token.text[i] == '%') {
                unit_start = i;
                break;
            }
        }
        if (unit_start == std::string::npos) {
            node->value = std::stod(token.text);
            node->unit = "";
        } else {
            node->value = std::stod(token.text.substr(0, unit_start));
            node->unit = token.text.substr(unit_start);
        }
        return node;
    }

    if (token.type == ValueTokenType::TOKEN_LPAREN) {
        std::shared_ptr<ExprNode> expr = parseExpression();
        if (peek().type != ValueTokenType::TOKEN_RPAREN) {
            throw std::runtime_error("Expected ')' after expression.");
        }
        advance();
        return expr;
    }

    throw std::runtime_error("Expected a value, identifier, or '(' for a prefix expression.");
}

std::shared_ptr<ExprNode> ValueParser::parseInfix(std::shared_ptr<ExprNode> left, ValueToken opToken) {
    if (opToken.type == ValueTokenType::TOKEN_QUESTION) {
        auto node = std::make_shared<ConditionalExprNode>();
        node->condition = left;
        node->trueBranch = parseExpression();
        if (peek().type != ValueTokenType::TOKEN_COLON) {
            throw std::runtime_error("Expected ':' in conditional expression.");
        }
        advance();
        node->falseBranch = parseExpression(getPrecedence(opToken.type));
        return node;
    }

    auto node = std::make_shared<BinaryOpNode>();
    node->left = left;
    node->op = opToken;
    node->right = parseExpression(getPrecedence(opToken.type));
    return node;
}

std::shared_ptr<ExprNode> ValueParser::parseExpression(int precedence) {
    std::shared_ptr<ExprNode> left = parsePrefix();

    while (precedence < getPrecedence(peek().type)) {
        ValueToken opToken = advance();
        left = parseInfix(left, opToken);
    }

    return left;
}

ValueToken ValueParser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

ValueToken ValueParser::peek() {
    return tokens[current];
}

bool ValueParser::isAtEnd() {
    return peek().type == ValueTokenType::TOKEN_EOF;
}

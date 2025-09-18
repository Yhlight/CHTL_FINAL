#pragma once

#include "../CHTLNode/Node.hpp"
#include <string>
#include <variant>

namespace CHTL {

enum class ObjectType {
    INTEGER,
    BOOLEAN,
    STRING,
    NULL_OBJ,
};

struct Object {
    ObjectType type;
    std::variant<int, bool, std::string> value;

    std::string toString() const {
        if (type == ObjectType::INTEGER) return std::to_string(std::get<int>(value));
        if (type == ObjectType::BOOLEAN) return std::get<bool>(value) ? "true" : "false";
        if (type == ObjectType::STRING) return std::get<std::string>(value);
        return "";
    }
};


class Evaluator {
public:
    Object evaluate(Node* node);

private:
    Object evaluateBinaryExpression(TokenType op, Object left, Object right);
    Object evaluateConditionalExpression(ConditionalExpressionNode* node);
};

} // namespace CHTL

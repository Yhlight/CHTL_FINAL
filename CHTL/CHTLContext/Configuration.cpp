#include "Configuration.h"
#include "../CHTLNode/ConfigNode.h"
#include "../Expression/ExpressionEvaluator.h"
#include <stdexcept>

namespace CHTL {

// A simple helper to get a string value from an expression.
// This is a simplified evaluator for config values.
static std::string getStringValue(Expr* expr) {
    if (auto* literal = dynamic_cast<LiteralExpr*>(expr)) {
        if (literal->value == 0 && !literal->unit.empty()) {
            return literal->unit;
        }
    }
    throw std::runtime_error("Configuration value must be a string literal.");
}

static bool getBoolValue(Expr* expr) {
    // We don't have a full context here for the evaluator, but for simple
    // literals like 'true' or 'false', we don't need one.
    ExpressionEvaluator temp_eval(nullptr, {}, nullptr);
    EvaluatedValue result = temp_eval.evaluate(expr, nullptr);
    // isTruthy considers any non-zero number to be true.
    return result.value != 0;
}

static int getIntValue(Expr* expr) {
    if (auto* literal = dynamic_cast<LiteralExpr*>(expr)) {
        if (literal->unit.empty()) {
            return static_cast<int>(literal->value);
        }
    }
    throw std::runtime_error("Configuration value must be an integer.");
}


// Maps the string keys from the [Name] block to the internal TokenType enum
static const std::map<std::string, TokenType> name_key_to_token_type = {
    // General Keywords
    {"KEYWORD_STYLE", TokenType::STYLE},
    {"KEYWORD_TEXT", TokenType::TEXT},
    {"KEYWORD_INHERIT", TokenType::INHERIT},
    {"KEYWORD_FROM", TokenType::FROM},
    {"KEYWORD_AS", TokenType::AS},
    {"KEYWORD_DELETE", TokenType::DELETE},
    {"KEYWORD_INSERT", TokenType::INSERT},
    {"KEYWORD_AFTER", TokenType::AFTER},
    {"KEYWORD_BEFORE", TokenType::BEFORE},
    {"KEYWORD_REPLACE", TokenType::REPLACE},
    // Block Keywords
    {"KEYWORD_TEMPLATE", TokenType::TEMPLATE},
    {"KEYWORD_CUSTOM", TokenType::CUSTOM},
    {"KEYWORD_ORIGIN", TokenType::ORIGIN},
    {"KEYWORD_IMPORT", TokenType::IMPORT},
    {"KEYWORD_CONFIGURATION", TokenType::CONFIGURATION},
    {"KEYWORD_NAMESPACE", TokenType::NAMESPACE},
    {"KEYWORD_NAME", TokenType::NAME},
};

void Configuration::update_from_node(const ConfigNode* node) {
    if (!node) return;

    // Handle general settings
    for (const auto& pair : node->settings) {
        const std::string& key = pair.first;
        Expr* expr = pair.second.get();

        if (key == "DEBUG_MODE") {
            this->debug_mode = getBoolValue(expr);
        } else if (key == "INDEX_INITIAL_COUNT") {
            this->index_initial_count = getIntValue(expr);
        } else if (key == "DISABLE_NAME_GROUP") {
             this->disable_name_group = getBoolValue(expr);
        }
    }

    // Handle [Name] block settings if not disabled
    if (this->disable_name_group) return;

    for (const auto& pair : node->name_settings) {
        const std::string& name_key = pair.first;
        const std::vector<std::string>& new_keywords = pair.second;

        if (name_key_to_token_type.count(name_key)) {
            TokenType type_to_update = name_key_to_token_type.at(name_key);

            // Remove all old keywords that map to this token type
            for (auto it = keyword_map.begin(); it != keyword_map.end(); ) {
                if (it->second == type_to_update) {
                    it = keyword_map.erase(it);
                } else {
                    ++it;
                }
            }

            // Add the new keyword(s)
            for (const std::string& new_keyword : new_keywords) {
                keyword_map[new_keyword] = type_to_update;
            }
        }
    }
}

} // namespace CHTL

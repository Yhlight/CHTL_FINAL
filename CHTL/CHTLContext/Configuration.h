#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <map>
#include <vector>
#include <variant>
#include "../CHTLLexer/Token.h" // For TokenType

namespace CHTL {

// Forward declaration
class ConfigNode;

class Configuration {
public:
    // --- Default values ---
    int index_initial_count = 0;
    bool debug_mode = false;
    bool disable_name_group = true;

    // Maps keyword strings to their corresponding token types
    std::map<std::string, TokenType> keyword_map;

    Configuration() {
        // Populate with default keywords
        keyword_map["style"] = TokenType::STYLE;
        keyword_map["text"] = TokenType::TEXT;
        keyword_map["inherit"] = TokenType::INHERIT;
        keyword_map["from"] = TokenType::FROM;
        keyword_map["as"] = TokenType::AS;
        keyword_map["delete"] = TokenType::DELETE;
        keyword_map["insert"] = TokenType::INSERT;
        keyword_map["after"] = TokenType::AFTER;
        keyword_map["before"] = TokenType::BEFORE;
        keyword_map["replace"] = TokenType::REPLACE;
        // Block keywords
        keyword_map["Template"] = TokenType::TEMPLATE;
        keyword_map["Custom"] = TokenType::CUSTOM;
        keyword_map["Origin"] = TokenType::ORIGIN;
        keyword_map["Import"] = TokenType::IMPORT;
        keyword_map["Configuration"] = TokenType::CONFIGURATION;
        keyword_map["Namespace"] = TokenType::NAMESPACE;
        keyword_map["Name"] = TokenType::NAME;
    }

    // Method to update settings from a parsed ConfigNode
    void update_from_node(const ConfigNode* node);
};

} // namespace CHTL

#endif // CONFIGURATION_H

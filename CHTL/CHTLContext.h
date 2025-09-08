#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "CHTLLexer/Token.h"

struct ConfigurationState {
    // General Settings
    int index_initial_count = 0;
    bool debug_mode = false;
    bool disable_name_group = false;
    bool disable_style_auto_add_class = false;
    bool disable_style_auto_add_id = false;
    bool disable_default_namespace = false;

    // Keyword Aliases from [Name] block
    std::map<TokenType, std::vector<std::string>> keyword_map;

    ConfigurationState() {
        // Initialize with default keywords from CHTL.md
        keyword_map[TokenType::KEYWORD_CUSTOM] = {"[Custom]"};
        keyword_map[TokenType::KEYWORD_TEMPLATE] = {"[Template]"};
        keyword_map[TokenType::KEYWORD_ORIGIN] = {"[Origin]"};
        keyword_map[TokenType::KEYWORD_IMPORT] = {"[Import]"};
        keyword_map[TokenType::KEYWORD_NAMESPACE] = {"[Namespace]"};
        keyword_map[TokenType::KEYWORD_CONFIGURATION] = {"[Configuration]"};
        keyword_map[TokenType::TEXT] = {"text"};
        keyword_map[TokenType::STYLE] = {"style"};
        keyword_map[TokenType::KEYWORD_INHERIT] = {"inherit"};
        keyword_map[TokenType::KEYWORD_DELETE] = {"delete"};
        keyword_map[TokenType::KEYWORD_INSERT] = {"insert"};
        keyword_map[TokenType::KEYWORD_FROM] = {"from"};
        keyword_map[TokenType::KEYWORD_AS] = {"as"};
        keyword_map[TokenType::KEYWORD_EXCEPT] = {"except"};
        keyword_map[TokenType::KEYWORD_SCRIPT] = {"script"};
    }
};

// A shared context for the compiler, holding state like configuration.
class CHTLContext {
public:
    std::shared_ptr<ConfigurationState> config;

    CHTLContext() {
        config = std::make_shared<ConfigurationState>();
    }
};

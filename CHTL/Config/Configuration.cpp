#include "Configuration.h"
#include <sstream>

namespace CHTL {

Configuration::Configuration() {
    // Set default values for all configuration settings based on CHTL.md
    index_initial_count = 0;
    debug_mode = false;
    disable_name_group = true; // Per spec, this is disabled by default
    disable_style_auto_add_class = false;
    disable_style_auto_add_id = false;
    disable_default_namespace = false;
    disable_custom_origin_type = false;

    setDefaultKeywords();
}

void Configuration::setDefaultKeywords() {
    // This map uses internal, standardized names for keys.
    // The parser will look up these keys to find the user-defined lexemes.
    keyword_map["KEYWORD_CUSTOM"] = {"[Custom]"};
    keyword_map["KEYWORD_TEMPLATE"] = {"[Template]"};
    keyword_map["KEYWORD_ORIGIN"] = {"[Origin]"};
    keyword_map["KEYWORD_IMPORT"] = {"[Import]"};
    keyword_map["KEYWORD_NAMESPACE"] = {"[Namespace]"};
    keyword_map["KEYWORD_CONFIG"] = {"[Configuration]"};

    keyword_map["CUSTOM_STYLE"] = {"@Style", "@style", "@CSS", "@Css", "@css"};
    keyword_map["CUSTOM_ELEMENT"] = {"@Element"};
    keyword_map["CUSTOM_VAR"] = {"@Var"};

    keyword_map["TEMPLATE_STYLE"] = {"@Style"};
    keyword_map["TEMPLATE_ELEMENT"] = {"@Element"};
    keyword_map["TEMPLATE_VAR"] = {"@Var"};

    keyword_map["ORIGIN_HTML"] = {"@Html"};
    keyword_map["ORIGIN_STYLE"] = {"@Style"};
    keyword_map["ORIGIN_JAVASCRIPT"] = {"@JavaScript"};

    keyword_map["IMPORT_HTML"] = {"@Html"};
    keyword_map["IMPORT_STYLE"] = {"@Style"};
    keyword_map["IMPORT_JAVASCRIPT"] = {"@JavaScript"};
    keyword_map["IMPORT_CHTL"] = {"@Chtl"};
    keyword_map["IMPORT_CRMOD"] = {"@CJmod"};
    keyword_map["IMPORT_CONFIG"] = {"@Config"};

    keyword_map["KEYWORD_INHERIT"] = {"inherit"};
    keyword_map["KEYWORD_DELETE"] = {"delete"};
    keyword_map["KEYWORD_INSERT"] = {"insert"};
    keyword_map["KEYWORD_AFTER"] = {"after"};
    keyword_map["KEYWORD_BEFORE"] = {"before"};
    keyword_map["KEYWORD_REPLACE"] = {"replace"};
    keyword_map["KEYWORD_ATTOP"] = {"at top"};
    keyword_map["KEYWORD_ATBOTTOM"] = {"at bottom"};
    keyword_map["KEYWORD_FROM"] = {"from"};
    keyword_map["KEYWORD_AS"] = {"as"};
    keyword_map["KEYWORD_EXCEPT"] = {"except"};
    keyword_map["KEYWORD_USE"] = {"use"};
    keyword_map["KEYWORD_HTML5"] = {"html5"};

    keyword_map["KEYWORD_TEXT"] = {"text"};
    keyword_map["KEYWORD_STYLE"] = {"style"};
    keyword_map["KEYWORD_SCRIPT"] = {"script"};
}


// A simple utility to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    const auto strBegin = str.find_first_not_of(" \t\n\r");
    if (strBegin == std::string::npos) return "";
    const auto strEnd = str.find_last_not_of(" \t\n\r");
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}

void Configuration::parseFromString(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    bool in_name_block = false;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '/' || line[0] == '#') {
            continue; // Skip empty lines and comments
        }

        if (line == "[Name]") {
            in_name_block = true;
            continue;
        }
        if (line == "]") { // A bit simplistic, assumes it's closing the name block
            in_name_block = false;
            continue;
        }

        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, equals_pos));
        std::string value = trim(line.substr(equals_pos + 1));
        if (value.back() == ';') {
            value.pop_back();
            value = trim(value);
        }

        if (in_name_block) {
            if (!key.empty() && !value.empty()) {
                // For now, only handle single values, not group options like `[@Style, @style]`
                keyword_map[key] = { value };
            }
        } else {
            // Handle top-level settings
            if (key == "INDEX_INITIAL_COUNT") {
                try {
                    index_initial_count = std::stoi(value);
                } catch (...) { /* ignore invalid values */ }
            } else if (key == "DEBUG_MODE") {
                debug_mode = (value == "true");
            } else if (key == "DISABLE_NAME_GROUP") {
                disable_name_group = (value == "true");
            }
            // ... add other top-level config settings here
        }
    }
}


} // namespace CHTL

#pragma once

#include <string>
#include <vector>

enum class ChtlJsTokenType {
    L_DOUBLE_BRACE, // {{
    R_DOUBLE_BRACE, // }}
    ARROW,          // ->
    IDENTIFIER,
    LBRACE,
    RBRACE,
    // For simplicity, everything else is just "other" stuff for now
    // A more advanced lexer would tokenize JS more granularly.
    OTHER_CONTENT,
    END_OF_FILE,
    UNKNOWN
};

struct ChtlJsToken {
    ChtlJsTokenType type;
    std::string value;
};

// Helper function for debugging
inline std::string cjsTokenTypeToString(ChtlJsTokenType type) {
    switch (type) {
        case ChtlJsTokenType::L_DOUBLE_BRACE: return "L_DOUBLE_BRACE";
        case ChtlJsTokenType::R_DOUBLE_BRACE: return "R_DOUBLE_BRACE";
        case ChtlJsTokenType::ARROW: return "ARROW";
        case ChtlJsTokenType::IDENTIFIER: return "IDENTIFIER";
        case ChtlJsTokenType::LBRACE: return "LBRACE";
        case ChtlJsTokenType::RBRACE: return "RBRACE";
        case ChtlJsTokenType::OTHER_CONTENT: return "OTHER_CONTENT";
        case ChtlJsTokenType::END_OF_FILE: return "END_OF_FILE";
        case ChtlJsTokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN_TYPE";
    }
}

#include "GlobalMap.h"

namespace CHTL {

GlobalMap::GlobalMap() {
    // Standard keywords
    keywordMap["text"] = TokenType::KEYWORD_TEXT;
    keywordMap["style"] = TokenType::KEYWORD_STYLE;
    keywordMap["script"] = TokenType::KEYWORD_SCRIPT;
    keywordMap["inherit"] = TokenType::KEYWORD_INHERIT;
    keywordMap["delete"] = TokenType::KEYWORD_DELETE;
    keywordMap["insert"] = TokenType::KEYWORD_INSERT;
    keywordMap["after"] = TokenType::KEYWORD_AFTER;
    keywordMap["before"] = TokenType::KEYWORD_BEFORE;
    keywordMap["replace"] = TokenType::KEYWORD_REPLACE;
    keywordMap["at top"] = TokenType::KEYWORD_ATTOP;
    keywordMap["at bottom"] = TokenType::KEYWORD_ATBOTTOM;
    keywordMap["from"] = TokenType::KEYWORD_FROM;
    keywordMap["as"] = TokenType::KEYWORD_AS;
    keywordMap["except"] = TokenType::KEYWORD_EXCEPT;
    keywordMap["use"] = TokenType::KEYWORD_USE;
    keywordMap["html5"] = TokenType::KEYWORD_HTML5;

    // Block keywords
    keywordMap["[Custom]"] = TokenType::KEYWORD_CUSTOM;
    keywordMap["[Template]"] = TokenType::KEYWORD_TEMPLATE;
    keywordMap["[Origin]"] = TokenType::KEYWORD_ORIGIN;
    keywordMap["[Import]"] = TokenType::KEYWORD_IMPORT;
    keywordMap["[Namespace]"] = TokenType::KEYWORD_NAMESPACE;
    keywordMap["[Configuration]"] = TokenType::KEYWORD_CONFIGURATION;
    keywordMap["[Info]"] = TokenType::KEYWORD_INFO;
    keywordMap["[Export]"] = TokenType::KEYWORD_EXPORT;
    keywordMap["[OriginType]"] = TokenType::KEYWORD_ORIGIN_TYPE;
    keywordMap["[Name]"] = TokenType::KEYWORD_NAME;

    // Type keywords
    keywordMap["@Style"] = TokenType::KEYWORD_AT_STYLE;
    keywordMap["@Element"] = TokenType::KEYWORD_AT_ELEMENT;
    keywordMap["@Var"] = TokenType::KEYWORD_AT_VAR;
    keywordMap["@Html"] = TokenType::KEYWORD_AT_HTML;
    keywordMap["@JavaScript"] = TokenType::KEYWORD_AT_JAVASCRIPT;
    keywordMap["@Chtl"] = TokenType::KEYWORD_AT_CHTL;
    keywordMap["@CJmod"] = TokenType::KEYWORD_AT_CJMOD;
    keywordMap["@Config"] = TokenType::KEYWORD_AT_CONFIG;
}

TokenType GlobalMap::getKeywordType(const std::string& keyword) const {
    auto it = keywordMap.find(keyword);
    if (it != keywordMap.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER; // Default to IDENTIFIER if not found
}

} // namespace CHTL

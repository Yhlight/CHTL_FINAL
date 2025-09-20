#include "UnifiedScanner.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cctype>

const std::string UnifiedScanner::CHTL_PLACEHOLDER = "_CHTL_CODE_PLACEHOLDER_";
const std::string UnifiedScanner::CHTLJS_PLACEHOLDER = "_CHTLJS_CODE_PLACEHOLDER_";
const std::string UnifiedScanner::JS_PLACEHOLDER = "_JS_CODE_PLACEHOLDER_";

size_t UnifiedScanner::findMatchingBrace(const std::string& str, size_t start_pos) {
    if (start_pos >= str.length() || str[start_pos] != '{') {
        return std::string::npos;
    }
    size_t brace_count = 1;
    for (size_t i = start_pos + 1; i < str.length(); ++i) {
        if (str[i] == '{') {
            brace_count++;
        } else if (str[i] == '}') {
            brace_count--;
            if (brace_count == 0) {
                return i;
            }
        }
    }
    return std::string::npos; // Not found
}

std::string UnifiedScanner::processStyleBlock(const std::string& blockContent) {
    // This is a simplified implementation. A full implementation would require
    // a more sophisticated parser to handle all the CHTL features allowed
    // in style blocks (e.g., property expressions, @-rules, delete).
    // For now, we placeholder [Origin] blocks, which are structurally distinct,
    // and treat the rest as a single block.
    if (blockContent.find("[Origin]") == std::string::npos) {
        if (blockContent.find_first_not_of(" \t\r\n") == std::string::npos) {
            return blockContent;
        }
        return generatePlaceholder(blockContent, CHTL_PLACEHOLDER);
    }

    std::string processedContent = "";
    size_t pos = 0;
    while(pos < blockContent.length()) {
        size_t originPos = blockContent.find("[Origin]", pos);
        if (originPos != std::string::npos) {
            processedContent += generatePlaceholder(blockContent.substr(pos, originPos - pos), CHTL_PLACEHOLDER);
            size_t openingBrace = blockContent.find('{', originPos);
            if(openingBrace != std::string::npos) {
                size_t closingBrace = findMatchingBrace(blockContent, openingBrace);
                if (closingBrace != std::string::npos) {
                    std::string originBlock = blockContent.substr(originPos, closingBrace - originPos + 1);
                    processedContent += generatePlaceholder(originBlock, CHTL_PLACEHOLDER);
                    pos = closingBrace + 1;
                    continue;
                }
            }
        }
        // If no more CHTL constructs are found, append the rest.
        processedContent += generatePlaceholder(blockContent.substr(pos), CHTL_PLACEHOLDER);
        break;
    }

    return processedContent;
}

std::string UnifiedScanner::processScriptBlock(const std::string& blockContent) {
    size_t start = blockContent.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) { // all whitespace
        return blockContent;
    }
    std::string leading_ws = blockContent.substr(0, start);

    size_t end = blockContent.find_last_not_of(" \t\r\n");
    std::string trailing_ws = blockContent.substr(end + 1);

    std::string core_content = blockContent.substr(start, end - start + 1);

    std::string processed_core = processScriptBlock_recursive(core_content);

    return leading_ws + processed_core + trailing_ws;
}

std::string UnifiedScanner::processScriptBlock_recursive(const std::string& blockContent) {
    std::string result = "";
    size_t pos = 0;

    const std::vector<std::string> chtljs_keywords = {"Listen", "Animate", "Router", "Delegate", "iNeverAway"};

    while (pos < blockContent.length()) {
        size_t next_pos = std::string::npos;
        std::string found_keyword = "";
        int construct_type = -1;

        size_t selector_pos = blockContent.find("{{", pos);
        if (selector_pos != std::string::npos) {
            next_pos = selector_pos;
            construct_type = 0;
        }

        for (const auto& keyword : chtljs_keywords) {
            size_t keyword_pos = blockContent.find(keyword, pos);
            if (keyword_pos != std::string::npos && (next_pos == std::string::npos || keyword_pos < next_pos)) {
                 if ((keyword_pos + keyword.length()) < blockContent.length()) {
                    char char_after = blockContent[keyword_pos + keyword.length()];
                    if (isspace(char_after) || char_after == '{') {
                        next_pos = keyword_pos;
                        found_keyword = keyword;
                        construct_type = 1;
                    }
                }
            }
        }

        if (next_pos == std::string::npos) {
            std::string remaining_js = blockContent.substr(pos);
            if (!remaining_js.empty()) {
                result += generatePlaceholder(remaining_js, JS_PLACEHOLDER);
            }
            break;
        }

        std::string js_part = blockContent.substr(pos, next_pos - pos);
        if (!js_part.empty()) {
            result += generatePlaceholder(js_part, JS_PLACEHOLDER);
        }

        if (construct_type == 0) {
            size_t end_selector_pos = blockContent.find("}}", next_pos);
            if (end_selector_pos != std::string::npos) {
                result += blockContent.substr(next_pos, end_selector_pos - next_pos + 2);
                pos = end_selector_pos + 2;
            } else {
                pos = next_pos + 2;
            }
        } else if (construct_type == 1) {
            result += found_keyword;
            size_t open_brace = blockContent.find("{", next_pos);
            if (open_brace != std::string::npos) {
                 result += blockContent.substr(next_pos + found_keyword.length(), open_brace - (next_pos + found_keyword.length()));
                 result += "{";
                size_t close_brace = findMatchingBrace(blockContent, open_brace);
                if (close_brace != std::string::npos) {
                    std::string inner_content = blockContent.substr(open_brace + 1, close_brace - open_brace - 1);
                    result += processScriptBlock(inner_content);
                    result += "}";
                    pos = close_brace + 1;
                } else {
                    pos = open_brace + 1;
                }
            } else {
                 result += " ";
                 pos = next_pos + found_keyword.length();
            }
        }
    }
    return result;
}


std::string UnifiedScanner::scan(const std::string& source) {
    std::string result = "";
    size_t currentPos = 0;

    while (currentPos < source.length()) {
        size_t stylePos = source.find("style", currentPos);
        size_t scriptPos = source.find("script", currentPos);

        size_t nextBlockKeywordPos = std::string::npos;
        std::string blockKeyword = "";
        bool isStyle = false;

        if (stylePos != std::string::npos && (scriptPos == std::string::npos || stylePos < scriptPos)) {
            nextBlockKeywordPos = stylePos;
            blockKeyword = "style";
            isStyle = true;
        } else if (scriptPos != std::string::npos) {
            nextBlockKeywordPos = scriptPos;
            blockKeyword = "script";
            isStyle = false;
        }

        if (nextBlockKeywordPos == std::string::npos) {
            result += source.substr(currentPos);
            break;
        }

        size_t keywordEndPos = nextBlockKeywordPos + blockKeyword.length();
        size_t openingBrace = source.find('{', keywordEndPos);

        if (openingBrace == std::string::npos) {
            result += source.substr(currentPos);
            break;
        }

        bool isBlock = true;
        for (size_t i = keywordEndPos; i < openingBrace; ++i) {
            if (!isspace(source[i])) {
                isBlock = false;
                break;
            }
        }

        if (!isBlock) {
            result += source.substr(currentPos, keywordEndPos - currentPos);
            currentPos = keywordEndPos;
            continue;
        }

        result += source.substr(currentPos, openingBrace + 1 - currentPos);

        size_t closingBrace = findMatchingBrace(source, openingBrace);
        if (closingBrace == std::string::npos) {
            result += source.substr(openingBrace + 1);
            break;
        }

        std::string blockContent = source.substr(openingBrace + 1, closingBrace - openingBrace - 1);
        std::string processedContent;

        if (isStyle) {
            processedContent = processStyleBlock(blockContent);
        } else {
            processedContent = processScriptBlock(blockContent);
        }

        result += processedContent;
        result += "}";
        currentPos = closingBrace + 1;
    }

    return result;
}

std::string UnifiedScanner::generatePlaceholder(const std::string& code, const std::string& type) {
    std::string placeholder = type + std::to_string(placeholderCounter++) + "_";
    placeholderMap[placeholder] = code;
    return placeholder;
}

std::string UnifiedScanner::getOriginalCode(const std::string& placeholder) const {
    auto it = placeholderMap.find(placeholder);
    if (it != placeholderMap.end()) {
        return it->second;
    }
    return ""; // Or throw an exception
}

const std::map<std::string, std::string>& UnifiedScanner::getPlaceholderMap() const {
    return placeholderMap;
}

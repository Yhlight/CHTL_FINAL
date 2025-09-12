#include "LocalStyleParser.h"
#include "../../Util/StringUtil/StringUtil.h"
#include "../CssValueParser/ValueTokenizer.h"
#include "../CssValueParser/ValueParser.h"
#include <algorithm>
#include <iostream>

// Pre-processes the style block content to remove comments.
static std::string removeComments(const std::string& input) {
    std::string output;
    output.reserve(input.length());
    for (size_t i = 0; i < input.length(); ++i) {
        if (i + 1 < input.length()) {
            if (input[i] == '/' && input[i+1] == '/') {
                size_t end_of_line = input.find('\n', i);
                if (end_of_line == std::string::npos) break;
                i = end_of_line -1;
                continue;
            }
            if (input[i] == '/' && input[i+1] == '*') {
                size_t end_of_comment = input.find("*/", i + 2);
                if (end_of_comment == std::string::npos) break;
                i = end_of_comment + 1;
                continue;
            }
        }
        output += input[i];
    }
    return output;
}

// The computeValue helper is no longer needed here, it will be part of the new StyleEvaluator pass.

ParsedStyleBlock LocalStyleParser::parse(const std::string& rawContent) {
    ParsedStyleBlock result;
    std::string content = removeComments(rawContent);
    size_t pos = 0;

    while (pos < content.length()) {
        size_t start_of_meaningful_content = content.find_first_not_of(" \t\n\r", pos);
        if (start_of_meaningful_content == std::string::npos) {
            break;
        }
        pos = start_of_meaningful_content;

        // Check for template usage: @Style ... ;
        if (content[pos] == '@') {
            size_t semi_pos = content.find(';', pos);
            if (semi_pos != std::string::npos) {
                // We expect "@Style TemplateName"
                std::string usage_str = trim(std::string_view(content).substr(pos, semi_pos - pos));
                size_t space_pos = usage_str.find(' ');
                if (space_pos != std::string::npos) {
                    std::string template_name = trim(usage_str.substr(space_pos + 1));
                    result.templateUsages.push_back(template_name);
                }
                pos = semi_pos + 1;
                continue;
            }
        }

        size_t semi_pos = content.find(';', pos);
        size_t brace_pos = content.find('{', pos);

        if (brace_pos != std::string::npos && (semi_pos == std::string::npos || brace_pos < semi_pos)) {
            // --- This is a global rule ---
            std::string selector = trim(std::string_view(content).substr(pos, brace_pos - pos));
            int brace_count = 1;
            size_t content_end_pos = brace_pos + 1;
            while (content_end_pos < content.length() && brace_count > 0) {
                if (content[content_end_pos] == '{') brace_count++;
                else if (content[content_end_pos] == '}') brace_count--;
                content_end_pos++;
            }

            std::string raw_rule_content;
            if (brace_count == 0) {
                 raw_rule_content = trim(std::string_view(content).substr(brace_pos + 1, content_end_pos - brace_pos - 2));
            }

            if (!selector.empty() && !raw_rule_content.empty()) {
                // For now, we pass the raw content. Evaluation will happen later.
                result.globalRules.push_back({selector, raw_rule_content});
            }
            pos = content_end_pos;

        } else if (semi_pos != std::string::npos) {
            // --- This is an inline property ---
            std::string declaration = content.substr(pos, semi_pos - pos);
            size_t colon_pos = declaration.find(':');

            if (colon_pos != std::string::npos) {
                std::string key = trim(std::string_view(declaration).substr(0, colon_pos));
                std::string raw_value = trim(std::string_view(declaration).substr(colon_pos + 1));

                if (!key.empty() && !raw_value.empty()) {
                    // Tokenize and parse the value into an expression tree, but do not evaluate it.
                    try {
                        ValueTokenizer tokenizer(raw_value);
                        std::vector<ValueToken> tokens = tokenizer.tokenize();
                        ValueParser valueParser(tokens);
                        result.inlineStyleTrees[key] = valueParser.parse();
                    } catch (const std::runtime_error& e) {
                        std::cerr << "Warning: Could not parse style expression '" << raw_value << "'. Error: " << e.what() << std::endl;
                        // Store a null ptr to signify a parse failure.
                        result.inlineStyleTrees[key] = nullptr;
                    }
                }
            }
            pos = semi_pos + 1;
        } else {
            break;
        }
    }

    return result;
}

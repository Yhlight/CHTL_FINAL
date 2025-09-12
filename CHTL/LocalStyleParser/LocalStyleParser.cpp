#include "LocalStyleParser.h"
#include "../../Util/StringUtil/StringUtil.h"
#include "../CssValueParser/ValueTokenizer.h"
#include "../CssValueParser/ValueParser.h"
#include "../CssValueParser/Evaluator.h"
#include <algorithm>
#include <iostream>
#include <sstream>

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

// Takes a raw value string (e.g., "100px + 50px") and returns the computed value string (e.g., "150px")
static std::string computeValue(const std::string& rawValue) {
    // Updated to not check for '%' as an operator
    if (rawValue.find_first_of("+-*/()") == std::string::npos) {
        return rawValue;
    }
    try {
        ValueTokenizer tokenizer(rawValue);
        std::vector<ValueToken> tokens = tokenizer.tokenize();

        ValueParser parser(tokens);
        std::shared_ptr<ExprNode> expr = parser.parse();

        Evaluator evaluator;
        EvaluatedValue evaluated = evaluator.evaluate(expr);

        if (evaluated.hasError) {
            std::cerr << "Warning: Could not evaluate style expression '" << rawValue << "'. Error: " << evaluated.errorMessage << std::endl;
            return rawValue;
        }

        std::stringstream ss;
        ss << evaluated.value << evaluated.unit;
        return ss.str();

    } catch (const std::runtime_error& e) {
        std::cerr << "Warning: Could not parse style expression '" << rawValue << "'. Error: " << e.what() << std::endl;
        return rawValue;
    }
}

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
                // Manually parse the properties inside the rule's content
                std::string computed_rule_content;
                size_t inner_pos = 0;
                while (inner_pos < raw_rule_content.length()) {
                    size_t next_semi = raw_rule_content.find(';', inner_pos);
                    if (next_semi == std::string::npos) next_semi = raw_rule_content.length();

                    std::string decl = raw_rule_content.substr(inner_pos, next_semi - inner_pos);
                    size_t next_colon = decl.find(':');

                    if (next_colon != std::string::npos) {
                        std::string key = trim(std::string_view(decl).substr(0, next_colon));
                        std::string val = trim(std::string_view(decl).substr(next_colon + 1));
                        if (!key.empty() && !val.empty()) {
                            computed_rule_content += key + ": " + computeValue(val) + "; ";
                        }
                    }
                    inner_pos = next_semi + 1;
                }
                result.globalRules.push_back({selector, computed_rule_content});
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
                    result.inlineStyles[key] = computeValue(raw_value);
                }
            }
            pos = semi_pos + 1;
        } else {
            break;
        }
    }

    return result;
}

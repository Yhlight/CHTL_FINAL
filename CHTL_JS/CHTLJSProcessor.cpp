#include "CHTLJSProcessor.h"
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <sstream>

namespace CHTL {

CHTLJSProcessor::CHTLJSProcessor(const std::string& source) : source(source) {}

std::string CHTLJSProcessor::process() {
    while (!isAtEnd()) {
        if (match("{{")) {
            parseEnhancedSelector();
        } else if (isalnum(peek()[0]) || peek() == "_") {
            parseIdentifier();
        }
        else {
            output_buffer += peek();
            advance();
        }
    }
    return output_buffer;
}

void CHTLJSProcessor::parseIdentifier() {
    size_t start = current_pos;
    while (!isAtEnd() && (isalnum(peek()[0]) || peek() == "_")) {
        advance();
    }
    std::string identifier = source.substr(start, current_pos - start);

    size_t next_char_pos = source.find_first_not_of(" \t\n\r", current_pos);

    if (next_char_pos != std::string::npos && source.substr(next_char_pos, 8) == "->Listen") {
        current_pos = next_char_pos + 8;
        parseListenBlock(identifier);
    } else if (next_char_pos != std::string::npos && source.substr(next_char_pos, 3) == "&->") {
        current_pos = next_char_pos + 3;
        parseEventBindingOperator(identifier);
    } else {
        output_buffer += identifier;
    }
}

void CHTLJSProcessor::parseEnhancedSelector() {
    size_t end_pos = source.find("}}", current_pos);
    if (end_pos == std::string::npos) {
        throw std::runtime_error("Unmatched '{{' found in script block.");
    }
    std::string selector = source.substr(current_pos, end_pos - current_pos);
    output_buffer += transformSelector(selector);
    advance(end_pos - current_pos + 2); // Advance past the content and the "}}"
}

void CHTLJSProcessor::parseListenBlock(const std::string& element_var) {
    while(isspace(peek()[0])) advance();
    if (!match("{")) throw std::runtime_error("Expected '{' after ->Listen.");

    std::string block_content = extractBlockContent('{', '}');

    size_t pos = 0;
    while (pos < block_content.length()) {
        size_t colon_pos = block_content.find(':', pos);
        if (colon_pos == std::string::npos) break;
        std::string event_name = block_content.substr(pos, colon_pos - pos);
        trim(event_name);

        size_t comma_pos = block_content.find(',', colon_pos);
        size_t end_callback_pos = (comma_pos == std::string::npos) ? block_content.length() : comma_pos;
        std::string callback = block_content.substr(colon_pos + 1, end_callback_pos - (colon_pos + 1));
        trim(callback);

        output_buffer += element_var + ".addEventListener('" + event_name + "', " + callback + ");";
        if (comma_pos == std::string::npos) break;
        pos = comma_pos + 1;
    }
}

void CHTLJSProcessor::parseEventBindingOperator(const std::string& element_var) {
    while(isspace(peek()[0])) advance();
    if (match("{")) {
        // Block form: &-> { ... }
        std::string block_content = extractBlockContent('{', '}');
        // Re-use the listen block logic
        size_t pos = 0;
        while (pos < block_content.length()) {
            size_t colon_pos = block_content.find(':', pos);
            if (colon_pos == std::string::npos) break;
            std::string event_name = block_content.substr(pos, colon_pos - pos);
            trim(event_name);

            size_t comma_pos = block_content.find(',', colon_pos);
            size_t end_callback_pos = (comma_pos == std::string::npos) ? block_content.length() : comma_pos;
            std::string callback = block_content.substr(colon_pos + 1, end_callback_pos - (colon_pos + 1));
            trim(callback);

            output_buffer += element_var + ".addEventListener('" + event_name + "', " + callback + ");";
            if (comma_pos == std::string::npos) break;
            pos = comma_pos + 1;
        }
    } else {
        // Inline form: &-> event1, event2: callback;
        size_t colon_pos = source.find(':', current_pos);
        if (colon_pos == std::string::npos) throw std::runtime_error("Invalid event binding syntax: missing ':'.");

        std::string events_str = source.substr(current_pos, colon_pos - current_pos);

        size_t semicolon_pos = source.find(';', colon_pos);
        if (semicolon_pos == std::string::npos) throw std::runtime_error("Event binding must end with a semicolon.");

        std::string callback = source.substr(colon_pos + 1, semicolon_pos - (colon_pos + 1));
        trim(callback);

        std::stringstream ss(events_str);
        std::string event_name;
        while(std::getline(ss, event_name, ',')) {
            trim(event_name);
            if (!event_name.empty()) {
                output_buffer += element_var + ".addEventListener('" + event_name + "', " + callback + ");";
            }
        }
        current_pos = semicolon_pos + 1;
    }
}

std::string CHTLJSProcessor::transformSelector(const std::string& selector_str) {
    std::string selector = selector_str;
    trim(selector);
    size_t bracket_pos = selector.find('[');
    if (bracket_pos != std::string::npos) {
        size_t end_bracket_pos = selector.find(']');
        if (end_bracket_pos != std::string::npos) {
            std::string index_str = selector.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1);
            std::string base_selector = selector.substr(0, bracket_pos);
            trim(base_selector);
            return "document.querySelectorAll('" + base_selector + "')[" + index_str + "]";
        }
    }
    if (selector.find(' ') != std::string::npos || selector[0] == '.' || (isalpha(selector[0]) && selector.find_first_of(".#[]") == std::string::npos)) {
        return "document.querySelector('" + selector + "')";
    }
    if (selector[0] == '#') {
        return "document.getElementById('" + selector.substr(1) + "')";
    }
    return "document.querySelector('" + selector + "')";
}

// --- Helper Implementations ---

void CHTLJSProcessor::trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}


std::string CHTLJSProcessor::extractBlockContent(char open, char close) {
    std::string content;
    int level = 1;
    while (!isAtEnd() && level > 0) {
        if (peek()[0] == open) level++;
        else if (peek()[0] == close) level--;

        if (level > 0) {
            content += peek();
            advance();
        }
    }
    if (level != 0) throw std::runtime_error("Mismatched delimiters in CHTL JS block.");
    advance();
    return content;
}

std::string CHTLJSProcessor::peek(int n) {
    if (current_pos + n >= source.length()) return "\0";
    return source.substr(current_pos, n + 1);
}

bool CHTLJSProcessor::match(const std::string& expected) {
    if (source.substr(current_pos, expected.length()) == expected) {
        advance(expected.length());
        return true;
    }
    return false;
}

void CHTLJSProcessor::advance(int n) {
    current_pos += n;
}

bool CHTLJSProcessor::isAtEnd() {
    return current_pos >= source.length();
}

} // namespace CHTL

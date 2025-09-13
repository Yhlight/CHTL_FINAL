#include "CHTLJSParser.h"
#include "../CHTLJSNode/SelectorNode.h"
#include "../CHTLJSNode/JSCodeNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include <iostream>

namespace CHTL {
namespace CHTL_JS {

CHTLJSParser::CHTLJSParser(std::vector<JSToken>& tokens) : tokens_(tokens) {}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parsePrimary() {
    // This function parses one complete CHTL JS expression or a block of JS code.

    if (tokens_[current_].type == JSTokenType::JSCode) {
        auto node = std::make_unique<JSCodeNode>(tokens_[current_].text);
        current_++;
        return node;
    }

    // It's a CHTL JS expression. It must start with an Identifier or Selector.
    std::unique_ptr<SelectorNode> target;
    if (tokens_[current_].type == JSTokenType::Selector) {
        target = std::make_unique<SelectorNode>(tokens_[current_].text);
        current_++;
    } else if (tokens_[current_].type == JSTokenType::Identifier) {
        // We create a "fake" selector node for identifiers to unify the AST.
        target = std::make_unique<SelectorNode>(tokens_[current_].text);
        current_++;
    } else {
        return nullptr; // Not a valid start
    }

    // Check for -> listen
    if (current_ < tokens_.size() && tokens_[current_].type == JSTokenType::Arrow &&
        current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == JSTokenType::Listen)
    {
        current_ += 2; // consume -> and listen
        auto listenNode = std::make_unique<ListenNode>(std::move(target));

        if (current_ < tokens_.size() && tokens_[current_].type == JSTokenType::OpenBrace) {
            current_++; // consume {
            // For now, just consume the raw string of handlers
            // This is a simplification; a real parser would build an AST for the handlers
            size_t start_token_idx = current_;
            int brace_level = 1;
            while (brace_level > 0 && current_ < tokens_.size()) {
                if (tokens_[current_].type == JSTokenType::OpenBrace) brace_level++;
                else if (tokens_[current_].type == JSTokenType::CloseBrace) brace_level--;

                if (brace_level == 0) break;
                current_++;
            }

            std::string raw_handlers;
            for(size_t i = start_token_idx; i < current_; ++i) {
                raw_handlers += tokens_[i].text;
            }
            listenNode->event_handlers_raw_string_ = raw_handlers;

            if(current_ < tokens_.size()) current_++; // consume final }
        }
        return listenNode;
    }

    // If it wasn't a listen expression, just return the selector/identifier itself.
    // The generator will have to handle this case (e.g. just printing the identifier name).
    return target;
}


std::vector<std::unique_ptr<CHTLJSNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJSNode>> nodes;
    while (current_ < tokens_.size() && tokens_[current_].type != JSTokenType::EndOfFile) {
        auto node = parsePrimary();
        if (node) {
            nodes.push_back(std::move(node));
        } else {
            // Skip unknown tokens for now
            current_++;
        }
    }
    return nodes;
}

} // namespace CHTL_JS
} // namespace CHTL

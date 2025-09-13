#include "CHTLJSParser.h"
#include "../CHTLJSNode/SelectorNode.h"
#include "../CHTLJSNode/JSCodeNode.h"

namespace CHTL {
namespace CHTL_JS {

CHTLJSParser::CHTLJSParser(std::vector<JSToken>& tokens) : tokens_(tokens) {}

std::vector<std::unique_ptr<CHTLJSNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJSNode>> nodes;
    while (current_ < tokens_.size() && tokens_[current_].type != JSTokenType::EndOfFile) {
        const auto& token = tokens_[current_];
        if (token.type == JSTokenType::Selector) {
            nodes.push_back(std::make_unique<SelectorNode>(token.text));
        } else if (token.type == JSTokenType::JSCode) {
            nodes.push_back(std::make_unique<JSCodeNode>(token.text));
        }
        current_++;
    }
    return nodes;
}

} // namespace CHTL_JS
} // namespace CHTL

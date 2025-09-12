#pragma once
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>
namespace CHTL {
struct AttributeNode; struct CssRuleNode;
struct Node { virtual ~Node() = default; };
struct ValueNode : Node {};
struct IdentifierNode : ValueNode { std::string value; explicit IdentifierNode(Token t):value(std::move(t.literal)){} };
struct StringLiteralNode : ValueNode { std::string value; explicit StringLiteralNode(Token t):value(std::move(t.literal)){} };
struct AttributeNode : Node { std::string name; std::unique_ptr<ValueNode> value; };
struct CssRuleNode : Node { std::string selector; std::vector<std::unique_ptr<AttributeNode>> properties; };
struct StyleBlockNode : Node { std::vector<std::unique_ptr<AttributeNode>> inline_properties; };
struct TextNode : Node { std::unique_ptr<ValueNode> textValue; };
struct ElementNode : Node { std::string tagName; std::vector<std::unique_ptr<AttributeNode>> attributes; std::unique_ptr<StyleBlockNode> styleBlock; std::vector<std::unique_ptr<Node>> children;};
struct RootNode : Node { std::vector<std::unique_ptr<Node>> children; std::vector<std::unique_ptr<CssRuleNode>> global_css_rules; };
}

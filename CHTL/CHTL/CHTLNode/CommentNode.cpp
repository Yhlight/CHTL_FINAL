#include "CommentNode.h"
#include <algorithm>
#include <cctype>

namespace CHTL {

CommentNode::CommentNode(const std::string& content, CommentType type)
    : BaseNode(NodeType::COMMENT, "comment", content), commentType(type), isGeneratorComment(type == CommentType::GENERATOR) {
}

std::string CommentNode::getTrimmedContent() const {
    std::string trimmed = value;
    
    // 去除前导空白
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(),
        [](unsigned char ch) { return !std::isspace(ch); }));
    
    // 去除尾随空白
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(),
        [](unsigned char ch) { return !std::isspace(ch); }).base(), trimmed.end());
    
    return trimmed;
}

bool CommentNode::isEmpty() const {
    return value.empty();
}

bool CommentNode::isWhitespace() const {
    return std::all_of(value.begin(), value.end(), [this](char c) {
        return isWhitespaceChar(c);
    });
}

bool CommentNode::isValid() const {
    return true; // 注释节点总是有效的
}

std::string CommentNode::toHTML() const {
    // 根据注释类型生成HTML注释
    if (commentType == CommentType::SINGLE_LINE) {
        return "<!-- " + value + " -->";
    } else if (commentType == CommentType::MULTI_LINE) {
        return "<!-- " + value + " -->";
    } else if (commentType == CommentType::GENERATOR) {
        // 生成器注释根据上下文生成不同语言的注释
        return "<!-- " + value + " -->";
    }
    
    return "<!-- " + value + " -->";
}

std::string CommentNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "CommentType: " << getCommentTypeName() << "\n";
    oss << "Content: \"" << value << "\"\n";
    oss << "IsGenerator: " << (isGeneratorComment ? "true" : "false") << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> CommentNode::clone() const {
    auto cloned = std::make_shared<CommentNode>(value, commentType);
    cloned->isGeneratorComment = isGeneratorComment;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    return cloned;
}

bool CommentNode::operator==(const CommentNode& other) const {
    return BaseNode::operator==(other) &&
           commentType == other.commentType &&
           isGeneratorComment == other.isGeneratorComment;
}

bool CommentNode::operator!=(const CommentNode& other) const {
    return !(*this == other);
}

std::string CommentNode::getCommentTypeName() const {
    switch (commentType) {
        case CommentType::SINGLE_LINE: return "SINGLE_LINE";
        case CommentType::MULTI_LINE: return "MULTI_LINE";
        case CommentType::GENERATOR: return "GENERATOR";
        default: return "UNKNOWN";
    }
}

bool CommentNode::isWhitespaceChar(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

} // namespace CHTL
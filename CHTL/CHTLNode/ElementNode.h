#ifndef CHTL_ELEMENTNODE_H
#define CHTL_ELEMENTNODE_H

#include "CHTLBaseNode.h"

namespace CHTL {

/**
 * HTML元素节点
 */
class ElementNode : public CHTLBaseNode {
public:
    ElementNode(const std::string& tagName);
    ~ElementNode();
    
    void setTagName(const std::string& tagName);
    const std::string& getTagName() const;
    
    void setTextContent(const std::string& text);
    const std::string& getTextContent() const;
    bool hasTextContent() const;
    
    void setSelfClosing(bool selfClosing);
    bool isSelfClosing() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string tag_name_;
    std::string text_content_;
    bool self_closing_;
    bool has_text_content_;
};

/**
 * 文本节点
 */
class TextNode : public CHTLBaseNode {
public:
    TextNode(const std::string& text);
    ~TextNode();
    
    void setText(const std::string& text);
    const std::string& getText() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string text_;
};

/**
 * 注释节点
 */
class CommentNode : public CHTLBaseNode {
public:
    CommentNode(const std::string& comment);
    ~CommentNode();
    
    void setComment(const std::string& comment);
    const std::string& getComment() const;
    
    void setGeneratorComment(bool generator);
    bool isGeneratorComment() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string comment_;
    bool generator_comment_;
};

} // namespace CHTL

#endif // CHTL_ELEMENTNODE_H
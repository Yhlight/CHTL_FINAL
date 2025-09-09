package node

import (
	"chtl/CHTL/lexer"
)

// ElementNode 元素节点
type ElementNode struct {
	*BaseNode
	TagName    string
	Attributes map[string]*AttributeNode
	Text       *TextNode
	Style      *StyleNode
	Script     *ScriptNode
}

// NewElementNode 创建元素节点
func NewElementNode(tagName string, pos lexer.Position) *ElementNode {
	return &ElementNode{
		BaseNode:   NewBaseNode(ELEMENT, pos),
		TagName:    tagName,
		Attributes: make(map[string]*AttributeNode),
	}
}

// Accept 接受访问者
func (n *ElementNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitElement(n)
}

// AddAttribute 添加属性
func (n *ElementNode) AddAttribute(attr *AttributeNode) {
	if attr != nil {
		n.Attributes[attr.Name] = attr
		attr.SetParent(n)
	}
}

// GetAttribute 获取属性
func (n *ElementNode) GetAttribute(name string) (*AttributeNode, bool) {
	attr, exists := n.Attributes[name]
	return attr, exists
}

// SetText 设置文本内容
func (n *ElementNode) SetText(text *TextNode) {
	if text != nil {
		n.Text = text
		text.SetParent(n)
	}
}

// SetStyle 设置样式
func (n *ElementNode) SetStyle(style *StyleNode) {
	if style != nil {
		n.Style = style
		style.SetParent(n)
	}
}

// SetScript 设置脚本
func (n *ElementNode) SetScript(script *ScriptNode) {
	if script != nil {
		n.Script = script
		script.SetParent(n)
	}
}

// IsSelfClosing 检查是否为自闭合标签
func (n *ElementNode) IsSelfClosing() bool {
	selfClosingTags := map[string]bool{
		"area":   true,
		"base":   true,
		"br":     true,
		"col":    true,
		"embed":  true,
		"hr":     true,
		"img":    true,
		"input":  true,
		"link":   true,
		"meta":   true,
		"param":  true,
		"source": true,
		"track":  true,
		"wbr":    true,
	}
	return selfClosingTags[n.TagName]
}

// TextNode 文本节点
type TextNode struct {
	*BaseNode
	Content string
}

// NewTextNode 创建文本节点
func NewTextNode(content string, pos lexer.Position) *TextNode {
	return &TextNode{
		BaseNode: NewBaseNode(TEXT, pos),
		Content:  content,
	}
}

// Accept 接受访问者
func (n *TextNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitText(n)
}

// CommentNode 注释节点
type CommentNode struct {
	*BaseNode
	Content string
	Type    CommentType
}


// NewCommentNode 创建注释节点
func NewCommentNode(content string, commentType CommentType, pos lexer.Position) *CommentNode {
	return &CommentNode{
		BaseNode: NewBaseNode(COMMENT, pos),
		Content:  content,
		Type:     commentType,
	}
}

// Accept 接受访问者
func (n *CommentNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitComment(n)
}

// AttributeNode 属性节点
type AttributeNode struct {
	*BaseNode
	Name      string
	Value     string
	IsLiteral bool
}

// NewAttributeNode 创建属性节点
func NewAttributeNode(name, value string, isLiteral bool, pos lexer.Position) *AttributeNode {
	return &AttributeNode{
		BaseNode:  NewBaseNode(ATTRIBUTE, pos),
		Name:      name,
		Value:     value,
		IsLiteral: isLiteral,
	}
}

// Accept 接受访问者
func (n *AttributeNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitAttribute(n)
}
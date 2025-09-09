package node

import (
	"chtl/CHTL/lexer"
)

// Node 基础节点接口
type Node interface {
	Type() NodeType
	Position() lexer.Position
	Children() []Node
	AddChild(child Node)
	SetParent(parent Node)
	Parent() Node
	Accept(visitor Visitor) interface{}
}

// BaseNode 基础节点实现
type BaseNode struct {
	nodeType NodeType
	pos      lexer.Position
	children []Node
	parent   Node
}

// NewBaseNode 创建基础节点
func NewBaseNode(nodeType NodeType, pos lexer.Position) *BaseNode {
	return &BaseNode{
		nodeType: nodeType,
		pos:      pos,
		children: make([]Node, 0),
	}
}

// Type 返回节点类型
func (n *BaseNode) Type() NodeType {
	return n.nodeType
}

// Position 返回位置信息
func (n *BaseNode) Position() lexer.Position {
	return n.pos
}

// Children 返回子节点
func (n *BaseNode) Children() []Node {
	return n.children
}

// AddChild 添加子节点
func (n *BaseNode) AddChild(child Node) {
	if child != nil {
		child.SetParent(n)
		n.children = append(n.children, child)
	}
}

// SetParent 设置父节点
func (n *BaseNode) SetParent(parent Node) {
	n.parent = parent
}

// Parent 返回父节点
func (n *BaseNode) Parent() Node {
	return n.parent
}

// Accept 接受访问者
func (n *BaseNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitBaseNode(n)
}

// Visitor 访问者模式接口
type Visitor interface {
	VisitProgram(node *ProgramNode) interface{}
	VisitElement(node *ElementNode) interface{}
	VisitText(node *TextNode) interface{}
	VisitComment(node *CommentNode) interface{}
	VisitAttribute(node *AttributeNode) interface{}
	VisitStyle(node *StyleNode) interface{}
	VisitScript(node *ScriptNode) interface{}
	VisitTemplate(node *TemplateNode) interface{}
	VisitCustom(node *CustomNode) interface{}
	VisitImport(node *ImportNode) interface{}
	VisitOrigin(node *OriginNode) interface{}
	VisitNamespace(node *NamespaceNode) interface{}
	VisitConfiguration(node *ConfigurationNode) interface{}
	VisitUse(node *UseNode) interface{}
	VisitExcept(node *ExceptNode) interface{}
	VisitBaseNode(node *BaseNode) interface{}
}
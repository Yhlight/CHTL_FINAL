package node

import (
	"chtl/CHTL/lexer"
)

// TemplateNode 模板节点
type TemplateNode struct {
	*BaseNode
	TemplateType TemplateType
	Name         string
	Content      []Node
	Inherits     []string
	Exports      []string
}


// NewTemplateNode 创建模板节点
func NewTemplateNode(templateType TemplateType, name string, pos lexer.Position) *TemplateNode {
	return &TemplateNode{
		BaseNode:     NewBaseNode(TEMPLATE_STYLE, pos),
		TemplateType: templateType,
		Name:         name,
		Content:      make([]Node, 0),
		Inherits:     make([]string, 0),
		Exports:      make([]string, 0),
	}
}

// Accept 接受访问者
func (n *TemplateNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitTemplate(n)
}

// AddContent 添加内容
func (n *TemplateNode) AddContent(content Node) {
	if content != nil {
		n.Content = append(n.Content, content)
		content.SetParent(n)
	}
}

// AddInherit 添加继承
func (n *TemplateNode) AddInherit(inherit string) {
	n.Inherits = append(n.Inherits, inherit)
}

// AddExport 添加导出
func (n *TemplateNode) AddExport(export string) {
	n.Exports = append(n.Exports, export)
}

// CustomNode 自定义节点
type CustomNode struct {
	*BaseNode
	CustomType CustomType
	Name       string
	Content    []Node
	Specializations []*Specialization
	Inherits   []string
	Exports    []string
}


// NewCustomNode 创建自定义节点
func NewCustomNode(customType CustomType, name string, pos lexer.Position) *CustomNode {
	return &CustomNode{
		BaseNode:        NewBaseNode(CUSTOM_STYLE, pos),
		CustomType:      customType,
		Name:            name,
		Content:         make([]Node, 0),
		Specializations: make([]*Specialization, 0),
		Inherits:        make([]string, 0),
		Exports:         make([]string, 0),
	}
}

// Accept 接受访问者
func (n *CustomNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitCustom(n)
}

// AddContent 添加内容
func (n *CustomNode) AddContent(content Node) {
	if content != nil {
		n.Content = append(n.Content, content)
		content.SetParent(n)
	}
}

// AddSpecialization 添加特例化
func (n *CustomNode) AddSpecialization(spec *Specialization) {
	if spec != nil {
		n.Specializations = append(n.Specializations, spec)
		spec.SetParent(n)
	}
}

// AddInherit 添加继承
func (n *CustomNode) AddInherit(inherit string) {
	n.Inherits = append(n.Inherits, inherit)
}

// AddExport 添加导出
func (n *CustomNode) AddExport(export string) {
	n.Exports = append(n.Exports, export)
}

// Specialization 特例化
type Specialization struct {
	*BaseNode
	Type        SpecializationType
	Target      string
	Operations  []*SpecializationOperation
}

// Children 返回子节点
func (s *Specialization) Children() []Node {
	// 暂时返回空切片
	return []Node{}
}

// AddChild 添加子节点
func (s *Specialization) AddChild(child Node) {
	// 暂时不实现
}

// Accept 接受访问者
func (s *Specialization) Accept(visitor Visitor) interface{} {
	return visitor.VisitBaseNode(s.BaseNode)
}


// NewSpecialization 创建特例化
func NewSpecialization(specType SpecializationType, target string, pos lexer.Position) *Specialization {
	return &Specialization{
		BaseNode:   NewBaseNode(ATTRIBUTE, pos),
		Type:       specType,
		Target:     target,
		Operations: make([]*SpecializationOperation, 0),
	}
}

// AddOperation 添加操作
func (s *Specialization) AddOperation(op *SpecializationOperation) {
	if op != nil {
		s.Operations = append(s.Operations, op)
		// op.SetParent(s)  // 暂时注释掉
	}
}

// SpecializationOperation 特例化操作
type SpecializationOperation struct {
	*BaseNode
	OperationType OperationType
	Position      InsertPosition
	Content       []Node
	Selector      string
}

// Children 返回子节点
func (op *SpecializationOperation) Children() []Node {
	return op.Content
}

// AddChild 添加子节点
func (op *SpecializationOperation) AddChild(child Node) {
	if child != nil {
		op.Content = append(op.Content, child)
		// child.SetParent(op)  // 暂时注释掉
	}
}

// Accept 接受访问者
func (op *SpecializationOperation) Accept(visitor Visitor) interface{} {
	return visitor.VisitBaseNode(op.BaseNode)
}


// NewSpecializationOperation 创建特例化操作
func NewSpecializationOperation(opType OperationType, pos lexer.Position) *SpecializationOperation {
	return &SpecializationOperation{
		BaseNode:      NewBaseNode(ATTRIBUTE, pos),
		OperationType: opType,
		Content:       make([]Node, 0),
	}
}

// AddContent 添加内容
func (op *SpecializationOperation) AddContent(content Node) {
	if content != nil {
		op.Content = append(op.Content, content)
		// content.SetParent(op)  // 暂时注释掉
	}
}
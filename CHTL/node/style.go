package node

import (
	"chtl/CHTL/lexer"
)

// StyleNode 样式节点
type StyleNode struct {
	*BaseNode
	Properties    map[string]*StyleProperty
	Selectors     []*StyleSelector
	IsInline      bool
	IsLocal       bool
	AutoAddClass  bool
	AutoAddID     bool
}

// NewStyleNode 创建样式节点
func NewStyleNode(isInline, isLocal bool, pos lexer.Position) *StyleNode {
	return &StyleNode{
		BaseNode:     NewBaseNode(STYLE, pos),
		Properties:   make(map[string]*StyleProperty),
		Selectors:    make([]*StyleSelector, 0),
		IsInline:     isInline,
		IsLocal:      isLocal,
		AutoAddClass: true,
		AutoAddID:    true,
	}
}

// Accept 接受访问者
func (n *StyleNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitStyle(n)
}

// AddProperty 添加样式属性
func (n *StyleNode) AddProperty(prop *StyleProperty) {
	if prop != nil {
		n.Properties[prop.Name] = prop
		prop.SetParent(n)
	}
}

// AddSelector 添加选择器
func (n *StyleNode) AddSelector(selector *StyleSelector) {
	if selector != nil {
		n.Selectors = append(n.Selectors, selector)
		selector.SetParent(n)
	}
}

// StyleProperty 样式属性
type StyleProperty struct {
	*BaseNode
	Name      string
	Value     string
	Condition *ConditionalExpression
}

// NewStyleProperty 创建样式属性
func NewStyleProperty(name, value string, pos lexer.Position) *StyleProperty {
	return &StyleProperty{
		BaseNode: NewBaseNode(ATTRIBUTE, pos),
		Name:     name,
		Value:    value,
	}
}

// StyleSelector 样式选择器
type StyleSelector struct {
	*BaseNode
	Selector string
	Type     SelectorType
	Properties map[string]*StyleProperty
}

// Children 返回子节点
func (s *StyleSelector) Children() []Node {
	nodes := make([]Node, 0, len(s.Properties))
	for _, prop := range s.Properties {
		nodes = append(nodes, prop)
	}
	return nodes
}

// AddChild 添加子节点
func (s *StyleSelector) AddChild(child Node) {
	if child != nil {
		if prop, ok := child.(*StyleProperty); ok {
			s.Properties[prop.Name] = prop
			// prop.SetParent(s)  // 暂时注释掉
		}
	}
}

// Accept 接受访问者
func (s *StyleSelector) Accept(visitor Visitor) interface{} {
	return visitor.VisitBaseNode(s.BaseNode)
}


// NewStyleSelector 创建样式选择器
func NewStyleSelector(selector string, selectorType SelectorType, pos lexer.Position) *StyleSelector {
	return &StyleSelector{
		BaseNode:   NewBaseNode(ATTRIBUTE, pos),
		Selector:   selector,
		Type:       selectorType,
		Properties: make(map[string]*StyleProperty),
	}
}

// AddProperty 添加属性到选择器
func (s *StyleSelector) AddProperty(prop *StyleProperty) {
	if prop != nil {
		s.Properties[prop.Name] = prop
		// prop.SetParent(s)  // 暂时注释掉
	}
}

// ConditionalExpression 条件表达式
type ConditionalExpression struct {
	*BaseNode
	Condition string
	TrueValue string
	FalseValue string
	Chain      []*ConditionalExpression
	LogicOp    LogicOperator
}


// NewConditionalExpression 创建条件表达式
func NewConditionalExpression(condition, trueValue, falseValue string, pos lexer.Position) *ConditionalExpression {
	return &ConditionalExpression{
		BaseNode:   NewBaseNode(ATTRIBUTE, pos),
		Condition:  condition,
		TrueValue:  trueValue,
		FalseValue: falseValue,
		Chain:      make([]*ConditionalExpression, 0),
	}
}

// ScriptNode 脚本节点
type ScriptNode struct {
	*BaseNode
	Content    string
	IsLocal    bool
	IsCHTLJS   bool
	Selectors  []*ScriptSelector
	Functions  []*ScriptFunction
}

// NewScriptNode 创建脚本节点
func NewScriptNode(isLocal, isCHTLJS bool, pos lexer.Position) *ScriptNode {
	return &ScriptNode{
		BaseNode:  NewBaseNode(SCRIPT, pos),
		IsLocal:   isLocal,
		IsCHTLJS:  isCHTLJS,
		Selectors: make([]*ScriptSelector, 0),
		Functions: make([]*ScriptFunction, 0),
	}
}

// Accept 接受访问者
func (n *ScriptNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitScript(n)
}

// AddSelector 添加脚本选择器
func (n *ScriptNode) AddSelector(selector *ScriptSelector) {
	if selector != nil {
		n.Selectors = append(n.Selectors, selector)
		selector.SetParent(n)
	}
}

// AddFunction 添加脚本函数
func (n *ScriptNode) AddFunction(fn *ScriptFunction) {
	if fn != nil {
		n.Functions = append(n.Functions, fn)
		fn.SetParent(n)
	}
}

// ScriptSelector 脚本选择器
type ScriptSelector struct {
	*BaseNode
	Selector string
	Type     SelectorType
}

// NewScriptSelector 创建脚本选择器
func NewScriptSelector(selector string, selectorType SelectorType, pos lexer.Position) *ScriptSelector {
	return &ScriptSelector{
		BaseNode: NewBaseNode(ATTRIBUTE, pos),
		Selector: selector,
		Type:     selectorType,
	}
}

// ScriptFunction 脚本函数
type ScriptFunction struct {
	*BaseNode
	Name       string
	Parameters map[string]interface{}
	Body       string
	Type       FunctionType
}


// NewScriptFunction 创建脚本函数
func NewScriptFunction(name string, functionType FunctionType, pos lexer.Position) *ScriptFunction {
	return &ScriptFunction{
		BaseNode:   NewBaseNode(ATTRIBUTE, pos),
		Name:       name,
		Parameters: make(map[string]interface{}),
		Type:       functionType,
	}
}
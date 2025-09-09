package node

import (
	"chtl/CHTL/lexer"
)

// ImportNode 导入节点
type ImportNode struct {
	*BaseNode
	ImportType ImportType
	Path       string
	Alias      string
	Target     string
	Namespace  string
}


// NewImportNode 创建导入节点
func NewImportNode(importType ImportType, path, alias, target, namespace string, pos lexer.Position) *ImportNode {
	return &ImportNode{
		BaseNode:   NewBaseNode(IMPORT_HTML, pos),
		ImportType: importType,
		Path:       path,
		Alias:      alias,
		Target:     target,
		Namespace:  namespace,
	}
}

// Accept 接受访问者
func (n *ImportNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitImport(n)
}

// OriginNode 原始嵌入节点
type OriginNode struct {
	*BaseNode
	OriginType OriginType
	Name       string
	Content    string
	Language   string
}


// NewOriginNode 创建原始嵌入节点
func NewOriginNode(originType OriginType, name, content, language string, pos lexer.Position) *OriginNode {
	return &OriginNode{
		BaseNode:   NewBaseNode(ORIGIN, pos),
		OriginType: originType,
		Name:       name,
		Content:    content,
		Language:   language,
	}
}

// Accept 接受访问者
func (n *OriginNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitOrigin(n)
}

// NamespaceNode 命名空间节点
type NamespaceNode struct {
	*BaseNode
	Name      string
	Content   []Node
	Parent    *NamespaceNode
	Children  []*NamespaceNode
	Exports   []string
	Imports   []*ImportNode
	Templates []*TemplateNode
	Customs   []*CustomNode
}

// GetChildren 返回子节点
func (n *NamespaceNode) GetChildren() []Node {
	// 暂时返回空切片
	return []Node{}
}

// AddChild 添加子节点
func (n *NamespaceNode) AddChild(child Node) {
	// 暂时不实现
}

// Accept 接受访问者
func (n *NamespaceNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitNamespace(n)
}

// NewNamespaceNode 创建命名空间节点
func NewNamespaceNode(name string, pos lexer.Position) *NamespaceNode {
	return &NamespaceNode{
		BaseNode:  NewBaseNode(NAMESPACE, pos),
		Name:      name,
		Content:   make([]Node, 0),
		Children:  make([]*NamespaceNode, 0),
		Exports:   make([]string, 0),
		Imports:   make([]*ImportNode, 0),
		Templates: make([]*TemplateNode, 0),
		Customs:   make([]*CustomNode, 0),
	}
}

// AddContent 添加内容
func (n *NamespaceNode) AddContent(content Node) {
	if content != nil {
		n.Content = append(n.Content, content)
		// content.SetParent(n)  // 暂时注释掉
	}
}


// AddExport 添加导出
func (n *NamespaceNode) AddExport(export string) {
	n.Exports = append(n.Exports, export)
}

// AddImport 添加导入
func (n *NamespaceNode) AddImport(imp *ImportNode) {
	if imp != nil {
		n.Imports = append(n.Imports, imp)
		// imp.SetParent(n)  // 暂时注释掉
	}
}

// AddTemplate 添加模板
func (n *NamespaceNode) AddTemplate(template *TemplateNode) {
	if template != nil {
		n.Templates = append(n.Templates, template)
		// template.SetParent(n)  // 暂时注释掉
	}
}

// AddCustom 添加自定义
func (n *NamespaceNode) AddCustom(custom *CustomNode) {
	if custom != nil {
		n.Customs = append(n.Customs, custom)
		// custom.SetParent(n)  // 暂时注释掉
	}
}

// Configuration 配置结构
type Configuration struct {
	IndexInitialCount        int
	DebugMode               bool
	DisableStyleAutoAddClass bool
	DisableStyleAutoAddID   bool
	DisableDefaultNamespace bool
	DisableCustomOriginType bool
	DisableScriptAutoAddClass bool
	DisableScriptAutoAddID  bool
	Names                   map[string]interface{}
	OriginTypes             map[string]string
}

// SetConfigValue 设置配置值
func (c *Configuration) SetConfigValue(key, value string) {
	c.Names[key] = value
}

// ConfigurationNode 配置节点
type ConfigurationNode struct {
	*Configuration
	*BaseNode
	Name string
}

// NewConfigurationNode 创建配置节点
func NewConfigurationNode(name string, pos lexer.Position) *ConfigurationNode {
	return &ConfigurationNode{
		BaseNode: NewBaseNode(CONFIGURATION, pos),
		Name:     name,
		Configuration: &Configuration{
			IndexInitialCount:        0,
			DebugMode:               false,
			DisableStyleAutoAddClass: false,
			DisableStyleAutoAddID:   false,
			DisableDefaultNamespace: false,
			DisableCustomOriginType: false,
			DisableScriptAutoAddClass: true,
			DisableScriptAutoAddID:  true,
			Names:                   make(map[string]interface{}),
			OriginTypes:             make(map[string]string),
		},
	}
}

// Accept 接受访问者
func (n *ConfigurationNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitConfiguration(n)
}

// UseNode 使用节点
type UseNode struct {
	*BaseNode
	UseType UseType
	Target  string
	Config  string
}


// NewUseNode 创建使用节点
func NewUseNode(useType UseType, target, config string, pos lexer.Position) *UseNode {
	return &UseNode{
		BaseNode: NewBaseNode(USE, pos),
		UseType:  useType,
		Target:   target,
		Config:   config,
	}
}

// Accept 接受访问者
func (n *UseNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitUse(n)
}

// ExceptNode 约束节点
type ExceptNode struct {
	*BaseNode
	Targets []string
	Types   []string
}

// NewExceptNode 创建约束节点
func NewExceptNode(targets, types []string, pos lexer.Position) *ExceptNode {
	return &ExceptNode{
		BaseNode: NewBaseNode(EXCEPT, pos),
		Targets:  targets,
		Types:    types,
	}
}

// Accept 接受访问者
func (n *ExceptNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitExcept(n)
}

// ProgramNode 程序节点
type ProgramNode struct {
	*BaseNode
	Use        *UseNode
	Imports    []*ImportNode
	Namespaces []*NamespaceNode
	Configs    []*ConfigurationNode
	Elements   []*ElementNode
	Excepts    []*ExceptNode
	Templates  []*TemplateNode
	Customs    []*CustomNode
}

// NewProgramNode 创建程序节点
func NewProgramNode(pos lexer.Position) *ProgramNode {
	return &ProgramNode{
		BaseNode:   NewBaseNode(PROGRAM, pos),
		Imports:    make([]*ImportNode, 0),
		Namespaces: make([]*NamespaceNode, 0),
		Configs:    make([]*ConfigurationNode, 0),
		Elements:   make([]*ElementNode, 0),
		Excepts:    make([]*ExceptNode, 0),
		Templates:  make([]*TemplateNode, 0),
		Customs:    make([]*CustomNode, 0),
	}
}

// Accept 接受访问者
func (n *ProgramNode) Accept(visitor Visitor) interface{} {
	return visitor.VisitProgram(n)
}

// AddUse 添加使用声明
func (n *ProgramNode) AddUse(use *UseNode) {
	if use != nil {
		n.Use = use
		use.SetParent(n)
	}
}

// AddImport 添加导入
func (n *ProgramNode) AddImport(imp *ImportNode) {
	if imp != nil {
		n.Imports = append(n.Imports, imp)
		imp.SetParent(n)
	}
}

// AddNamespace 添加命名空间
func (n *ProgramNode) AddNamespace(namespace *NamespaceNode) {
	if namespace != nil {
		n.Namespaces = append(n.Namespaces, namespace)
		namespace.SetParent(n)
	}
}

// AddConfig 添加配置
func (n *ProgramNode) AddConfig(config *ConfigurationNode) {
	if config != nil {
		n.Configs = append(n.Configs, config)
		config.SetParent(n)
	}
}

// AddElement 添加元素
func (n *ProgramNode) AddElement(element *ElementNode) {
	if element != nil {
		n.Elements = append(n.Elements, element)
		element.SetParent(n)
	}
}

// AddExcept 添加约束
func (n *ProgramNode) AddExcept(except *ExceptNode) {
	if except != nil {
		n.Excepts = append(n.Excepts, except)
		except.SetParent(n)
	}
}

// AddTemplate 添加模板
func (n *ProgramNode) AddTemplate(template *TemplateNode) {
	if template != nil {
		n.Templates = append(n.Templates, template)
		template.SetParent(n)
	}
}

// AddCustom 添加自定义
func (n *ProgramNode) AddCustom(custom *CustomNode) {
	if custom != nil {
		n.Customs = append(n.Customs, custom)
		custom.SetParent(n)
	}
}
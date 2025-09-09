package node

// NodeType 节点类型
type NodeType int

const (
	// 基础节点
	PROGRAM NodeType = iota
	ELEMENT
	TEXT
	COMMENT
	ATTRIBUTE
	TEMPLATE
	CUSTOM
	STYLE
	SCRIPT

	// 模板节点
	TEMPLATE_STYLE
	TEMPLATE_ELEMENT
	TEMPLATE_VAR
	CUSTOM_STYLE
	CUSTOM_ELEMENT
	CUSTOM_VAR

	// 导入节点
	IMPORT_HTML
	IMPORT_CSS
	IMPORT_JS
	IMPORT_CHTL
	IMPORT_CJMOD

	// 其他节点
	ORIGIN
	NAMESPACE
	CONFIGURATION
	USE
	EXCEPT
)

// ImportType 导入类型
type ImportType int

const (
	IMPORT_TYPE_HTML ImportType = iota
	IMPORT_TYPE_CSS
	IMPORT_TYPE_JS
	IMPORT_TYPE_CHTL
	IMPORT_TYPE_CJMOD
	IMPORT_TYPE_CONFIG
)

// TemplateType 模板类型
type TemplateType int

const (
	TEMPLATE_TYPE_STYLE TemplateType = iota
	TEMPLATE_TYPE_ELEMENT
	TEMPLATE_TYPE_VAR
)

// CustomType 自定义类型
type CustomType int

const (
	CUSTOM_TYPE_STYLE CustomType = iota
	CUSTOM_TYPE_ELEMENT
	CUSTOM_TYPE_VAR
)

// OriginType 原始嵌入类型
type OriginType int

const (
	ORIGIN_TYPE_HTML OriginType = iota
	ORIGIN_TYPE_CSS
	ORIGIN_TYPE_JS
	ORIGIN_TYPE_CUSTOM
)

// UseType 使用类型
type UseType int

const (
	USE_TYPE_HTML5 UseType = iota
	USE_TYPE_CONFIG
)

// CommentType 注释类型
type CommentType int

const (
	LINE_COMMENT CommentType = iota
	BLOCK_COMMENT
	GENERATOR_COMMENT
)

// SelectorType 选择器类型
type SelectorType int

const (
	CLASS_SELECTOR SelectorType = iota
	ID_SELECTOR
	PSEUDO_SELECTOR
	PSEUDO_ELEMENT_SELECTOR
	ELEMENT_SELECTOR
	DESCENDANT_SELECTOR
	CONTEXT_SELECTOR
)

// LogicOperator 逻辑运算符
type LogicOperator int

const (
	AND_OP LogicOperator = iota
	OR_OP
)

// SpecializationType 特例化类型
type SpecializationType int

const (
	DELETE_ATTRIBUTE SpecializationType = iota
	DELETE_INHERIT
	DELETE_ELEMENT
	INSERT_ELEMENT
	ADD_STYLE
	ADD_ATTRIBUTE
	OVERRIDE_VALUE
)

// OperationType 操作类型
type OperationType int

const (
	DELETE_OP OperationType = iota
	INSERT_OP
	REPLACE_OP
	ADD_OP
	MODIFY_OP
)

// InsertPosition 插入位置
type InsertPosition int

const (
	BEFORE InsertPosition = iota
	AFTER
	AT_TOP
	AT_BOTTOM
	REPLACE
)

// FunctionType 函数类型
type FunctionType int

const (
	LISTEN_FUNCTION FunctionType = iota
	DELEGATE_FUNCTION
	ANIMATE_FUNCTION
	ROUTER_FUNCTION
	UTIL_FUNCTION
	VIRTUAL_FUNCTION
	FILELOADER_FUNCTION
	CUSTOM_FUNCTION
)
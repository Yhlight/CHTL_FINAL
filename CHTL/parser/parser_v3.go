package parser

import (
	"chtl/CHTL/lexer"
	"chtl/CHTL/node"
	"fmt"
)

// ParserV3 重新设计的语法分析器
type ParserV3 struct {
	Lexer        *lexer.Lexer
	CurrentToken lexer.Token
	PeekToken    lexer.Token
	errors       []string
}

// NewParserV3 创建重新设计的语法分析器
func NewParserV3(lexer *lexer.Lexer) *ParserV3 {
	p := &ParserV3{
		Lexer:  lexer,
		errors: make([]string, 0),
	}
	p.NextToken()
	p.NextToken()
	return p
}

// ParseProgram 解析程序
func (p *ParserV3) ParseProgram() *node.ProgramNode {
	program := node.NewProgramNode(p.CurrentToken.Position)

	for p.CurrentToken.Type != lexer.EOF {
		switch p.CurrentToken.Type {
		case lexer.KEYWORD_USE:
			use := p.parseUse()
			program.AddUse(use)
		case lexer.KEYWORD_IMPORT:
			imp := p.parseImport()
			program.AddImport(imp)
		case lexer.KEYWORD_NAMESPACE:
			namespace := p.parseNamespace()
			program.AddNamespace(namespace)
		case lexer.KEYWORD_CONFIGURATION:
			config := p.parseConfiguration()
			program.AddConfig(config)
		case lexer.KEYWORD_EXCEPT:
			except := p.parseExcept()
			program.AddExcept(except)
		case lexer.KEYWORD_TEMPLATE:
			template := p.parseTemplate()
			program.AddTemplate(template)
		case lexer.KEYWORD_CUSTOM:
			custom := p.parseCustom()
			program.AddCustom(custom)
		case lexer.IDENTIFIER:
			element := p.parseElement()
			program.AddElement(element)
		default:
			p.addError(fmt.Sprintf("意外的词法单元: %s", p.CurrentToken.Literal))
			p.NextToken()
		}
	}

	return program
}

// parseUse 解析使用声明
func (p *ParserV3) parseUse() *node.UseNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	var useType node.UseType
	var target string

	switch p.CurrentToken.Type {
	case lexer.KEYWORD_HTML5:
		useType = node.USE_TYPE_HTML5
		target = "html5"
		p.NextToken()
	case lexer.KEYWORD_CONFIG:
		useType = node.USE_TYPE_CONFIG
		p.NextToken()
		if p.CurrentToken.Type == lexer.IDENTIFIER {
			target = p.CurrentToken.Literal
			p.NextToken()
		}
	default:
		p.addError(fmt.Sprintf("意外的使用类型: %s", p.CurrentToken.Literal))
	}

	// 消费分号
	if p.CurrentToken.Type == lexer.SEMICOLON {
		p.NextToken()
	}

	return node.NewUseNode(useType, target, "", pos)
}

// parseImport 解析导入
func (p *ParserV3) parseImport() *node.ImportNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	var importType node.ImportType
	var path, alias, target, namespace string

	// 解析导入类型
	switch p.CurrentToken.Literal {
	case "html", "Html":
		importType = node.IMPORT_TYPE_HTML
	case "style", "css", "Style":
		importType = node.IMPORT_TYPE_CSS
	case "javascript", "js", "JavaScript":
		importType = node.IMPORT_TYPE_JS
	case "chtl", "Chtl":
		importType = node.IMPORT_TYPE_CHTL
	case "cjmod", "Cjmod":
		importType = node.IMPORT_TYPE_CJMOD
	case "config", "Config":
		importType = node.IMPORT_TYPE_CONFIG
	default:
		p.addError(fmt.Sprintf("意外的导入类型: %s", p.CurrentToken.Literal))
	}

	p.NextToken()

	// 解析目标（可选）
	if p.CurrentToken.Type == lexer.IDENTIFIER {
		target = p.CurrentToken.Literal
		p.NextToken()
	}

	// 解析 from 关键字
	if p.CurrentToken.Type == lexer.KEYWORD_FROM {
		p.NextToken()
		path = p.CurrentToken.Literal
		p.NextToken()
	}

	// 解析 as 关键字（可选）
	if p.CurrentToken.Type == lexer.KEYWORD_AS {
		p.NextToken()
		alias = p.CurrentToken.Literal
		p.NextToken()
	}

	// 跳过分号
	if p.CurrentToken.Type == lexer.SEMICOLON {
		p.NextToken()
	}

	return node.NewImportNode(importType, path, alias, target, namespace, pos)
}

// parseNamespace 解析命名空间
func (p *ParserV3) parseNamespace() *node.NamespaceNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	name := p.CurrentToken.Literal
	p.NextToken()

	namespace := node.NewNamespaceNode(name, pos)

	// 解析命名空间内容
	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken()
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			switch p.CurrentToken.Type {
			case lexer.KEYWORD_IMPORT:
				imp := p.parseImport()
				namespace.AddImport(imp)
			case lexer.KEYWORD_NAMESPACE:
				_ = p.parseNamespace()
			case lexer.KEYWORD_TEMPLATE:
				template := p.parseTemplate()
				namespace.AddContent(template)
			case lexer.KEYWORD_CUSTOM:
				custom := p.parseCustom()
				namespace.AddContent(custom)
			case lexer.KEYWORD_CONFIGURATION:
				config := p.parseConfiguration()
				namespace.AddContent(config)
			default:
				p.addError(fmt.Sprintf("命名空间内意外的词法单元: %s", p.CurrentToken.Literal))
				p.NextToken()
			}
		}
		if p.CurrentToken.Type == lexer.RIGHT_BRACE {
			p.NextToken()
		}
	}

	return namespace
}

// parseConfiguration 解析配置
func (p *ParserV3) parseConfiguration() *node.ConfigurationNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	var name string
	if p.CurrentToken.Type == lexer.IDENTIFIER {
		name = p.CurrentToken.Literal
		p.NextToken()
	}

	config := node.NewConfigurationNode(name, pos)

	// 解析配置内容
	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken()
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			// 解析配置项
			if p.CurrentToken.Type == lexer.IDENTIFIER {
				key := p.CurrentToken.Literal
				p.NextToken()
				if p.CurrentToken.Type == lexer.EQUALS {
					p.NextToken()
					value := p.CurrentToken.Literal
					p.NextToken()
					// 设置配置值
					config.SetConfigValue(key, value)
				}
			}
			p.NextToken()
		}
		if p.CurrentToken.Type == lexer.RIGHT_BRACE {
			p.NextToken()
		}
	}

	return config
}

// parseExcept 解析约束
func (p *ParserV3) parseExcept() *node.ExceptNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	targets := make([]string, 0)
	types := make([]string, 0)

	for p.CurrentToken.Type != lexer.SEMICOLON && p.CurrentToken.Type != lexer.EOF {
		if p.CurrentToken.Type == lexer.IDENTIFIER {
			targets = append(targets, p.CurrentToken.Literal)
		}
		p.NextToken()
		if p.CurrentToken.Type == lexer.COMMA {
			p.NextToken()
		}
	}

	if p.CurrentToken.Type == lexer.SEMICOLON {
		p.NextToken()
	}

	return node.NewExceptNode(targets, types, pos)
}

// parseElement 解析元素
func (p *ParserV3) parseElement() *node.ElementNode {
	pos := p.CurrentToken.Position
	tagName := p.CurrentToken.Literal
	p.NextToken()

	element := node.NewElementNode(tagName, pos)

	// 解析元素内容
	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken()
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			switch p.CurrentToken.Type {
			case lexer.IDENTIFIER:
				// 检查下一个token来确定类型
				if p.PeekToken.Type == lexer.COLON {
					// 属性
					attr := p.parseAttribute()
					element.AddAttribute(attr)
				} else if p.PeekToken.Type == lexer.LEFT_BRACE {
					// 子元素
					child := p.parseElement()
					element.AddChild(child)
				} else {
					// 可能是属性名，但没有值
					attr := node.NewAttributeNode(p.CurrentToken.Literal, "", false, p.CurrentToken.Position)
					element.AddAttribute(attr)
					p.NextToken()
				}
			case lexer.KEYWORD_TEXT:
				text := p.parseText()
				element.SetText(text)
			case lexer.KEYWORD_STYLE:
				style := p.parseStyle()
				element.SetStyle(style)
			case lexer.KEYWORD_SCRIPT:
				script := p.parseScript()
				element.SetScript(script)
			case lexer.DOT, lexer.HASH:
				// 样式选择器
				selector := p.parseStyleSelector()
				element.AddStyleSelector(selector)
			default:
				p.addError(fmt.Sprintf("元素内意外的词法单元: %s", p.CurrentToken.Literal))
				p.NextToken()
			}
		}
		if p.CurrentToken.Type == lexer.RIGHT_BRACE {
			p.NextToken()
		}
	}

	return element
}

// parseAttribute 解析属性
func (p *ParserV3) parseAttribute() *node.AttributeNode {
	pos := p.CurrentToken.Position
	name := p.CurrentToken.Literal
	p.NextToken()

	var value string
	var isLiteral bool

	if p.CurrentToken.Type == lexer.COLON {
		p.NextToken()
		if p.CurrentToken.Type == lexer.STRING {
			value = p.CurrentToken.Literal
			isLiteral = true
		} else if p.CurrentToken.Type == lexer.IDENTIFIER || p.CurrentToken.Type == lexer.NUMBER {
			value = p.CurrentToken.Literal
			isLiteral = false
		}
		p.NextToken()
	}

	if p.CurrentToken.Type == lexer.SEMICOLON {
		p.NextToken()
	}

	return node.NewAttributeNode(name, value, isLiteral, pos)
}

// parseText 解析文本
func (p *ParserV3) parseText() *node.TextNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	var content string
	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken()
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			content += p.CurrentToken.Literal
			p.NextToken()
		}
		if p.CurrentToken.Type == lexer.RIGHT_BRACE {
			p.NextToken()
		}
	}

	return node.NewTextNode(content, pos)
}

// parseStyle 解析样式
func (p *ParserV3) parseStyle() *node.StyleNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	style := node.NewStyleNode(true, true, pos)

	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken()
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			switch p.CurrentToken.Type {
			case lexer.DOT, lexer.HASH, lexer.AMPERSAND:
				// 样式选择器
				selector := p.parseStyleSelector()
				style.AddSelector(selector)
			case lexer.IDENTIFIER:
				// 样式属性
				prop := p.parseStyleProperty()
				style.AddProperty(prop)
			default:
				p.NextToken()
			}
		}
		if p.CurrentToken.Type == lexer.RIGHT_BRACE {
			p.NextToken()
		}
	}

	return style
}

// parseStyleProperty 解析样式属性
func (p *ParserV3) parseStyleProperty() *node.StyleProperty {
	pos := p.CurrentToken.Position
	name := p.CurrentToken.Literal
	p.NextToken()

	var value string
	if p.CurrentToken.Type == lexer.COLON {
		p.NextToken()
		// 收集所有值直到分号
		for p.CurrentToken.Type != lexer.SEMICOLON && p.CurrentToken.Type != lexer.EOF {
			if value != "" {
				value += " "
			}
			value += p.CurrentToken.Literal
			p.NextToken()
		}
	}

	if p.CurrentToken.Type == lexer.SEMICOLON {
		p.NextToken()
	}

	return node.NewStyleProperty(name, value, pos)
}

// parseStyleSelector 解析样式选择器
func (p *ParserV3) parseStyleSelector() *node.StyleSelector {
	pos := p.CurrentToken.Position
	var selector string
	var selectorType node.SelectorType

	if p.CurrentToken.Type == lexer.DOT {
		selectorType = node.CLASS_SELECTOR
		p.NextToken()
		selector = "." + p.CurrentToken.Literal
		p.NextToken()
	} else if p.CurrentToken.Type == lexer.HASH {
		selectorType = node.ID_SELECTOR
		p.NextToken()
		selector = "#" + p.CurrentToken.Literal
		p.NextToken()
	} else if p.CurrentToken.Type == lexer.AMPERSAND {
		selectorType = node.PSEUDO_SELECTOR
		selector = "&"
		p.NextToken()
		if p.CurrentToken.Type == lexer.COLON {
			p.NextToken()
			if p.CurrentToken.Type == lexer.IDENTIFIER {
				selector += ":" + p.CurrentToken.Literal
				p.NextToken()
			}
		}
	}

	styleSelector := node.NewStyleSelector(selector, selectorType, pos)

	// 解析选择器内容
	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken()
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			if p.CurrentToken.Type == lexer.IDENTIFIER {
				prop := p.parseStyleProperty()
				styleSelector.AddProperty(prop)
			} else {
				p.NextToken()
			}
		}
		if p.CurrentToken.Type == lexer.RIGHT_BRACE {
			p.NextToken()
		}
	}

	return styleSelector
}

// parseScript 解析脚本
func (p *ParserV3) parseScript() *node.ScriptNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	script := node.NewScriptNode(true, true, pos)

	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken()
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			// 解析脚本内容
			content := p.CurrentToken.Literal
			script.Content += content
			p.NextToken()
		}
		if p.CurrentToken.Type == lexer.RIGHT_BRACE {
			p.NextToken()
		}
	}

	return script
}

// parseTemplate 解析模板
func (p *ParserV3) parseTemplate() *node.TemplateNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	var templateType node.TemplateType
	switch p.CurrentToken.Type {
	case lexer.KEYWORD_STYLE:
		templateType = node.TEMPLATE_TYPE_STYLE
	case lexer.KEYWORD_ELEMENT:
		templateType = node.TEMPLATE_TYPE_ELEMENT
	case lexer.KEYWORD_VAR:
		templateType = node.TEMPLATE_TYPE_VAR
	default:
		p.addError(fmt.Sprintf("意外的模板类型: %s", p.CurrentToken.Literal))
	}

	p.NextToken()
	name := p.CurrentToken.Literal
	p.NextToken()

	template := node.NewTemplateNode(templateType, name, pos)

	// 解析模板内容
	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken()
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			switch templateType {
			case node.TEMPLATE_TYPE_STYLE:
				// 样式模板直接解析样式属性
				if p.CurrentToken.Type == lexer.IDENTIFIER {
					prop := p.parseStyleProperty()
					template.AddContent(prop)
				} else {
					p.addError(fmt.Sprintf("样式模板内意外的词法单元: %s", p.CurrentToken.Literal))
					p.NextToken()
				}
			case node.TEMPLATE_TYPE_ELEMENT:
				// 元素模板解析元素
				if p.CurrentToken.Type == lexer.IDENTIFIER {
					content := p.parseElement()
					template.AddContent(content)
				} else if p.CurrentToken.Type == lexer.KEYWORD_STYLE {
					style := p.parseStyle()
					template.AddContent(style)
				} else {
					p.addError(fmt.Sprintf("元素模板内意外的词法单元: %s", p.CurrentToken.Literal))
					p.NextToken()
				}
			case node.TEMPLATE_TYPE_VAR:
				// 变量模板解析变量
				if p.CurrentToken.Type == lexer.IDENTIFIER {
					// 解析变量定义
					_ = p.CurrentToken.Literal
					p.NextToken()
					if p.CurrentToken.Type == lexer.COLON {
						p.NextToken()
						_ = p.CurrentToken.Literal
						p.NextToken()
						// 创建变量节点（简化实现）
						// template.AddContent(NewVarNode(varName, varValue, p.CurrentToken.Position))
					}
				} else {
					p.addError(fmt.Sprintf("变量模板内意外的词法单元: %s", p.CurrentToken.Literal))
					p.NextToken()
				}
			default:
				p.addError(fmt.Sprintf("未知的模板类型: %d", templateType))
				p.NextToken()
			}
		}
		if p.CurrentToken.Type == lexer.RIGHT_BRACE {
			p.NextToken()
		}
	}

	return template
}

// parseCustom 解析自定义
func (p *ParserV3) parseCustom() *node.CustomNode {
	pos := p.CurrentToken.Position
	p.NextToken()

	var customType node.CustomType
	switch p.CurrentToken.Type {
	case lexer.KEYWORD_STYLE:
		customType = node.CUSTOM_TYPE_STYLE
	case lexer.KEYWORD_ELEMENT:
		customType = node.CUSTOM_TYPE_ELEMENT
	case lexer.KEYWORD_VAR:
		customType = node.CUSTOM_TYPE_VAR
	default:
		p.addError(fmt.Sprintf("意外的自定义类型: %s", p.CurrentToken.Literal))
	}

	p.NextToken()
	name := p.CurrentToken.Literal
	p.NextToken()

	custom := node.NewCustomNode(customType, name, pos)

	// 解析自定义内容
	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken()
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			switch p.CurrentToken.Type {
			case lexer.IDENTIFIER:
				// 解析元素
				content := p.parseElement()
				custom.AddContent(content)
			case lexer.KEYWORD_STYLE:
				// 解析样式
				style := p.parseStyle()
				custom.AddContent(style)
			default:
				p.addError(fmt.Sprintf("自定义内意外的词法单元: %s", p.CurrentToken.Literal))
				p.NextToken()
			}
		}
		if p.CurrentToken.Type == lexer.RIGHT_BRACE {
			p.NextToken()
		}
	}

	return custom
}

// NextToken 获取下一个词法单元
func (p *ParserV3) NextToken() {
	p.CurrentToken = p.PeekToken
	p.PeekToken = p.Lexer.NextToken()
}

// addError 添加错误
func (p *ParserV3) addError(message string) {
	p.errors = append(p.errors, message)
}

// Errors 获取错误列表
func (p *ParserV3) Errors() []string {
	return p.errors
}
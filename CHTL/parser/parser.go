package parser

import (
	"chtl/CHTL/lexer"
	"chtl/CHTL/node"
	"fmt"
)

// Parser 语法分析器
type Parser struct {
	lexer        *lexer.Lexer
	currentToken lexer.Token
	peekToken    lexer.Token
	errors       []string
}

// NewParser 创建新的语法分析器
func NewParser(lexer *lexer.Lexer) *Parser {
	p := &Parser{
		lexer:  lexer,
		errors: make([]string, 0),
	}
	p.nextToken()
	p.nextToken()
	return p
}

// ParseProgram 解析程序
func (p *Parser) ParseProgram() *node.ProgramNode {
	program := node.NewProgramNode(p.currentToken.Position)

	for p.currentToken.Type != lexer.EOF {
		switch p.currentToken.Type {
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
		case lexer.IDENTIFIER:
			element := p.parseElement()
			program.AddElement(element)
		default:
			p.addError(fmt.Sprintf("意外的词法单元: %s", p.currentToken.Literal))
			p.nextToken()
		}
	}

	return program
}

// parseUse 解析使用声明
func (p *Parser) parseUse() *node.UseNode {
	pos := p.currentToken.Position
	p.nextToken()

	var useType node.UseType
	var target string

	switch p.currentToken.Type {
	case lexer.KEYWORD_HTML5:
		useType = node.USE_TYPE_HTML5
		target = "html5"
	case lexer.KEYWORD_CONFIG:
		useType = node.USE_TYPE_CONFIG
		p.nextToken()
		if p.currentToken.Type == lexer.IDENTIFIER {
			target = p.currentToken.Literal
		}
	default:
		p.addError(fmt.Sprintf("意外的使用类型: %s", p.currentToken.Literal))
	}

	p.nextToken()
	return node.NewUseNode(useType, target, "", pos)
}

// parseImport 解析导入
func (p *Parser) parseImport() *node.ImportNode {
	pos := p.currentToken.Position
	p.nextToken()

	var importType node.ImportType
	var path, alias, target, namespace string

	// 解析导入类型
	switch p.currentToken.Type {
	case lexer.KEYWORD_HTML:
		importType = node.IMPORT_TYPE_HTML
	case lexer.KEYWORD_STYLE:
		importType = node.IMPORT_TYPE_CSS
	case lexer.KEYWORD_JAVASCRIPT:
		importType = node.IMPORT_TYPE_JS
	case lexer.KEYWORD_CHTL:
		importType = node.IMPORT_TYPE_CHTL
	case lexer.KEYWORD_CJMOD:
		importType = node.IMPORT_TYPE_CJMOD
	case lexer.KEYWORD_CONFIG:
		importType = node.IMPORT_TYPE_CONFIG
	default:
		p.addError(fmt.Sprintf("意外的导入类型: %s", p.currentToken.Literal))
	}

	p.nextToken()

	// 解析目标（可选）
	if p.currentToken.Type == lexer.IDENTIFIER {
		target = p.currentToken.Literal
		p.nextToken()
	}

	// 解析 from 关键字
	if p.currentToken.Type == lexer.KEYWORD_FROM {
		p.nextToken()
		path = p.currentToken.Literal
		p.nextToken()
	}

	// 解析 as 关键字（可选）
	if p.currentToken.Type == lexer.KEYWORD_AS {
		p.nextToken()
		alias = p.currentToken.Literal
		p.nextToken()
	}

	return node.NewImportNode(importType, path, alias, target, namespace, pos)
}

// parseNamespace 解析命名空间
func (p *Parser) parseNamespace() *node.NamespaceNode {
	pos := p.currentToken.Position
	p.nextToken()

	name := p.currentToken.Literal
	p.nextToken()

	namespace := node.NewNamespaceNode(name, pos)

	// 解析命名空间内容
	if p.currentToken.Type == lexer.LEFT_BRACE {
		p.nextToken()
		for p.currentToken.Type != lexer.RIGHT_BRACE && p.currentToken.Type != lexer.EOF {
			switch p.currentToken.Type {
			case lexer.KEYWORD_IMPORT:
				imp := p.parseImport()
				namespace.AddImport(imp)
			case lexer.KEYWORD_NAMESPACE:
				_ = p.parseNamespace()
				// namespace.AddChild(child)  // 暂时注释掉
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
				p.addError(fmt.Sprintf("命名空间内意外的词法单元: %s", p.currentToken.Literal))
				p.nextToken()
			}
		}
		if p.currentToken.Type == lexer.RIGHT_BRACE {
			p.nextToken()
		}
	}

	return namespace
}

// parseConfiguration 解析配置
func (p *Parser) parseConfiguration() *node.ConfigurationNode {
	pos := p.currentToken.Position
	p.nextToken()

	var name string
	if p.currentToken.Type == lexer.IDENTIFIER {
		name = p.currentToken.Literal
		p.nextToken()
	}

	config := node.NewConfigurationNode(name, pos)

	// 解析配置内容
	if p.currentToken.Type == lexer.LEFT_BRACE {
		p.nextToken()
		for p.currentToken.Type != lexer.RIGHT_BRACE && p.currentToken.Type != lexer.EOF {
			// 解析配置项
			if p.currentToken.Type == lexer.IDENTIFIER {
				key := p.currentToken.Literal
				p.nextToken()
				if p.currentToken.Type == lexer.EQUALS {
					p.nextToken()
					value := p.currentToken.Literal
					p.nextToken()
					// 设置配置值
					config.SetConfigValue(key, value)
				}
			}
			p.nextToken()
		}
		if p.currentToken.Type == lexer.RIGHT_BRACE {
			p.nextToken()
		}
	}

	return config
}

// parseExcept 解析约束
func (p *Parser) parseExcept() *node.ExceptNode {
	pos := p.currentToken.Position
	p.nextToken()

	targets := make([]string, 0)
	types := make([]string, 0)

	for p.currentToken.Type != lexer.SEMICOLON && p.currentToken.Type != lexer.EOF {
		if p.currentToken.Type == lexer.IDENTIFIER {
			targets = append(targets, p.currentToken.Literal)
		}
		p.nextToken()
		if p.currentToken.Type == lexer.COMMA {
			p.nextToken()
		}
	}

	if p.currentToken.Type == lexer.SEMICOLON {
		p.nextToken()
	}

	return node.NewExceptNode(targets, types, pos)
}

// parseElement 解析元素
func (p *Parser) parseElement() *node.ElementNode {
	pos := p.currentToken.Position
	tagName := p.currentToken.Literal
	p.nextToken()

	element := node.NewElementNode(tagName, pos)

	// 解析属性
	if p.currentToken.Type == lexer.LEFT_BRACE {
		p.nextToken()
		for p.currentToken.Type != lexer.RIGHT_BRACE && p.currentToken.Type != lexer.EOF {
			switch p.currentToken.Type {
			case lexer.IDENTIFIER:
				attr := p.parseAttribute()
				element.AddAttribute(attr)
			case lexer.KEYWORD_TEXT:
				text := p.parseText()
				element.SetText(text)
			case lexer.KEYWORD_STYLE:
				style := p.parseStyle()
				element.SetStyle(style)
			case lexer.KEYWORD_SCRIPT:
				script := p.parseScript()
				element.SetScript(script)
			default:
				if p.currentToken.Type == lexer.IDENTIFIER {
					// 子元素
					child := p.parseElement()
					element.AddChild(child)
				} else {
					p.addError(fmt.Sprintf("元素内意外的词法单元: %s", p.currentToken.Literal))
					p.nextToken()
				}
			}
		}
		if p.currentToken.Type == lexer.RIGHT_BRACE {
			p.nextToken()
		}
	}

	return element
}

// parseAttribute 解析属性
func (p *Parser) parseAttribute() *node.AttributeNode {
	pos := p.currentToken.Position
	name := p.currentToken.Literal
	p.nextToken()

	var value string
	var isLiteral bool

	if p.currentToken.Type == lexer.COLON || p.currentToken.Type == lexer.EQUALS {
		p.nextToken()
		if p.currentToken.Type == lexer.STRING {
			value = p.currentToken.Literal
			isLiteral = true
		} else if p.currentToken.Type == lexer.IDENTIFIER {
			value = p.currentToken.Literal
			isLiteral = false
		}
		p.nextToken()
	}

	if p.currentToken.Type == lexer.SEMICOLON {
		p.nextToken()
	}

	return node.NewAttributeNode(name, value, isLiteral, pos)
}

// parseText 解析文本
func (p *Parser) parseText() *node.TextNode {
	pos := p.currentToken.Position
	p.nextToken()

	var content string
	if p.currentToken.Type == lexer.LEFT_BRACE {
		p.nextToken()
		if p.currentToken.Type == lexer.STRING {
			content = p.currentToken.Literal
		} else if p.currentToken.Type == lexer.IDENTIFIER {
			content = p.currentToken.Literal
		}
		p.nextToken()
		if p.currentToken.Type == lexer.RIGHT_BRACE {
			p.nextToken()
		}
	}

	return node.NewTextNode(content, pos)
}

// parseStyle 解析样式
func (p *Parser) parseStyle() *node.StyleNode {
	pos := p.currentToken.Position
	p.nextToken()

	style := node.NewStyleNode(false, true, pos)

	if p.currentToken.Type == lexer.LEFT_BRACE {
		p.nextToken()
		for p.currentToken.Type != lexer.RIGHT_BRACE && p.currentToken.Type != lexer.EOF {
			// 解析样式属性或选择器
			if p.currentToken.Type == lexer.IDENTIFIER {
				prop := p.parseStyleProperty()
				style.AddProperty(prop)
			} else if p.currentToken.Type == lexer.DOT || p.currentToken.Type == lexer.HASH {
				selector := p.parseStyleSelector()
				style.AddSelector(selector)
			}
		}
		if p.currentToken.Type == lexer.RIGHT_BRACE {
			p.nextToken()
		}
	}

	return style
}

// parseStyleProperty 解析样式属性
func (p *Parser) parseStyleProperty() *node.StyleProperty {
	pos := p.currentToken.Position
	name := p.currentToken.Literal
	p.nextToken()

	var value string
	if p.currentToken.Type == lexer.COLON {
		p.nextToken()
		value = p.currentToken.Literal
		p.nextToken()
	}

	if p.currentToken.Type == lexer.SEMICOLON {
		p.nextToken()
	}

	return node.NewStyleProperty(name, value, pos)
}

// parseStyleSelector 解析样式选择器
func (p *Parser) parseStyleSelector() *node.StyleSelector {
	pos := p.currentToken.Position
	var selector string
	var selectorType node.SelectorType

	if p.currentToken.Type == lexer.DOT {
		selectorType = node.CLASS_SELECTOR
		selector = "." + p.currentToken.Literal
	} else if p.currentToken.Type == lexer.HASH {
		selectorType = node.ID_SELECTOR
		selector = "#" + p.currentToken.Literal
	}

	p.nextToken()

	styleSelector := node.NewStyleSelector(selector, selectorType, pos)

	// 解析选择器内容
	if p.currentToken.Type == lexer.LEFT_BRACE {
		p.nextToken()
		for p.currentToken.Type != lexer.RIGHT_BRACE && p.currentToken.Type != lexer.EOF {
			if p.currentToken.Type == lexer.IDENTIFIER {
				prop := p.parseStyleProperty()
				styleSelector.AddProperty(prop)
			}
		}
		if p.currentToken.Type == lexer.RIGHT_BRACE {
			p.nextToken()
		}
	}

	return styleSelector
}

// parseScript 解析脚本
func (p *Parser) parseScript() *node.ScriptNode {
	pos := p.currentToken.Position
	p.nextToken()

	script := node.NewScriptNode(true, true, pos)

	if p.currentToken.Type == lexer.LEFT_BRACE {
		p.nextToken()
		for p.currentToken.Type != lexer.RIGHT_BRACE && p.currentToken.Type != lexer.EOF {
			// 解析脚本内容
			content := p.currentToken.Literal
			script.Content += content
			p.nextToken()
		}
		if p.currentToken.Type == lexer.RIGHT_BRACE {
			p.nextToken()
		}
	}

	return script
}

// parseTemplate 解析模板
func (p *Parser) parseTemplate() *node.TemplateNode {
	pos := p.currentToken.Position
	p.nextToken()

	var templateType node.TemplateType
	switch p.currentToken.Type {
	case lexer.KEYWORD_STYLE:
		templateType = node.TEMPLATE_TYPE_STYLE
	case lexer.KEYWORD_ELEMENT:
		templateType = node.TEMPLATE_TYPE_ELEMENT
	case lexer.KEYWORD_VAR:
		templateType = node.TEMPLATE_TYPE_VAR
	default:
		p.addError(fmt.Sprintf("意外的模板类型: %s", p.currentToken.Literal))
	}

	p.nextToken()
	name := p.currentToken.Literal
	p.nextToken()

	template := node.NewTemplateNode(templateType, name, pos)

	// 解析模板内容
	if p.currentToken.Type == lexer.LEFT_BRACE {
		p.nextToken()
		for p.currentToken.Type != lexer.RIGHT_BRACE && p.currentToken.Type != lexer.EOF {
			// 解析模板内容
			content := p.parseElement()
			template.AddContent(content)
		}
		if p.currentToken.Type == lexer.RIGHT_BRACE {
			p.nextToken()
		}
	}

	return template
}

// parseCustom 解析自定义
func (p *Parser) parseCustom() *node.CustomNode {
	pos := p.currentToken.Position
	p.nextToken()

	var customType node.CustomType
	switch p.currentToken.Type {
	case lexer.KEYWORD_STYLE:
		customType = node.CUSTOM_TYPE_STYLE
	case lexer.KEYWORD_ELEMENT:
		customType = node.CUSTOM_TYPE_ELEMENT
	case lexer.KEYWORD_VAR:
		customType = node.CUSTOM_TYPE_VAR
	default:
		p.addError(fmt.Sprintf("意外的自定义类型: %s", p.currentToken.Literal))
	}

	p.nextToken()
	name := p.currentToken.Literal
	p.nextToken()

	custom := node.NewCustomNode(customType, name, pos)

	// 解析自定义内容
	if p.currentToken.Type == lexer.LEFT_BRACE {
		p.nextToken()
		for p.currentToken.Type != lexer.RIGHT_BRACE && p.currentToken.Type != lexer.EOF {
			// 解析自定义内容
			content := p.parseElement()
			custom.AddContent(content)
		}
		if p.currentToken.Type == lexer.RIGHT_BRACE {
			p.nextToken()
		}
	}

	return custom
}

// nextToken 获取下一个词法单元
func (p *Parser) nextToken() {
	p.currentToken = p.peekToken
	p.peekToken = p.lexer.NextToken()
}

// addError 添加错误
func (p *Parser) addError(message string) {
	p.errors = append(p.errors, message)
}

// Errors 获取错误列表
func (p *Parser) Errors() []string {
	return p.errors
}
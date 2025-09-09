package chtljs

import (
	"chtl/CHTL/lexer"
	"chtl/CHTL/node"
	"fmt"
)

// CHTLJSParser CHTL JS 解析器
type CHTLJSParser struct {
	Lexer        *CHTLJSLexer
	CurrentToken lexer.Token
	PeekToken    lexer.Token
	Errors       []string
}

// NewCHTLJSParser 创建 CHTL JS 解析器
func NewCHTLJSParser(input string) *CHTLJSParser {
	l := NewCHTLJSLexer(input)
	p := &CHTLJSParser{
		Lexer:  l,
		Errors: []string{},
	}
	
	p.NextToken()
	p.NextToken()
	
	return p
}

// NextToken 获取下一个词法单元
func (p *CHTLJSParser) NextToken() {
	p.CurrentToken = p.PeekToken
	p.PeekToken = p.Lexer.NextToken()
}

// ParseProgram 解析程序
func (p *CHTLJSParser) ParseProgram() *node.ProgramNode {
	program := node.NewProgramNode(p.CurrentToken.Position)
	
	for p.CurrentToken.Type != lexer.EOF {
		switch p.CurrentToken.Type {
		case lexer.KEYWORD_FUNCTION:
			function := p.parseFunction()
			if function != nil {
				program.AddElement(function)
			}
		case lexer.KEYWORD_VAR, lexer.KEYWORD_LET, lexer.KEYWORD_CONST:
			variable := p.parseVariable()
			if variable != nil {
				program.AddElement(variable)
			}
		case lexer.KEYWORD_IF:
			conditional := p.parseConditional()
			if conditional != nil {
				program.AddElement(conditional)
			}
		case lexer.KEYWORD_FOR, lexer.KEYWORD_WHILE:
			loop := p.parseLoop()
			if loop != nil {
				program.AddElement(loop)
			}
		case lexer.KEYWORD_LOAD:
			load := p.parseLoad()
			if load != nil {
				program.AddElement(load)
			}
		case lexer.KEYWORD_LOCAL:
			local := p.parseLocal()
			if local != nil {
				program.AddElement(local)
			}
		case lexer.KEYWORD_SELECT:
			selectNode := p.parseSelect()
			if selectNode != nil {
				program.AddElement(selectNode)
			}
		case lexer.KEYWORD_DELEGATE:
			delegate := p.parseDelegate()
			if delegate != nil {
				program.AddElement(delegate)
			}
		case lexer.KEYWORD_ANIMATE:
			animate := p.parseAnimate()
			if animate != nil {
				program.AddElement(animate)
			}
		case lexer.KEYWORD_VIRTUAL:
			virtual := p.parseVirtual()
			if virtual != nil {
				program.AddElement(virtual)
			}
		case lexer.KEYWORD_ROUTE:
			route := p.parseRoute()
			if route != nil {
				program.AddElement(route)
			}
		default:
			p.addError(fmt.Sprintf("意外的词法单元: %s", p.CurrentToken.Literal))
			p.NextToken()
		}
	}
	
	return program
}

// parseFunction 解析函数
func (p *CHTLJSParser) parseFunction() *node.ElementNode {
	pos := p.CurrentToken.Position
	p.NextToken() // 跳过 'function'
	
	name := p.CurrentToken.Literal
	p.NextToken() // 跳过函数名
	
	// 解析参数
	params := []string{}
	if p.CurrentToken.Type == lexer.LEFT_PAREN {
		p.NextToken() // 跳过 '('
		for p.CurrentToken.Type != lexer.RIGHT_PAREN && p.CurrentToken.Type != lexer.EOF {
			if p.CurrentToken.Type == lexer.IDENTIFIER {
				params = append(params, p.CurrentToken.Literal)
				p.NextToken()
			}
			if p.CurrentToken.Type == lexer.COMMA {
				p.NextToken()
			}
		}
		if p.CurrentToken.Type == lexer.RIGHT_PAREN {
			p.NextToken()
		}
	}
	
	// 解析函数体
	body := ""
	if p.CurrentToken.Type == lexer.LEFT_BRACE {
		p.NextToken() // 跳过 '{'
		braceCount := 1
		for braceCount > 0 && p.CurrentToken.Type != lexer.EOF {
			if p.CurrentToken.Type == lexer.LEFT_BRACE {
				braceCount++
			} else if p.CurrentToken.Type == lexer.RIGHT_BRACE {
				braceCount--
			}
			if braceCount > 0 {
				body += p.CurrentToken.Literal + " "
			}
			p.NextToken()
		}
	}
	
	// 创建函数元素
	element := node.NewElementNode("function", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	element.Attributes["name"] = &node.AttributeNode{
		Name:  "name",
		Value: name,
	}
	element.Attributes["params"] = &node.AttributeNode{
		Name:  "params",
		Value: fmt.Sprintf("%v", params),
	}
	element.Text = &node.TextNode{
		Content: body,
	}
	
	return element
}

// parseVariable 解析变量
func (p *CHTLJSParser) parseVariable() *node.ElementNode {
	pos := p.CurrentToken.Position
	varType := p.CurrentToken.Literal
	p.NextToken() // 跳过 'var', 'let', 或 'const'
	
	name := p.CurrentToken.Literal
	p.NextToken() // 跳过变量名
	
	value := ""
	if p.CurrentToken.Type == lexer.ASSIGN {
		p.NextToken() // 跳过 '='
		// 解析值直到分号或换行
		for p.CurrentToken.Type != lexer.SEMICOLON && p.CurrentToken.Type != lexer.EOF {
			value += p.CurrentToken.Literal + " "
			p.NextToken()
		}
	}
	
	// 跳过分号
	if p.CurrentToken.Type == lexer.SEMICOLON {
		p.NextToken()
	}
	
	element := node.NewElementNode("variable", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	element.Attributes["type"] = &node.AttributeNode{
		Name:  "type",
		Value: varType,
	}
	element.Attributes["name"] = &node.AttributeNode{
		Name:  "name",
		Value: name,
	}
	element.Text = &node.TextNode{
		Content: value,
	}
	
	return element
}

// parseConditional 解析条件语句
func (p *CHTLJSParser) parseConditional() *node.ElementNode {
	pos := p.CurrentToken.Position
	p.NextToken() // 跳过 'if'
	
	condition := ""
	if p.CurrentToken.Type == lexer.LEFT_PAREN {
		p.NextToken() // 跳过 '('
		parenCount := 1
		for parenCount > 0 && p.CurrentToken.Type != lexer.EOF {
			if p.CurrentToken.Type == lexer.LEFT_PAREN {
				parenCount++
			} else if p.CurrentToken.Type == lexer.RIGHT_PAREN {
				parenCount--
			}
			if parenCount > 0 {
				condition += p.CurrentToken.Literal + " "
			}
			p.NextToken()
		}
	}
	
	element := node.NewElementNode("conditional", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	element.Attributes["condition"] = &node.AttributeNode{
		Name:  "condition",
		Value: condition,
	}
	
	return element
}

// parseLoop 解析循环语句
func (p *CHTLJSParser) parseLoop() *node.ElementNode {
	pos := p.CurrentToken.Position
	loopType := p.CurrentToken.Literal
	p.NextToken() // 跳过 'for' 或 'while'
	
	element := node.NewElementNode("loop", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	element.Attributes["type"] = &node.AttributeNode{
		Name:  "type",
		Value: loopType,
	}
	
	return element
}

// parseLoad 解析文件加载
func (p *CHTLJSParser) parseLoad() *node.ElementNode {
	pos := p.CurrentToken.Position
	p.NextToken() // 跳过 'load'
	
	file := p.CurrentToken.Literal
	p.NextToken() // 跳过文件名
	
	element := node.NewElementNode("load", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	element.Attributes["file"] = &node.AttributeNode{
		Name:  "file",
		Value: file,
	}
	
	return element
}

// parseLocal 解析本地脚本
func (p *CHTLJSParser) parseLocal() *node.ElementNode {
	pos := p.CurrentToken.Position
	p.NextToken() // 跳过 'local'
	
	element := node.NewElementNode("local", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	
	// 解析本地脚本内容
	if p.CurrentToken.Type == lexer.KEYWORD_SCRIPT {
		p.NextToken() // 跳过 'script'
		if p.CurrentToken.Type == lexer.LEFT_BRACE {
			p.NextToken() // 跳过 '{'
			body := ""
			braceCount := 1
			for braceCount > 0 && p.CurrentToken.Type != lexer.EOF {
				if p.CurrentToken.Type == lexer.LEFT_BRACE {
					braceCount++
				} else if p.CurrentToken.Type == lexer.RIGHT_BRACE {
					braceCount--
				}
				if braceCount > 0 {
					body += p.CurrentToken.Literal + " "
				}
				p.NextToken()
			}
			element.Text = &node.TextNode{
				Content: body,
			}
		}
	}
	
	return element
}

// parseSelect 解析选择器
func (p *CHTLJSParser) parseSelect() *node.ElementNode {
	pos := p.CurrentToken.Position
	p.NextToken() // 跳过 'select'
	
	selector := p.CurrentToken.Literal
	p.NextToken() // 跳过选择器
	
	element := node.NewElementNode("select", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	element.Attributes["selector"] = &node.AttributeNode{
		Name:  "selector",
		Value: selector,
	}
	
	return element
}

// parseDelegate 解析事件委托
func (p *CHTLJSParser) parseDelegate() *node.ElementNode {
	pos := p.CurrentToken.Position
	p.NextToken() // 跳过 'delegate'
	
	event := p.CurrentToken.Literal
	p.NextToken() // 跳过事件类型
	
	selector := p.CurrentToken.Literal
	p.NextToken() // 跳过选择器
	
	element := node.NewElementNode("delegate", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	element.Attributes["event"] = &node.AttributeNode{
		Name:  "event",
		Value: event,
	}
	element.Attributes["selector"] = &node.AttributeNode{
		Name:  "selector",
		Value: selector,
	}
	
	return element
}

// parseAnimate 解析动画
func (p *CHTLJSParser) parseAnimate() *node.ElementNode {
	pos := p.CurrentToken.Position
	p.NextToken() // 跳过 'animate'
	
	element := node.NewElementNode("animate", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	
	return element
}

// parseVirtual 解析虚拟对象
func (p *CHTLJSParser) parseVirtual() *node.ElementNode {
	pos := p.CurrentToken.Position
	p.NextToken() // 跳过 'virtual'
	
	element := node.NewElementNode("virtual", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	
	return element
}

// parseRoute 解析路由
func (p *CHTLJSParser) parseRoute() *node.ElementNode {
	pos := p.CurrentToken.Position
	p.NextToken() // 跳过 'route'
	
	path := p.CurrentToken.Literal
	p.NextToken() // 跳过路径
	
	element := node.NewElementNode("route", lexer.Position{Line: pos.Line, Column: pos.Column, Offset: pos.Offset})
	element.Attributes["path"] = &node.AttributeNode{
		Name:  "path",
		Value: path,
	}
	
	return element
}

// addError 添加错误
func (p *CHTLJSParser) addError(msg string) {
	p.Errors = append(p.Errors, msg)
}
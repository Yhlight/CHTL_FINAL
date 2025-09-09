package Test

import (
	"chtl/CHTL/lexer"
	"chtl/CHTL/parser"
	"testing"
)

func TestParser(t *testing.T) {
	input := `
use html5;

div
{
    id: main;
    class: container;
    
    text
    {
        Hello, CHTL World!
    }
    
    style
    {
        .container
        {
            width: 100%;
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }
    }
}
`

	lexer := lexer.NewLexer(input)
	parser := parser.NewParser(lexer)
	program := parser.ParseProgram()

	// 检查解析错误
	if len(parser.Errors()) > 0 {
		t.Errorf("解析错误: %v", parser.Errors())
	}

	// 检查程序结构
	if program == nil {
		t.Error("程序节点为空")
	}

	// 检查使用声明
	if program.Use == nil {
		t.Error("缺少使用声明")
	}

	// 检查元素
	if len(program.Elements) == 0 {
		t.Error("缺少元素")
	}

	// 检查第一个元素
	element := program.Elements[0]
	if element.TagName != "div" {
		t.Errorf("期望标签名 'div', 得到 '%s'", element.TagName)
	}

	// 检查属性
	if idAttr, exists := element.GetAttribute("id"); !exists || idAttr.Value != "main" {
		t.Error("缺少或错误的 id 属性")
	}

	if classAttr, exists := element.GetAttribute("class"); !exists || classAttr.Value != "container" {
		t.Error("缺少或错误的 class 属性")
	}

	// 检查文本内容
	if element.Text == nil || element.Text.Content != "Hello, CHTL World!" {
		t.Error("缺少或错误的文本内容")
	}

	// 检查样式
	if element.Style == nil {
		t.Error("缺少样式")
	}

	if len(element.Style.Selectors) == 0 {
		t.Error("缺少样式选择器")
	}
}

func TestLexer(t *testing.T) {
	input := `div { id: main; class: container; }`
	l := lexer.NewLexer(input)

	expectedTokens := []lexer.TokenType{
		lexer.IDENTIFIER,
		lexer.LEFT_BRACE,
		lexer.IDENTIFIER,
		lexer.COLON,
		lexer.IDENTIFIER,
		lexer.SEMICOLON,
		lexer.IDENTIFIER,
		lexer.COLON,
		lexer.IDENTIFIER,
		lexer.SEMICOLON,
		lexer.RIGHT_BRACE,
		lexer.EOF,
	}

	for i, expected := range expectedTokens {
		token := l.NextToken()
		if token.Type != expected {
			t.Errorf("Token %d: 期望 %s, 得到 %s", i, expected, token.Type)
		}
	}
}
package Test

import (
	"chtl/CHTL/generator"
	"chtl/CHTL/node"
	"chtl/CHTL/lexer"
	"strings"
	"testing"
)

func TestHTMLGenerator(t *testing.T) {
	// 创建测试程序
	program := node.NewProgramNode(lexer.Position{Line: 1, Column: 1})
	
	// 添加使用声明
	use := node.NewUseNode(node.USE_TYPE_HTML5, "html5", "", lexer.Position{Line: 1, Column: 1})
	program.AddUse(use)
	
	// 添加 div 元素
	div := node.NewElementNode("div", lexer.Position{Line: 3, Column: 1})
	div.AddAttribute(node.NewAttributeNode("id", "main", false, lexer.Position{Line: 4, Column: 5}))
	div.AddAttribute(node.NewAttributeNode("class", "container", false, lexer.Position{Line: 5, Column: 5}))
	
	// 添加文本内容
	text := node.NewTextNode("Hello, CHTL World!", lexer.Position{Line: 7, Column: 9})
	div.SetText(text)
	
	// 添加样式
	style := node.NewStyleNode(false, true, lexer.Position{Line: 11, Column: 5})
	style.AddProperty(node.NewStyleProperty("width", "100%", lexer.Position{Line: 13, Column: 9}))
	style.AddProperty(node.NewStyleProperty("max-width", "1200px", lexer.Position{Line: 14, Column: 9}))
	div.SetStyle(style)
	
	program.AddElement(div)
	
	// 生成 HTML
	generator := generator.NewHTMLGenerator()
	html := generator.Generate(program)
	
	// 检查生成的 HTML
	if !strings.Contains(html, "<!DOCTYPE html>") {
		t.Error("缺少 HTML5 声明")
	}
	
	if !strings.Contains(html, "<html>") {
		t.Error("缺少 html 标签")
	}
	
	if !strings.Contains(html, "<head>") {
		t.Error("缺少 head 标签")
	}
	
	if !strings.Contains(html, "<body>") {
		t.Error("缺少 body 标签")
	}
	
	if !strings.Contains(html, `<div id="main" class="container">`) {
		t.Error("缺少 div 元素或属性")
	}
	
	if !strings.Contains(html, "Hello, CHTL World!") {
		t.Error("缺少文本内容")
	}
	
	if !strings.Contains(html, "<style>") {
		t.Error("缺少样式标签")
	}
}

func TestCSSGenerator(t *testing.T) {
	// 创建测试程序
	program := node.NewProgramNode(lexer.Position{Line: 1, Column: 1})
	
	// 添加 div 元素
	div := node.NewElementNode("div", lexer.Position{Line: 1, Column: 1})
	
	// 添加样式
	style := node.NewStyleNode(false, true, lexer.Position{Line: 3, Column: 1})
	
	// 添加选择器
	selector := node.NewStyleSelector(".container", node.CLASS_SELECTOR, lexer.Position{Line: 4, Column: 1})
	selector.AddProperty(node.NewStyleProperty("width", "100%", lexer.Position{Line: 5, Column: 1}))
	selector.AddProperty(node.NewStyleProperty("max-width", "1200px", lexer.Position{Line: 6, Column: 1}))
	style.AddSelector(selector)
	
	div.SetStyle(style)
	program.AddElement(div)
	
	// 生成 CSS
	generator := generator.NewCSSGenerator()
	css := generator.Generate(program)
	
	// 检查生成的 CSS
	if !strings.Contains(css, ".container") {
		t.Error("缺少容器选择器")
	}
	
	if !strings.Contains(css, "width: 100%") {
		t.Error("缺少宽度属性")
	}
	
	if !strings.Contains(css, "max-width: 1200px") {
		t.Error("缺少最大宽度属性")
	}
}
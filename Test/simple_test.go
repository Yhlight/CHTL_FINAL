package Test

import (
	"chtl/CHTL/lexer"
	"chtl/CHTL/node"
	"testing"
)

func TestSimpleNodeCreation(t *testing.T) {
	// 测试基础节点创建
	pos := lexer.Position{Line: 1, Column: 1}
	
	// 测试元素节点
	element := node.NewElementNode("div", pos)
	if element.TagName != "div" {
		t.Errorf("期望标签名 'div', 得到 '%s'", element.TagName)
	}
	
	// 测试文本节点
	text := node.NewTextNode("Hello World", pos)
	if text.Content != "Hello World" {
		t.Errorf("期望内容 'Hello World', 得到 '%s'", text.Content)
	}
	
	// 测试属性节点
	attr := node.NewAttributeNode("id", "main", false, pos)
	if attr.Name != "id" || attr.Value != "main" {
		t.Errorf("期望属性 id=main, 得到 %s=%s", attr.Name, attr.Value)
	}
}

func TestElementOperations(t *testing.T) {
	pos := lexer.Position{Line: 1, Column: 1}
	element := node.NewElementNode("div", pos)
	
	// 测试添加属性
	attr := node.NewAttributeNode("class", "container", false, pos)
	element.AddAttribute(attr)
	
	if _, exists := element.GetAttribute("class"); !exists {
		t.Error("属性添加失败")
	}
	
	// 测试添加文本
	text := node.NewTextNode("Hello", pos)
	element.SetText(text)
	
	if element.Text == nil || element.Text.Content != "Hello" {
		t.Error("文本设置失败")
	}
}
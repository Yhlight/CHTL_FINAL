package chtljs

import (
	"chtl/CHTL/node"
	"fmt"
	"strings"
)

// CHTLJSGenerator CHTL JS 代码生成器
type CHTLJSGenerator struct {
	program *node.ProgramNode
}

// NewCHTLJSGenerator 创建 CHTL JS 代码生成器
func NewCHTLJSGenerator(program *node.ProgramNode) *CHTLJSGenerator {
	return &CHTLJSGenerator{
		program: program,
	}
}

// Generate 生成 JavaScript 代码
func (g *CHTLJSGenerator) Generate() string {
	var result strings.Builder
	
	// 生成文件头注释
	result.WriteString("// CHTL JS 生成的 JavaScript 代码\n")
	result.WriteString("// 自动生成，请勿手动修改\n\n")
	
	// 生成程序内容
	for _, element := range g.program.Elements {
		result.WriteString(g.generateElement(element))
		result.WriteString("\n")
	}
	
	return result.String()
}

// generateElement 生成元素代码
func (g *CHTLJSGenerator) generateElement(element *node.ElementNode) string {
	switch element.TagName {
	case "function":
		return g.generateFunction(element)
	case "variable":
		return g.generateVariable(element)
	case "conditional":
		return g.generateConditional(element)
	case "loop":
		return g.generateLoop(element)
	case "load":
		return g.generateLoad(element)
	case "local":
		return g.generateLocal(element)
	case "select":
		return g.generateSelect(element)
	case "delegate":
		return g.generateDelegate(element)
	case "animate":
		return g.generateAnimate(element)
	case "virtual":
		return g.generateVirtual(element)
	case "route":
		return g.generateRoute(element)
	default:
		return fmt.Sprintf("// 未知元素类型: %s\n", element.TagName)
	}
}

// generateFunction 生成函数
func (g *CHTLJSGenerator) generateFunction(element *node.ElementNode) string {
	var result strings.Builder
	
	name := element.Attributes["name"].Value
	params := element.Attributes["params"].Value
	body := element.Text.Content
	
	result.WriteString(fmt.Sprintf("function %s(%s) {\n", name, params))
	result.WriteString(fmt.Sprintf("    %s\n", body))
	result.WriteString("}\n")
	
	return result.String()
}

// generateVariable 生成变量
func (g *CHTLJSGenerator) generateVariable(element *node.ElementNode) string {
	varType := element.Attributes["type"].Value
	name := element.Attributes["name"].Value
	value := element.Text.Content
	
	if value != "" {
		return fmt.Sprintf("%s %s = %s;\n", varType, name, value)
	}
	return fmt.Sprintf("%s %s;\n", varType, name)
}

// generateConditional 生成条件语句
func (g *CHTLJSGenerator) generateConditional(element *node.ElementNode) string {
	condition := element.Attributes["condition"].Value
	return fmt.Sprintf("if (%s) {\n    // 条件语句体\n}\n", condition)
}

// generateLoop 生成循环语句
func (g *CHTLJSGenerator) generateLoop(element *node.ElementNode) string {
	loopType := element.Attributes["type"].Value
	return fmt.Sprintf("%s (/* 循环条件 */) {\n    // 循环体\n}\n", loopType)
}

// generateLoad 生成文件加载
func (g *CHTLJSGenerator) generateLoad(element *node.ElementNode) string {
	file := element.Attributes["file"].Value
	return fmt.Sprintf("loadFile(\"%s\", function(data) {\n    console.log(\"文件加载完成:\", data);\n});\n", file)
}

// generateLocal 生成本地脚本
func (g *CHTLJSGenerator) generateLocal(element *node.ElementNode) string {
	body := element.Text.Content
	return fmt.Sprintf("// 本地脚本\n%s\n", body)
}

// generateSelect 生成选择器
func (g *CHTLJSGenerator) generateSelect(element *node.ElementNode) string {
	selector := element.Attributes["selector"].Value
	return fmt.Sprintf("const elements = select('%s');\n", selector)
}

// generateDelegate 生成事件委托
func (g *CHTLJSGenerator) generateDelegate(element *node.ElementNode) string {
	event := element.Attributes["event"].Value
	selector := element.Attributes["selector"].Value
	return fmt.Sprintf("delegate('%s', '%s', function(event) {\n    // 事件处理\n});\n", event, selector)
}

// generateAnimate 生成动画
func (g *CHTLJSGenerator) generateAnimate(element *node.ElementNode) string {
	return "animate('.element', {\n    duration: 1000,\n    properties: {\n        opacity: 1,\n        transform: 'translateY(0)'\n    }\n});\n"
}

// generateVirtual 生成虚拟对象
func (g *CHTLJSGenerator) generateVirtual(element *node.ElementNode) string {
	return "const virtualObj = {\n    // 虚拟对象属性\n};\n"
}

// generateRoute 生成路由
func (g *CHTLJSGenerator) generateRoute(element *node.ElementNode) string {
	path := element.Attributes["path"].Value
	return fmt.Sprintf("route('%s', function() {\n    // 路由处理\n});\n", path)
}
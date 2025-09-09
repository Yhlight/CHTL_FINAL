package generator

import (
	"chtl/CHTL/node"
	"fmt"
	"strings"
)

// CSSGenerator CSS 代码生成器
type CSSGenerator struct {
	output strings.Builder
	indent int
}

// NewCSSGenerator 创建 CSS 生成器
func NewCSSGenerator() *CSSGenerator {
	return &CSSGenerator{
		output: strings.Builder{},
		indent: 0,
	}
}

// Generate 生成 CSS 代码
func (g *CSSGenerator) Generate(program *node.ProgramNode) string {
	g.output.Reset()
	g.indent = 0

	// 生成全局样式
	g.generateGlobalStyles(program)

	// 生成局部样式
	g.generateLocalStyles(program)

	return g.output.String()
}

// generateGlobalStyles 生成全局样式
func (g *CSSGenerator) generateGlobalStyles(program *node.ProgramNode) {
	// 从所有元素中收集样式
	styles := make(map[string]map[string]string)
	g.collectGlobalStyles(program.Elements, styles)

	// 生成样式规则
	for selector, properties := range styles {
		g.writeLine(fmt.Sprintf("%s {", selector))
		g.indent++
		for prop, value := range properties {
			g.writeLine(fmt.Sprintf("%s: %s;", prop, value))
		}
		g.indent--
		g.writeLine("}")
		g.writeLine("")
	}
}

// generateLocalStyles 生成局部样式
func (g *CSSGenerator) generateLocalStyles(program *node.ProgramNode) {
	// 从局部样式块中收集样式
	styles := make(map[string]map[string]string)
	g.collectLocalStyles(program.Elements, styles)

	// 生成样式规则
	for selector, properties := range styles {
		g.writeLine(fmt.Sprintf("%s {", selector))
		g.indent++
		for prop, value := range properties {
			g.writeLine(fmt.Sprintf("%s: %s;", prop, value))
		}
		g.indent--
		g.writeLine("}")
		g.writeLine("")
	}
}

// collectGlobalStyles 收集全局样式
func (g *CSSGenerator) collectGlobalStyles(elements []*node.ElementNode, styles map[string]map[string]string) {
	for _, element := range elements {
		// 处理元素的样式
		if element.Style != nil && !element.Style.IsInline {
			for _, selector := range element.Style.Selectors {
				if styles[selector.Selector] == nil {
					styles[selector.Selector] = make(map[string]string)
				}
				for _, prop := range selector.Properties {
					styles[selector.Selector][prop.Name] = prop.Value
				}
			}
		}

		// 递归处理子元素
		for _, child := range element.Children() {
			if childElement, ok := child.(*node.ElementNode); ok {
				g.collectGlobalStyles([]*node.ElementNode{childElement}, styles)
			}
		}
	}
}

// collectLocalStyles 收集局部样式
func (g *CSSGenerator) collectLocalStyles(elements []*node.ElementNode, styles map[string]map[string]string) {
	for _, element := range elements {
		// 处理局部样式块
		if element.Style != nil && element.Style.IsLocal {
			for _, selector := range element.Style.Selectors {
				// 生成选择器名称
				selectorName := g.generateSelectorName(element, selector)
				if styles[selectorName] == nil {
					styles[selectorName] = make(map[string]string)
				}
				for _, prop := range selector.Properties {
					styles[selectorName][prop.Name] = prop.Value
				}
			}
		}

		// 递归处理子元素
		for _, child := range element.Children() {
			if childElement, ok := child.(*node.ElementNode); ok {
				g.collectLocalStyles([]*node.ElementNode{childElement}, styles)
			}
		}
	}
}

// generateSelectorName 生成选择器名称
func (g *CSSGenerator) generateSelectorName(element *node.ElementNode, selector *node.StyleSelector) string {
	switch selector.Type {
	case node.CLASS_SELECTOR:
		className := strings.TrimPrefix(selector.Selector, ".")
		return fmt.Sprintf(".%s", className)
	case node.ID_SELECTOR:
		idName := strings.TrimPrefix(selector.Selector, "#")
		return fmt.Sprintf("#%s", idName)
	case node.PSEUDO_SELECTOR:
		return fmt.Sprintf("%s:%s", element.TagName, selector.Selector)
	case node.PSEUDO_ELEMENT_SELECTOR:
		return fmt.Sprintf("%s::%s", element.TagName, selector.Selector)
	case node.ELEMENT_SELECTOR:
		return element.TagName
	case node.DESCENDANT_SELECTOR:
		return fmt.Sprintf("%s %s", element.TagName, selector.Selector)
	case node.CONTEXT_SELECTOR:
		return fmt.Sprintf("%s&", element.TagName)
	default:
		return selector.Selector
	}
}

// generateConditionalStyles 生成条件样式
func (g *CSSGenerator) generateConditionalStyles(prop *node.StyleProperty) string {
	if prop.Condition == nil {
		return prop.Value
	}

	// 生成条件表达式
	condition := prop.Condition.Condition
	trueValue := prop.Condition.TrueValue
	falseValue := prop.Condition.FalseValue

	// 简单的条件处理，实际实现需要更复杂的逻辑
	if condition != "" {
		return fmt.Sprintf("/* 条件样式: %s ? %s : %s */", condition, trueValue, falseValue)
	}

	return prop.Value
}

// generateTemplateStyles 生成模板样式
func (g *CSSGenerator) generateTemplateStyles(templates []*node.TemplateNode) {
	for _, template := range templates {
		if template.TemplateType == node.TEMPLATE_TYPE_STYLE {
			g.writeLine(fmt.Sprintf("/* 模板样式: %s */", template.Name))
			for _, content := range template.Content {
				if styleNode, ok := content.(*node.StyleNode); ok {
					g.generateStyleNode(styleNode)
				}
			}
		}
	}
}

// generateStyleNode 生成样式节点
func (g *CSSGenerator) generateStyleNode(style *node.StyleNode) {
	// 生成属性
	for _, prop := range style.Properties {
		value := g.generateConditionalStyles(prop)
		g.writeLine(fmt.Sprintf("%s: %s;", prop.Name, value))
	}

	// 生成选择器
	for _, selector := range style.Selectors {
		g.writeLine(fmt.Sprintf("%s {", selector.Selector))
		g.indent++
		for _, prop := range selector.Properties {
			value := g.generateConditionalStyles(prop)
			g.writeLine(fmt.Sprintf("%s: %s;", prop.Name, value))
		}
		g.indent--
		g.writeLine("}")
	}
}

// write 写入内容
func (g *CSSGenerator) write(content string) {
	g.output.WriteString(content)
}

// writeLine 写入一行内容
func (g *CSSGenerator) writeLine(content string) {
	for i := 0; i < g.indent; i++ {
		g.output.WriteString("  ")
	}
	g.output.WriteString(content)
	g.output.WriteString("\n")
}
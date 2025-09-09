package generator

import (
	"chtl/CHTL/node"
	"fmt"
	"strings"
)

// HTMLGenerator HTML 代码生成器
type HTMLGenerator struct {
	output strings.Builder
	indent int
}

// NewHTMLGenerator 创建 HTML 生成器
func NewHTMLGenerator() *HTMLGenerator {
	return &HTMLGenerator{
		output: strings.Builder{},
		indent: 0,
	}
}

// Generate 生成 HTML 代码
func (g *HTMLGenerator) Generate(program *node.ProgramNode) string {
	g.output.Reset()
	g.indent = 0

	// 生成 HTML5 声明
	g.writeLine("<!DOCTYPE html>")
	g.writeLine("<html>")

	// 生成 head
	g.writeLine("<head>")
	g.indent++
	g.generateHead(program)
	g.indent--
	g.writeLine("</head>")

	// 生成 body
	g.writeLine("<body>")
	g.indent++
	g.generateBody(program)
	g.indent--
	g.writeLine("</body>")

	g.writeLine("</html>")

	return g.output.String()
}

// generateHead 生成 head 部分
func (g *HTMLGenerator) generateHead(program *node.ProgramNode) {
	// 生成 meta 标签
	g.writeLine("<meta charset=\"UTF-8\">")
	g.writeLine("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")

	// 生成导入的 CSS
	for _, imp := range program.Imports {
		if imp.ImportType == node.IMPORT_TYPE_CSS {
			g.writeLine(fmt.Sprintf("<link rel=\"stylesheet\" href=\"%s\">", imp.Path))
		}
	}

	// 生成导入的 JS
	for _, imp := range program.Imports {
		if imp.ImportType == node.IMPORT_TYPE_JS {
			g.writeLine(fmt.Sprintf("<script src=\"%s\"></script>", imp.Path))
		}
	}

	// 生成内联样式
	g.generateInlineStyles(program)
}

// generateBody 生成 body 部分
func (g *HTMLGenerator) generateBody(program *node.ProgramNode) {
	for _, element := range program.Elements {
		g.generateElement(element)
	}
}

// generateElement 生成元素
func (g *HTMLGenerator) generateElement(element *node.ElementNode) {
	// 生成开始标签
	g.write(fmt.Sprintf("<%s", element.TagName))

	// 生成属性
	for _, attr := range element.Attributes {
		if attr.IsLiteral {
			g.write(fmt.Sprintf(" %s=\"%s\"", attr.Name, attr.Value))
		} else {
			g.write(fmt.Sprintf(" %s=%s", attr.Name, attr.Value))
		}
	}

	// 生成自动添加的类名和ID
	if element.Style != nil {
		for _, selector := range element.Style.Selectors {
			if selector.Type == node.CLASS_SELECTOR {
				className := strings.TrimPrefix(selector.Selector, ".")
				g.write(fmt.Sprintf(" class=\"%s\"", className))
			} else if selector.Type == node.ID_SELECTOR {
				idName := strings.TrimPrefix(selector.Selector, "#")
				g.write(fmt.Sprintf(" id=\"%s\"", idName))
			}
		}
	}

	// 生成内联样式
	if element.Style != nil && element.Style.IsInline {
		g.write(" style=\"")
		g.generateInlineStyleProperties(element.Style)
		g.write("\"")
	}

	// 检查是否为自闭合标签
	if element.IsSelfClosing() {
		g.write(" />")
		g.writeLine("")
		return
	}

	g.write(">")
	g.writeLine("")

	// 生成文本内容
	if element.Text != nil {
		g.indent++
		g.writeLine(element.Text.Content)
		g.indent--
	}

	// 生成子元素
	for _, child := range element.Children() {
		if childElement, ok := child.(*node.ElementNode); ok {
			g.indent++
			g.generateElement(childElement)
			g.indent--
		}
	}

	// 生成结束标签
	g.writeLine(fmt.Sprintf("</%s>", element.TagName))
}

// generateInlineStyles 生成内联样式
func (g *HTMLGenerator) generateInlineStyles(program *node.ProgramNode) {
	// 收集所有样式
	styles := make(map[string]string)

	// 从元素中收集样式
	g.collectStylesFromElements(program.Elements, styles)

	// 生成 style 标签
	if len(styles) > 0 {
		g.writeLine("<style>")
		for selector, properties := range styles {
			g.writeLine(fmt.Sprintf("%s {", selector))
			g.indent++
			g.writeLine(properties)
			g.indent--
			g.writeLine("}")
		}
		g.writeLine("</style>")
	}
}

// collectStylesFromElements 从元素中收集样式
func (g *HTMLGenerator) collectStylesFromElements(elements []*node.ElementNode, styles map[string]string) {
	for _, element := range elements {
		if element.Style != nil {
			for _, selector := range element.Style.Selectors {
				var properties strings.Builder
				for _, prop := range selector.Properties {
					properties.WriteString(fmt.Sprintf("  %s: %s;\n", prop.Name, prop.Value))
				}
				styles[selector.Selector] = properties.String()
			}
		}
		// 递归处理子元素
		for _, child := range element.Children() {
			if childElement, ok := child.(*node.ElementNode); ok {
				g.collectStylesFromElements([]*node.ElementNode{childElement}, styles)
			}
		}
	}
}

// generateInlineStyleProperties 生成内联样式属性
func (g *HTMLGenerator) generateInlineStyleProperties(style *node.StyleNode) {
	var properties []string
	for _, prop := range style.Properties {
		properties = append(properties, fmt.Sprintf("%s: %s", prop.Name, prop.Value))
	}
	g.write(strings.Join(properties, "; "))
}

// write 写入内容
func (g *HTMLGenerator) write(content string) {
	g.output.WriteString(content)
}

// writeLine 写入一行内容
func (g *HTMLGenerator) writeLine(content string) {
	for i := 0; i < g.indent; i++ {
		g.output.WriteString("  ")
	}
	g.output.WriteString(content)
	g.output.WriteString("\n")
}
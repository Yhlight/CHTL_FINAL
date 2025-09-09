package generator

import (
	"chtl/CHTL/chtljs"
	"chtl/CHTL/lexer"
	"chtl/CHTL/node"
	"fmt"
	"strings"
)

// HTMLGenerator HTML 代码生成器
type HTMLGenerator struct {
	output  strings.Builder
	indent  int
	program *node.ProgramNode
}

// NewHTMLGenerator 创建 HTML 生成器
func NewHTMLGenerator(program *node.ProgramNode) *HTMLGenerator {
	return &HTMLGenerator{
		output:  strings.Builder{},
		indent:  0,
		program: program,
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

	// 生成导入的 HTML 内容
	for _, imp := range program.Imports {
		if imp.ImportType == node.IMPORT_TYPE_HTML {
			g.writeLine(fmt.Sprintf("<!-- 导入的 HTML: %s -->", imp.Path))
		}
	}

	// 生成内联样式
	g.generateInlineStyles(program)
}

// generateBody 生成 body 部分
func (g *HTMLGenerator) generateBody(program *node.ProgramNode) {
	// 生成命名空间内容
	for _, namespace := range program.Namespaces {
		g.generateNamespace(namespace)
	}
	
	// 生成主元素
	for _, element := range program.Elements {
		g.generateElement(element)
	}
}

// generateElement 生成元素
func (g *HTMLGenerator) generateElement(element *node.ElementNode) {
	// 检查是否为模板元素
	if g.isTemplateElement(element.TagName) {
		g.generateTemplateElement(element)
		return
	}

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

	// 应用模板样式
	g.applyTemplateStyles(element)

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

	// 从模板中收集样式
	g.collectStylesFromTemplates(program.Templates, styles)

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

// collectStylesFromTemplates 从模板中收集样式
func (g *HTMLGenerator) collectStylesFromTemplates(templates []*node.TemplateNode, styles map[string]string) {
	for _, template := range templates {
		if template.TemplateType == node.TEMPLATE_TYPE_STYLE {
			// 样式模板
			var properties strings.Builder
			for _, content := range template.Content {
				if prop, ok := content.(*node.StyleProperty); ok {
					properties.WriteString(fmt.Sprintf("  %s: %s;\n", prop.Name, prop.Value))
				}
			}
			styles[template.Name] = properties.String()
		}
	}
}

// applyTemplateStyles 应用模板样式
func (g *HTMLGenerator) applyTemplateStyles(element *node.ElementNode) {
	// 根据元素标签名查找对应的模板
	for _, template := range g.program.Templates {
		if template.TemplateType == node.TEMPLATE_TYPE_STYLE && template.Name == element.TagName {
			// 应用模板样式作为内联样式
			var properties []string
			for _, content := range template.Content {
				if prop, ok := content.(*node.StyleProperty); ok {
					properties = append(properties, fmt.Sprintf("%s: %s", prop.Name, prop.Value))
				}
			}
			if len(properties) > 0 {
				g.write(fmt.Sprintf(" style=\"%s\"", strings.Join(properties, "; ")))
			}
			break
		}
	}
}

// isTemplateElement 检查是否为模板元素
func (g *HTMLGenerator) isTemplateElement(tagName string) bool {
	for _, template := range g.program.Templates {
		if template.TemplateType == node.TEMPLATE_TYPE_ELEMENT && template.Name == tagName {
			return true
		}
	}
	return false
}

// generateTemplateElement 生成模板元素
func (g *HTMLGenerator) generateTemplateElement(element *node.ElementNode) {
	// 查找对应的元素模板
	for _, template := range g.program.Templates {
		if template.TemplateType == node.TEMPLATE_TYPE_ELEMENT && template.Name == element.TagName {
			// 生成模板内容
			for _, content := range template.Content {
				if contentElement, ok := content.(*node.ElementNode); ok {
					// 生成模板元素，并应用当前元素的属性
					g.generateTemplateElementWithAttributes(contentElement, element)
				}
			}
			break
		}
	}
}

// generateTemplateElementWithAttributes 生成带属性的模板元素
func (g *HTMLGenerator) generateTemplateElementWithAttributes(templateElement *node.ElementNode, originalElement *node.ElementNode) {
	// 生成开始标签
	g.write(fmt.Sprintf("<%s", templateElement.TagName))

	// 生成模板元素的属性
	for _, attr := range templateElement.Attributes {
		if attr.IsLiteral {
			g.write(fmt.Sprintf(" %s=\"%s\"", attr.Name, attr.Value))
		} else {
			g.write(fmt.Sprintf(" %s=%s", attr.Name, attr.Value))
		}
	}

	// 应用原始元素的属性
	for _, attr := range originalElement.Attributes {
		if attr.IsLiteral {
			g.write(fmt.Sprintf(" %s=\"%s\"", attr.Name, attr.Value))
		} else {
			g.write(fmt.Sprintf(" %s=%s", attr.Name, attr.Value))
		}
	}

	// 应用模板样式
	g.applyTemplateStyles(originalElement)

	// 生成内联样式
	if templateElement.Style != nil && templateElement.Style.IsInline {
		g.write(" style=\"")
		g.generateInlineStyleProperties(templateElement.Style)
		g.write("\"")
	}

	// 检查是否为自闭合标签
	if templateElement.IsSelfClosing() {
		g.write(" />")
		g.writeLine("")
		return
	}

	g.write(">")
	g.writeLine("")

	// 生成文本内容
	if originalElement.Text != nil {
		g.indent++
		g.writeLine(originalElement.Text.Content)
		g.indent--
	}

	// 生成子元素
	for _, child := range originalElement.Children() {
		if childElement, ok := child.(*node.ElementNode); ok {
			g.indent++
			g.generateElement(childElement)
			g.indent--
		}
	}

	// 生成结束标签
	g.writeLine(fmt.Sprintf("</%s>", templateElement.TagName))
}

// generateNamespace 生成命名空间
func (g *HTMLGenerator) generateNamespace(namespace *node.NamespaceNode) {
	// 生成命名空间注释
	g.writeLine(fmt.Sprintf("<!-- 命名空间: %s -->", namespace.Name))
	
	// 生成命名空间内的模板
	for _, template := range namespace.Templates {
		g.generateTemplate(template)
	}
	
	// 生成命名空间内的自定义
	for _, custom := range namespace.Customs {
		g.generateCustom(custom)
	}
}

// generateTemplate 生成模板
func (g *HTMLGenerator) generateTemplate(template *node.TemplateNode) {
	// 简化实现：模板在编译时被应用，这里只生成注释
	g.writeLine(fmt.Sprintf("<!-- 模板: %s (%s) -->", template.Name, template.TemplateType))
}

// generateCustom 生成自定义
func (g *HTMLGenerator) generateCustom(custom *node.CustomNode) {
	// 简化实现：自定义在编译时被应用，这里只生成注释
	g.writeLine(fmt.Sprintf("<!-- 自定义: %s (%s) -->", custom.Name, custom.CustomType))
}

// generateScript 生成脚本
func (g *HTMLGenerator) generateScript(script *node.ScriptNode) string {
	if script == nil {
		return ""
	}
	
	var result strings.Builder
	
	// 生成脚本标签
	result.WriteString("<script>\n")
	
	// 生成脚本内容
	if script.Content != "" {
		// 检查是否是 CHTL JS 代码
		if g.isCHTLJSCode(script.Content) {
			// 使用 CHTL JS 编译器
			chtljsProgram := g.parseCHTLJS(script.Content)
			chtljsGenerator := chtljs.NewCHTLJSGenerator(chtljsProgram)
			result.WriteString(chtljsGenerator.Generate())
		} else {
			// 普通 JavaScript 代码
			result.WriteString(script.Content)
		}
	}
	
	result.WriteString("\n</script>\n")
	
	return result.String()
}

// isCHTLJSCode 检查是否是 CHTL JS 代码
func (g *HTMLGenerator) isCHTLJSCode(content string) bool {
	// 简单的检查：如果包含 CHTL JS 关键字，则认为是 CHTL JS 代码
	chtljsKeywords := []string{"function", "load", "local", "select", "delegate", "animate", "virtual", "route"}
	for _, keyword := range chtljsKeywords {
		if strings.Contains(content, keyword) {
			return true
		}
	}
	return false
}

// parseCHTLJS 解析 CHTL JS 代码
func (g *HTMLGenerator) parseCHTLJS(content string) *node.ProgramNode {
	// 这里应该使用 CHTL JS 解析器
	// 为了简化，我们返回一个空的程序节点
	return node.NewProgramNode(lexer.Position{Line: 1, Column: 1})
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
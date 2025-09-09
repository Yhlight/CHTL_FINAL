package import

import (
	"chtl/CHTL/lexer"
	"chtl/CHTL/node"
	"chtl/CHTL/parser"
	"fmt"
	"io/ioutil"
	"path/filepath"
)

// ImportManager 导入管理器
type ImportManager struct {
	workingDir string
	imported   map[string]*node.ProgramNode
}

// NewImportManager 创建导入管理器
func NewImportManager(workingDir string) *ImportManager {
	return &ImportManager{
		workingDir: workingDir,
		imported:   make(map[string]*node.ProgramNode),
	}
}

// ImportFile 导入文件
func (im *ImportManager) ImportFile(importNode *node.ImportNode) (*node.ProgramNode, error) {
	// 检查是否已经导入
	if program, exists := im.imported[importNode.Path]; exists {
		return program, nil
	}

	// 构建文件路径
	filePath := filepath.Join(im.workingDir, importNode.Path)
	
	// 读取文件内容
	content, err := ioutil.ReadFile(filePath)
	if err != nil {
		return nil, fmt.Errorf("无法读取文件 %s: %v", filePath, err)
	}

	// 根据文件类型解析
	switch importNode.ImportType {
	case node.IMPORT_TYPE_HTML:
		return im.parseHTMLFile(string(content), importNode.Path)
	case node.IMPORT_TYPE_CSS:
		return im.parseCSSFile(string(content), importNode.Path)
	case node.IMPORT_TYPE_JS:
		return im.parseJSFile(string(content), importNode.Path)
	case node.IMPORT_TYPE_CHTL:
		return im.parseCHTLFile(string(content), importNode.Path)
	case node.IMPORT_TYPE_CJMOD:
		return im.parseCJMODFile(string(content), importNode.Path)
	default:
		return nil, fmt.Errorf("不支持的导入类型: %d", importNode.ImportType)
	}
}

// parseHTMLFile 解析 HTML 文件
func (im *ImportManager) parseHTMLFile(content, path string) (*node.ProgramNode, error) {
	// 简化实现：创建包含 HTML 内容的程序节点
	program := node.NewProgramNode(lexer.Position{Line: 1, Column: 1})
	
	// 创建文本节点包含 HTML 内容
	textNode := node.NewTextNode(content, lexer.Position{Line: 1, Column: 1})
	
	// 创建包装元素
	element := node.NewElementNode("html", lexer.Position{Line: 1, Column: 1})
	element.SetText(textNode)
	program.AddElement(element)
	
	im.imported[path] = program
	return program, nil
}

// parseCSSFile 解析 CSS 文件
func (im *ImportManager) parseCSSFile(content, path string) (*node.ProgramNode, error) {
	// 简化实现：创建包含 CSS 内容的程序节点
	program := node.NewProgramNode(lexer.Position{Line: 1, Column: 1})
	
	// 创建样式节点
	styleNode := node.NewStyleNode(false, true, lexer.Position{Line: 1, Column: 1})
	styleNode.Content = content
	
	// 创建包装元素
	element := node.NewElementNode("style", lexer.Position{Line: 1, Column: 1})
	element.SetStyle(styleNode)
	program.AddElement(element)
	
	im.imported[path] = program
	return program, nil
}

// parseJSFile 解析 JS 文件
func (im *ImportManager) parseJSFile(content, path string) (*node.ProgramNode, error) {
	// 简化实现：创建包含 JS 内容的程序节点
	program := node.NewProgramNode(lexer.Position{Line: 1, Column: 1})
	
	// 创建脚本节点
	scriptNode := node.NewScriptNode(true, true, lexer.Position{Line: 1, Column: 1})
	scriptNode.Content = content
	
	// 创建包装元素
	element := node.NewElementNode("script", lexer.Position{Line: 1, Column: 1})
	element.SetScript(scriptNode)
	program.AddElement(element)
	
	im.imported[path] = program
	return program, nil
}

// parseCHTLFile 解析 CHTL 文件
func (im *ImportManager) parseCHTLFile(content, path string) (*node.ProgramNode, error) {
	// 使用 CHTL 解析器解析文件
	lexer := lexer.NewLexer(content)
	chtlParser := parser.NewParserV3(lexer)
	program := chtlParser.ParseProgram()
	
	// 检查解析错误
	if len(chtlParser.Errors()) > 0 {
		return nil, fmt.Errorf("解析 CHTL 文件 %s 失败: %v", path, chtlParser.Errors())
	}
	
	im.imported[path] = program
	return program, nil
}

// parseCJMODFile 解析 CJMOD 文件
func (im *ImportManager) parseCJMODFile(content, path string) (*node.ProgramNode, error) {
	// 简化实现：CJMOD 文件解析
	program := node.NewProgramNode(lexer.Position{Line: 1, Column: 1})
	
	// 创建脚本节点
	scriptNode := node.NewScriptNode(true, true, lexer.Position{Line: 1, Column: 1})
	scriptNode.Content = content
	
	// 创建包装元素
	element := node.NewElementNode("script", lexer.Position{Line: 1, Column: 1})
	element.SetScript(scriptNode)
	program.AddElement(element)
	
	im.imported[path] = program
	return program, nil
}

// GetImportedProgram 获取已导入的程序
func (im *ImportManager) GetImportedProgram(path string) (*node.ProgramNode, bool) {
	program, exists := im.imported[path]
	return program, exists
}
package CompilerDispatcher

import (
	"chtl/CHTL/context"
	"chtl/CHTL/generator"
	"chtl/CHTL/lexer"
	"chtl/CHTL/parser"
	"chtl/Scanner"
	"fmt"
	"os"
	"path/filepath"
	"time"
)

// Dispatcher 编译器调度器
type Dispatcher struct {
	ctx           *context.Context
	scanner       *Scanner.UnifiedScanner
	chtlParser    *parser.Parser
	htmlGenerator *generator.HTMLGenerator
	cssGenerator  *generator.CSSGenerator
}

// NewDispatcher 创建编译器调度器
func NewDispatcher(ctx *context.Context) *Dispatcher {
	return &Dispatcher{
		ctx:           ctx,
		scanner:       Scanner.NewUnifiedScanner(ctx),
		htmlGenerator: generator.NewHTMLGenerator(),
		cssGenerator:  generator.NewCSSGenerator(),
	}
}

// Compile 编译文件
func (d *Dispatcher) Compile(inputFile, outputFile string) error {
	// 设置工作目录
	workingDir := filepath.Dir(inputFile)
	d.ctx.SetWorkingDir(workingDir)

	// 设置输出目录
	if outputFile != "" {
		outputDir := filepath.Dir(outputFile)
		d.ctx.SetOutputDir(outputDir)
	}

	// 读取输入文件
	content, err := d.readFile(inputFile)
	if err != nil {
		return fmt.Errorf("读取文件失败: %v", err)
	}

	// 使用统一扫描器分离代码
	segments, err := d.scanner.Scan(content)
	if err != nil {
		return fmt.Errorf("代码分离失败: %v", err)
	}

	// 解析 CHTL 代码
	chtlLexer := lexer.NewLexer(segments.CHTL)
	chtlParser := parser.NewParser(chtlLexer)
	program := chtlParser.ParseProgram()

	// 检查解析错误
	if len(chtlParser.Errors()) > 0 {
		for _, err := range chtlParser.Errors() {
			d.ctx.ErrorLog("解析错误: %s", err)
		}
		return fmt.Errorf("解析失败")
	}

	// 生成 HTML
	htmlContent := d.htmlGenerator.Generate(program)

	// 生成 CSS
	cssContent := d.cssGenerator.Generate(program)

	// 合并代码
	finalHTML := d.mergeCode(htmlContent, cssContent, segments.CSS, segments.JS)

	// 写入输出文件
	if outputFile != "" {
		err = d.writeFile(outputFile, finalHTML)
		if err != nil {
			return fmt.Errorf("写入文件失败: %v", err)
		}
	} else {
		// 默认输出到控制台
		fmt.Println(finalHTML)
	}

	return nil
}

// WatchAndCompile 监听并编译
func (d *Dispatcher) WatchAndCompile(inputFile, outputFile string) error {
	// 首次编译
	err := d.Compile(inputFile, outputFile)
	if err != nil {
		return err
	}

	fmt.Printf("监听文件变化: %s\n", inputFile)
	fmt.Println("按 Ctrl+C 停止监听")

	// 简单的轮询监听
	for {
		time.Sleep(1 * time.Second)
		// 检查文件修改时间
		stat, err := os.Stat(inputFile)
		if err != nil {
			continue
		}
		
		// 这里可以实现更复杂的文件监听逻辑
		// 暂时只是简单的轮询
		fmt.Printf("检查文件: %s (修改时间: %v)\n", inputFile, stat.ModTime())
	}
}

// mergeCode 合并代码
func (d *Dispatcher) mergeCode(html, css, additionalCSS, js string) string {
	result := html

	// 在 head 中插入 CSS
	if css != "" || additionalCSS != "" {
		cssContent := css
		if additionalCSS != "" {
			if cssContent != "" {
				cssContent += "\n" + additionalCSS
			} else {
				cssContent = additionalCSS
			}
		}
		result = d.insertCSS(result, cssContent)
	}

	// 在 body 末尾插入 JS
	if js != "" {
		result = d.insertJS(result, js)
	}

	return result
}

// insertCSS 插入 CSS
func (d *Dispatcher) insertCSS(html, css string) string {
	// 在 </head> 前插入 <style> 标签
	styleTag := fmt.Sprintf("<style>\n%s\n</style>\n", css)
	return fmt.Sprintf("%s%s</head>", html[:len(html)-7], styleTag)
}

// insertJS 插入 JS
func (d *Dispatcher) insertJS(html, js string) string {
	// 在 </body> 前插入 <script> 标签
	scriptTag := fmt.Sprintf("<script>\n%s\n</script>\n", js)
	return fmt.Sprintf("%s%s</body>", html[:len(html)-7], scriptTag)
}

// readFile 读取文件
func (d *Dispatcher) readFile(filename string) (string, error) {
	content, err := os.ReadFile(filename)
	if err != nil {
		return "", err
	}
	return string(content), nil
}

// writeFile 写入文件
func (d *Dispatcher) writeFile(filename string, content string) error {
	// 确保目录存在
	dir := filepath.Dir(filename)
	err := os.MkdirAll(dir, 0755)
	if err != nil {
		return err
	}

	return os.WriteFile(filename, []byte(content), 0644)
}

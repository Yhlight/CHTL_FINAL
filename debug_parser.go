package main

import (
	"chtl/CHTL/lexer"
	"chtl/CHTL/parser"
	"fmt"
	"io/ioutil"
)

func main() {
	// 读取示例文件
	content, err := ioutil.ReadFile("Test/example.chtl")
	if err != nil {
		fmt.Printf("读取文件失败: %v\n", err)
		return
	}

	// 创建词法分析器
	l := lexer.NewLexer(string(content))
	
	// 创建解析器
	p := parser.NewParserV3(l)
	
	// 解析程序
	program := p.ParseProgram()
	
	// 检查错误
	if len(p.Errors()) > 0 {
		fmt.Println("解析错误:")
		for i, err := range p.Errors() {
			fmt.Printf("%d. %s\n", i+1, err)
		}
	} else {
		fmt.Println("解析成功!")
		fmt.Printf("程序包含 %d 个元素\n", len(program.Elements))
	}
}
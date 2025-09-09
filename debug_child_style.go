package main

import (
	"chtl/CHTL/lexer"
	"chtl/CHTL/node"
	"chtl/CHTL/parser"
	"fmt"
	"io/ioutil"
)

func main() {
	// 读取示例文件
	content, err := ioutil.ReadFile("Test/simple_child.chtl")
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
		
		// 检查第一个元素
		if len(program.Elements) > 0 {
			element := program.Elements[0]
			fmt.Printf("第一个元素: %s\n", element.TagName)
			fmt.Printf("子元素数量: %d\n", len(element.Children()))
			
			// 检查子元素
			for i, child := range element.Children() {
				if childElement, ok := child.(*node.ElementNode); ok {
					fmt.Printf("子元素 %d: %s\n", i, childElement.TagName)
					if childElement.Style != nil {
						fmt.Printf("  样式选择器数量: %d\n", len(childElement.Style.Selectors))
						fmt.Printf("  样式属性数量: %d\n", len(childElement.Style.Properties))
					}
				}
			}
		}
	}
}
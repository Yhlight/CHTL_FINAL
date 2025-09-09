package main

import (
	"chtl/CHTL/lexer"
	"chtl/CHTL/parser"
	"fmt"
	"io/ioutil"
)

func main() {
	// 读取示例文件
	content, err := ioutil.ReadFile("Test/use_only.chtl")
	if err != nil {
		fmt.Printf("读取文件失败: %v\n", err)
		return
	}

	// 创建词法分析器
	l := lexer.NewLexer(string(content))
	
	// 创建解析器
	p := parser.NewParserV3(l)
	
	// 手动解析 use 语句
	fmt.Println("开始解析...")
	fmt.Printf("当前token: %s, 字面值: '%s'\n", p.CurrentToken.Type, p.CurrentToken.Literal)
	
	if p.CurrentToken.Type == lexer.KEYWORD_USE {
		fmt.Println("解析 use 语句")
		p.NextToken()
		fmt.Printf("当前token: %s, 字面值: '%s'\n", p.CurrentToken.Type, p.CurrentToken.Literal)
		
		if p.CurrentToken.Type == lexer.KEYWORD_HTML5 {
			fmt.Println("找到 html5")
			p.NextToken()
			fmt.Printf("当前token: %s, 字面值: '%s'\n", p.CurrentToken.Type, p.CurrentToken.Literal)
		}
		
		if p.CurrentToken.Type == lexer.SEMICOLON {
			fmt.Println("找到分号")
			p.NextToken()
			fmt.Printf("当前token: %s, 字面值: '%s'\n", p.CurrentToken.Type, p.CurrentToken.Literal)
		}
	}
	
	// 检查错误
	if len(p.Errors()) > 0 {
		fmt.Println("解析错误:")
		for i, err := range p.Errors() {
			fmt.Printf("%d. %s\n", i+1, err)
		}
	} else {
		fmt.Println("解析成功!")
	}
}
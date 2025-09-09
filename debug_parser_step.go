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
	
	// 手动解析第一个元素
	fmt.Println("开始解析...")
	
	// 解析 use 语句
	if p.CurrentToken.Type == lexer.KEYWORD_USE {
		fmt.Println("解析 use 语句")
		p.NextToken()
		if p.CurrentToken.Type == lexer.KEYWORD_HTML5 {
			fmt.Println("找到 html5")
			p.NextToken()
		}
		if p.CurrentToken.Type == lexer.SEMICOLON {
			fmt.Println("找到分号")
			p.NextToken()
		}
	}
	
	// 解析 div 元素
	if p.CurrentToken.Type == lexer.IDENTIFIER && p.CurrentToken.Literal == "div" {
		fmt.Println("解析 div 元素")
		p.NextToken()
		if p.CurrentToken.Type == lexer.LEFT_BRACE {
			fmt.Println("找到左大括号")
			p.NextToken()
		}
		
		// 解析属性
		for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
			fmt.Printf("当前token: %s, 字面值: '%s'\n", p.CurrentToken.Type, p.CurrentToken.Literal)
			if p.CurrentToken.Type == lexer.IDENTIFIER {
				if p.PeekToken.Type == lexer.COLON {
					fmt.Println("解析属性")
					p.NextToken() // 跳过属性名
					p.NextToken() // 跳过冒号
					p.NextToken() // 跳过属性值
					if p.CurrentToken.Type == lexer.SEMICOLON {
						fmt.Println("找到属性分号")
						p.NextToken()
					}
				} else if p.PeekToken.Type == lexer.LEFT_BRACE {
					fmt.Printf("解析子元素: %s\n", p.CurrentToken.Literal)
					p.NextToken() // 跳过元素名
					if p.CurrentToken.Type == lexer.LEFT_BRACE {
						fmt.Println("找到子元素左大括号")
						p.NextToken()
						for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
							if p.CurrentToken.Type == lexer.KEYWORD_TEXT {
								fmt.Println("解析子元素文本")
								p.NextToken() // 跳过 text 关键字
								if p.CurrentToken.Type == lexer.LEFT_BRACE {
									fmt.Println("找到子元素文本左大括号")
									p.NextToken()
									for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
										fmt.Printf("子元素文本内容: %s\n", p.CurrentToken.Literal)
										p.NextToken()
									}
									if p.CurrentToken.Type == lexer.RIGHT_BRACE {
										fmt.Println("找到子元素文本右大括号")
										p.NextToken()
									}
								}
							} else if p.CurrentToken.Type == lexer.KEYWORD_STYLE {
								fmt.Println("解析子元素样式")
								p.NextToken() // 跳过 style 关键字
								if p.CurrentToken.Type == lexer.LEFT_BRACE {
									fmt.Println("找到子元素样式左大括号")
									p.NextToken()
									for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
										fmt.Printf("子元素样式内容: %s\n", p.CurrentToken.Literal)
										p.NextToken()
									}
									if p.CurrentToken.Type == lexer.RIGHT_BRACE {
										fmt.Println("找到子元素样式右大括号")
										p.NextToken()
									}
								}
							} else {
								fmt.Printf("子元素内容: %s\n", p.CurrentToken.Literal)
								p.NextToken()
							}
						}
						if p.CurrentToken.Type == lexer.RIGHT_BRACE {
							fmt.Println("找到子元素右大括号")
							p.NextToken()
						}
					}
				} else {
					fmt.Println("意外的标识符")
					p.NextToken()
				}
			} else if p.CurrentToken.Type == lexer.KEYWORD_TEXT {
				fmt.Println("解析文本")
				p.NextToken() // 跳过 text 关键字
				if p.CurrentToken.Type == lexer.LEFT_BRACE {
					fmt.Println("找到文本左大括号")
					p.NextToken()
					for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
						fmt.Printf("文本内容: %s\n", p.CurrentToken.Literal)
						p.NextToken()
					}
					if p.CurrentToken.Type == lexer.RIGHT_BRACE {
						fmt.Println("找到文本右大括号")
						p.NextToken()
					}
				}
			} else if p.CurrentToken.Type == lexer.KEYWORD_STYLE {
				fmt.Println("解析样式")
				p.NextToken() // 跳过 style 关键字
				if p.CurrentToken.Type == lexer.LEFT_BRACE {
					fmt.Println("找到样式左大括号")
					p.NextToken()
					for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
						if p.CurrentToken.Type == lexer.DOT || p.CurrentToken.Type == lexer.HASH {
							fmt.Printf("样式选择器: %s\n", p.CurrentToken.Literal)
							p.NextToken()
							if p.CurrentToken.Type == lexer.IDENTIFIER {
								fmt.Printf("选择器名称: %s\n", p.CurrentToken.Literal)
								p.NextToken()
							}
							if p.CurrentToken.Type == lexer.LEFT_BRACE {
								fmt.Println("找到选择器左大括号")
								p.NextToken()
								for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
									fmt.Printf("选择器内容: %s\n", p.CurrentToken.Literal)
									p.NextToken()
								}
								if p.CurrentToken.Type == lexer.RIGHT_BRACE {
									fmt.Println("找到选择器右大括号")
									p.NextToken()
								}
							}
						} else if p.CurrentToken.Type == lexer.AMPERSAND {
							fmt.Printf("伪选择器: %s\n", p.CurrentToken.Literal)
							p.NextToken()
							if p.CurrentToken.Type == lexer.COLON {
								p.NextToken()
								if p.CurrentToken.Type == lexer.IDENTIFIER {
									fmt.Printf("伪选择器名称: %s\n", p.CurrentToken.Literal)
									p.NextToken()
								}
							}
							if p.CurrentToken.Type == lexer.LEFT_BRACE {
								fmt.Println("找到伪选择器左大括号")
								p.NextToken()
								for p.CurrentToken.Type != lexer.RIGHT_BRACE && p.CurrentToken.Type != lexer.EOF {
									fmt.Printf("伪选择器内容: %s\n", p.CurrentToken.Literal)
									p.NextToken()
								}
								if p.CurrentToken.Type == lexer.RIGHT_BRACE {
									fmt.Println("找到伪选择器右大括号")
									p.NextToken()
								}
							}
						} else {
							fmt.Printf("样式内容: %s\n", p.CurrentToken.Literal)
							p.NextToken()
						}
					}
					if p.CurrentToken.Type == lexer.RIGHT_BRACE {
						fmt.Println("找到样式右大括号")
						p.NextToken()
					}
				}
			} else {
				fmt.Printf("意外的token: %s\n", p.CurrentToken.Type)
				p.NextToken()
			}
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
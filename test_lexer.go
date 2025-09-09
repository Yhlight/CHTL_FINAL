package main

import (
	"chtl/CHTL/lexer"
	"fmt"
)

func main() {
	input := "100%;"
	l := lexer.NewLexer(input)
	
	for i := 0; i < 5; i++ {
		token := l.NextToken()
		fmt.Printf("Token: %s, Literal: '%s', Position: %d:%d\n", 
			token.Type, token.Literal, token.Position.Line, token.Position.Column)
		if token.Type == lexer.EOF {
			break
		}
	}
}
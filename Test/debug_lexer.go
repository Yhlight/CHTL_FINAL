package Test

import (
	"chtl/CHTL/lexer"
	"fmt"
	"testing"
)

func TestDebugLexer(t *testing.T) {
	input := `div
{
    id: main;
    class: container;
    
    text
    {
        Hello, CHTL World!
    }
    
    style
    {
        .container
        {
            width: 100%;
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }
        
        &:hover
        {
            background-color: #f0f0f0;
        }
    }
}`

	l := lexer.NewLexer(input)
	
	fmt.Println("词法分析结果:")
	for {
		token := l.NextToken()
		fmt.Printf("Token: %s, Literal: '%s', Position: %d:%d\n", 
			token.Type, token.Literal, token.Position.Line, token.Position.Column)
		if token.Type == lexer.EOF {
			break
		}
	}
}
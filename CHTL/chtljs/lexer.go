package chtljs

import (
	"chtl/CHTL/lexer"
)

// CHTLJSLexer CHTL JS 词法分析器
type CHTLJSLexer struct {
	*lexer.Lexer
}

// NewCHTLJSLexer 创建 CHTL JS 词法分析器
func NewCHTLJSLexer(input string) *CHTLJSLexer {
	baseLexer := lexer.NewLexer(input)
	return &CHTLJSLexer{
		Lexer: baseLexer,
	}
}

// NextToken 获取下一个词法单元
func (l *CHTLJSLexer) NextToken() lexer.Token {
	return l.Lexer.NextToken()
}
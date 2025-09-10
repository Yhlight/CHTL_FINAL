package parser

import (
	"chtl/pkg/ast"
	"chtl/pkg/lexer"
)

type ExpressionParser struct {
	tokens []lexer.Token
	pos    int

	prefixParseFns map[lexer.TokenType]prefixParseFn
	infixParseFns  map[lexer.TokenType]infixParseFn
}

func NewExpressionParser(tokens []lexer.Token) *ExpressionParser {
	p := &ExpressionParser{tokens: tokens}

	p.prefixParseFns = make(map[lexer.TokenType]prefixParseFn)
	p.registerPrefix(lexer.IDENT, p.parseIdentifier)
	p.registerPrefix(lexer.NUMBER, p.parseNumberLiteral)

	p.infixParseFns = make(map[lexer.TokenType]infixParseFn)
	p.registerInfix(lexer.PLUS, p.parseInfixExpression)
	p.registerInfix(lexer.MINUS, p.parseInfixExpression)
	p.registerInfix(lexer.SLASH, p.parseInfixExpression)
	p.registerInfix(lexer.ASTERISK, p.parseInfixExpression)
	return p
}

func (p *ExpressionParser) curToken() lexer.Token {
	if p.pos >= len(p.tokens) {
		return lexer.Token{Type: lexer.EOF}
	}
	return p.tokens[p.pos]
}

func (p *ExpressionParser) peekToken() lexer.Token {
	if p.pos+1 >= len(p.tokens) {
		return lexer.Token{Type: lexer.EOF}
	}
	return p.tokens[p.pos+1]
}

func (p *ExpressionParser) nextToken() { p.pos++ }

func (p *ExpressionParser) Parse() ast.Expression {
	p.pos = 0
	return p.parseExpression(LOWEST)
}

func (p *ExpressionParser) parseExpression(precedence int) ast.Expression {
	if p.curToken().Type == lexer.EOF { return nil }

	prefix := p.prefixParseFns[p.curToken().Type]
	if prefix == nil { return nil }
	leftExp := prefix()

	for precedence < p.peekPrecedence() {
		infix := p.infixParseFns[p.peekToken().Type]
		if infix == nil { return leftExp }
		p.nextToken()
		leftExp = infix(leftExp)
	}
	return leftExp
}

func (p *ExpressionParser) parseIdentifier() ast.Expression {
	return &ast.Identifier{Token: p.curToken(), Value: p.curToken().Literal}
}

func (p *ExpressionParser) parseNumberLiteral() ast.Expression {
	lit := &ast.NumberLiteral{Token: p.curToken(), Value: p.curToken().Literal}
	if p.peekToken().Type == lexer.IDENT {
		p.nextToken()
		lit.Unit = p.curToken().Literal
	}
	return lit
}

func (p *ExpressionParser) parseInfixExpression(left ast.Expression) ast.Expression {
	expression := &ast.InfixExpression{
		Token:    p.curToken(),
		Operator: p.curToken().Literal,
		Left:     left,
	}
	precedence := p.curPrecedence()
	p.nextToken()
	expression.Right = p.parseExpression(precedence)
	return expression
}

func (p *ExpressionParser) peekPrecedence() int {
	if pr, ok := precedences[p.peekToken().Type]; ok { return pr }
	return LOWEST
}
func (p *ExpressionParser) curPrecedence() int {
	if pr, ok := precedences[p.curToken().Type]; ok { return pr }
	return LOWEST
}

func (p *ExpressionParser) registerPrefix(tokenType lexer.TokenType, fn prefixParseFn) { p.prefixParseFns[tokenType] = fn }
func (p *ExpressionParser) registerInfix(tokenType lexer.TokenType, fn infixParseFn) { p.infixParseFns[tokenType] = fn }

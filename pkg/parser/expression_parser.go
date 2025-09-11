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
	p.registerPrefix(lexer.STRING, p.parseStringLiteral)

	p.infixParseFns = make(map[lexer.TokenType]infixParseFn)
	p.registerInfix(lexer.PLUS, p.parseInfixExpression)
	p.registerInfix(lexer.MINUS, p.parseInfixExpression)
	p.registerInfix(lexer.SLASH, p.parseInfixExpression)
	p.registerInfix(lexer.ASTERISK, p.parseInfixExpression)
	p.registerInfix(lexer.GT, p.parseInfixExpression)
	p.registerInfix(lexer.LT, p.parseInfixExpression)
	p.registerInfix(lexer.QUESTION, p.parseConditionalExpression)
	p.registerInfix(lexer.LPAREN, p.parseCallExpression)
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
	if p.curToken().Type == lexer.EOF {
		return nil
	}

	prefix := p.prefixParseFns[p.curToken().Type]
	if prefix == nil {
		return nil
	}
	leftExp := prefix()

	for precedence < p.peekPrecedence() {
		infix := p.infixParseFns[p.peekToken().Type]
		if infix == nil {
			return leftExp
		}
		p.nextToken()
		leftExp = infix(leftExp)
	}
	return leftExp
}

func (p *ExpressionParser) parseIdentifier() ast.Expression {
	return &ast.Identifier{Token: p.curToken(), Value: p.curToken().Literal}
}

func (p *ExpressionParser) parseStringLiteral() ast.Expression {
	return &ast.StringLiteral{Token: p.curToken(), Value: p.curToken().Literal}
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

func (p *ExpressionParser) parseConditionalExpression(condition ast.Expression) ast.Expression {
	exp := &ast.ConditionalExpression{
		Token:     p.curToken(),
		Condition: condition,
	}
	p.nextToken()
	exp.Consequence = p.parseExpression(LOWEST)
	if p.peekToken().Type != lexer.COLON {
		return nil // Simplified: no optional alternative for now
	}
	p.nextToken() // consume ':'
	p.nextToken() // consume consequence
	exp.Alternative = p.parseExpression(LOWEST)
	return exp
}

func (p *ExpressionParser) parseCallExpression(function ast.Expression) ast.Expression {
	exp := &ast.CallExpression{Token: p.curToken(), Function: function}
	exp.Arguments = p.parseExpressionList(lexer.RPAREN)
	return exp
}

func (p *ExpressionParser) parseExpressionList(end lexer.TokenType) []ast.Expression {
	list := []ast.Expression{}

	if p.peekToken().Type == end {
		p.nextToken()
		return list
	}

	p.nextToken()
	list = append(list, p.parseExpression(LOWEST))

	for p.peekToken().Type == lexer.COMMA {
		p.nextToken()
		p.nextToken()
		list = append(list, p.parseExpression(LOWEST))
	}

	if p.peekToken().Type != end {
		return nil
	}
	p.nextToken()

	return list
}

func (p *ExpressionParser) peekPrecedence() int {
	if pr, ok := precedences[p.peekToken().Type]; ok {
		return pr
	}
	return LOWEST
}
func (p *ExpressionParser) curPrecedence() int {
	if pr, ok := precedences[p.curToken().Type]; ok {
		return pr
	}
	return LOWEST
}

func (p *ExpressionParser) registerPrefix(tokenType lexer.TokenType, fn prefixParseFn) {
	p.prefixParseFns[tokenType] = fn
}
func (p *ExpressionParser) registerInfix(tokenType lexer.TokenType, fn infixParseFn) {
	p.infixParseFns[tokenType] = fn
}

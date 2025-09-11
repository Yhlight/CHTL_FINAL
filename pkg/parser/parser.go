package parser

import (
	"chtl/pkg/ast"
	"chtl/pkg/lexer"
	"fmt"
)

const (
	_ int = iota
	LOWEST
	TERNARY
	LESSGREATER
	SUM
	PRODUCT
	PREFIX
	CALL
)

var precedences = map[lexer.TokenType]int{
	lexer.PLUS:     SUM,
	lexer.MINUS:    SUM,
	lexer.SLASH:    PRODUCT,
	lexer.ASTERISK: PRODUCT,
	lexer.LPAREN:   CALL,
	lexer.LT:       LESSGREATER,
	lexer.GT:       LESSGREATER,
	lexer.QUESTION: TERNARY,
}

type (
	prefixParseFn func() ast.Expression
	infixParseFn  func(ast.Expression) ast.Expression
)

type Parser struct {
	l      *lexer.Lexer
	errors []string

	curToken  lexer.Token
	peekToken lexer.Token

	prefixParseFns map[lexer.TokenType]prefixParseFn
	infixParseFns  map[lexer.TokenType]infixParseFn
}

func New(l *lexer.Lexer) *Parser {
	p := &Parser{l: l, errors: []string{}}
	p.prefixParseFns = make(map[lexer.TokenType]prefixParseFn)
	p.registerPrefix(lexer.IDENT, p.parseIdentifier)
	p.registerPrefix(lexer.STRING, p.parseStringLiteral)
	p.registerPrefix(lexer.NUMBER, p.parseNumberLiteral)
	p.registerPrefix(lexer.DOT, p.parsePrefixExpression)
	p.registerPrefix(lexer.HASH, p.parsePrefixExpression)
	p.registerPrefix(lexer.AMPERSAND, p.parsePrefixExpression)
	p.registerPrefix(lexer.AT, p.parseTemplateUsageExpression)

	p.infixParseFns = make(map[lexer.TokenType]infixParseFn)
	p.registerInfix(lexer.PLUS, p.parseInfixExpression)
	p.registerInfix(lexer.MINUS, p.parseInfixExpression)
	p.registerInfix(lexer.SLASH, p.parseInfixExpression)
	p.registerInfix(lexer.ASTERISK, p.parseInfixExpression)
	p.registerInfix(lexer.LPAREN, p.parseCallExpression)

	p.nextToken()
	p.nextToken()
	return p
}

func (p *Parser) Errors() []string { return p.errors }

func (p *Parser) nextToken() {
	p.curToken = p.peekToken
	p.peekToken = p.l.NextToken()
}

func (p *Parser) ParseProgram() *ast.Program {
	program := &ast.Program{
		Statements: []ast.Statement{},
		Templates:  make(map[string]*ast.TemplateDefinition),
	}
	for !p.curTokenIs(lexer.EOF) {
		if p.curTokenIs(lexer.COMMENT) || p.curTokenIs(lexer.GEN_COMMENT) {
			p.nextToken()
			continue
		}
		stmt := p.parseStatement(false, program)
		if stmt != nil {
			if templateDef, ok := stmt.(*ast.TemplateDefinition); ok {
				program.Templates[templateDef.Name.Value] = templateDef
			} else {
				program.Statements = append(program.Statements, stmt)
			}
		}
		p.nextToken()
	}
	return program
}

func (p *Parser) parseStatement(inStyleBlock bool, program *ast.Program) ast.Statement {
	switch p.curToken.Type {
	case lexer.LBRACKET:
		if p.peekTokenIs(lexer.IDENT) {
			switch p.peekToken.Literal {
			case "Template":
				return p.parseTemplateDefinitionStatement()
			case "Import":
				return p.parseImportStatement()
			case "Origin":
				return p.parseOriginStatement()
			}
		}
	case lexer.AT:
		return p.parseTemplateUsageStatement()
	case lexer.IDENT:
		if inStyleBlock {
			if p.peekTokenIs(lexer.COLON) {
				return p.parseStyleRuleStatement()
			}
			return p.parseRulesetStatement(program)
		} else {
			switch p.curToken.Literal {
			case "style":
				if p.peekTokenIs(lexer.LBRACE) {
					return p.parseStyleStatement(program)
				}
			case "text":
				if p.peekTokenIs(lexer.LBRACE) {
					return p.parseTextStatement()
				}
			case "script":
				if p.peekTokenIs(lexer.LBRACE) {
					return p.parseScriptStatement()
				}
			}
			if p.peekTokenIs(lexer.LBRACE) {
				return p.parseElementStatement(program)
			} else if p.peekTokenIs(lexer.COLON) || p.peekTokenIs(lexer.ASSIGN) {
				return p.parseAttributeStatement()
			}
		}
	case lexer.DOT, lexer.HASH, lexer.AMPERSAND:
		if inStyleBlock {
			return p.parseRulesetStatement(program)
		}
	}
	return nil
}

func (p *Parser) parseOriginStatement() ast.Statement {
	stmt := &ast.OriginStatement{Token: p.curToken}
	if !p.expectPeek(lexer.IDENT) || p.curToken.Literal != "Origin" {
		p.errors = append(p.errors, "Expected 'Origin' keyword after '['")
		return nil
	}
	if !p.expectPeek(lexer.RBRACKET) {
		return nil
	}
	if !p.expectPeek(lexer.AT) {
		return nil
	}
	if !p.expectPeek(lexer.IDENT) {
		return nil
	}
	stmt.Type = p.curToken.Literal

	if !p.expectPeek(lexer.LBRACE) {
		return nil
	}

	content, err := p.l.ReadRawBlock()
	if err != nil {
		p.errors = append(p.errors, err.Error())
		return nil
	}
	stmt.Content = content
	p.nextToken()
	return stmt
}

func (p *Parser) parseScriptStatement() ast.Statement {
	stmt := &ast.ScriptStatement{Token: p.curToken}

	if !p.expectPeek(lexer.LBRACE) {
		return nil
	}

	content, err := p.l.ReadRawBlock()
	if err != nil {
		p.errors = append(p.errors, err.Error())
		return nil
	}
	stmt.Content = content
	p.nextToken()
	return stmt
}

func (p *Parser) parseImportStatement() ast.Statement {
	stmt := &ast.ImportStatement{Token: p.curToken}
	p.nextToken() // consume '['

	if !p.curTokenIs(lexer.IDENT) || p.curToken.Literal != "Import" {
		p.errors = append(p.errors, "Expected 'Import' keyword after '['")
		return nil
	}

	if !p.expectPeek(lexer.RBRACKET) {
		return nil
	}
	if !p.expectPeek(lexer.AT) {
		return nil
	}
	if !p.expectPeek(lexer.IDENT) {
		return nil
	}
	stmt.ImportType = "@" + p.curToken.Literal

	if !p.expectPeek(lexer.FROM) {
		return nil
	}
	p.nextToken()
	stmt.Path = p.parseExpression(LOWEST)

	if p.peekTokenIs(lexer.AS) {
		p.nextToken()
		if !p.expectPeek(lexer.IDENT) {
			return nil
		}
		stmt.Alias = p.parseIdentifier().(*ast.Identifier)
	}

	if p.peekTokenIs(lexer.SEMICOLON) {
		p.nextToken()
	}

	return stmt
}

func (p *Parser) parseTemplateDefinitionStatement() ast.Statement {
	stmt := &ast.TemplateDefinition{Token: p.curToken}
	if !p.expectPeek(lexer.IDENT) || p.curToken.Literal != "Template" {
		p.errors = append(p.errors, "Expected 'Template' keyword after '['")
		return nil
	}
	if !p.expectPeek(lexer.RBRACKET) {
		return nil
	}
	if !p.expectPeek(lexer.AT) {
		return nil
	}
	if !p.expectPeek(lexer.IDENT) {
		return nil
	}
	stmt.TemplateType = p.curToken.Literal
	if !p.expectPeek(lexer.IDENT) {
		return nil
	}
	stmt.Name = p.parseIdentifier().(*ast.Identifier)
	if !p.expectPeek(lexer.LBRACE) {
		return nil
	}
	inStyleBlock := stmt.TemplateType == "Style"
	stmt.Body = p.parseBlockStatement(inStyleBlock, nil)
	return stmt
}

func (p *Parser) parseTemplateUsageStatement() ast.Statement {
	stmt := &ast.TemplateUsage{Token: p.curToken}
	if !p.expectPeek(lexer.IDENT) {
		return nil
	}
	stmt.TemplateType = p.curToken.Literal
	if !p.expectPeek(lexer.IDENT) {
		return nil
	}
	stmt.Name = p.parseIdentifier().(*ast.Identifier)
	if !p.expectPeek(lexer.SEMICOLON) {
		return nil
	}
	return stmt
}

func (p *Parser) parseTemplateUsageExpression() ast.Expression {
	stmt := p.parseTemplateUsageStatement()
	if stmt == nil {
		return nil
	}
	return stmt.(ast.Expression)
}

func (p *Parser) parseElementStatement(program *ast.Program) *ast.Element {
	element := &ast.Element{Token: p.curToken, Name: p.curToken.Literal}
	if !p.expectPeek(lexer.LBRACE) {
		return nil
	}
	element.Body = p.parseBlockStatement(false, program)
	return element
}

func (p *Parser) parseTextStatement() *ast.Text {
	text := &ast.Text{Token: p.curToken}
	if !p.expectPeek(lexer.LBRACE) {
		return nil
	}
	p.nextToken() // consume {
	text.Content = p.parseExpression(LOWEST)
	if !p.expectPeek(lexer.RBRACE) {
		return nil
	}
	return text
}

func (p *Parser) parseAttributeStatement() *ast.Attribute {
	attribute := &ast.Attribute{Token: p.curToken, Name: p.curToken.Literal}
	if !p.peekTokenIs(lexer.COLON) && !p.peekTokenIs(lexer.ASSIGN) {
		p.peekError(lexer.COLON)
		return nil
	}
	p.nextToken() // consume name
	p.nextToken() // consume : or =
	attribute.Value = p.parseExpression(LOWEST)
	if !p.expectPeek(lexer.SEMICOLON) {
		return nil
	}
	return attribute
}

func (p *Parser) parseStyleStatement(program *ast.Program) *ast.Style {
	style := &ast.Style{Token: p.curToken}
	if !p.expectPeek(lexer.LBRACE) {
		return nil
	}
	style.Body = p.parseBlockStatement(true, program)
	return style
}

func (p *Parser) parseStyleRuleStatement() *ast.StyleRule {
	rule := &ast.StyleRule{Token: p.curToken, Name: p.curToken.Literal}
	if !p.expectPeek(lexer.COLON) {
		return nil
	}
	p.nextToken()

	rawExpr := &ast.RawExpression{Tokens: []lexer.Token{}}
	for !p.curTokenIs(lexer.SEMICOLON) && !p.curTokenIs(lexer.EOF) {
		rawExpr.Tokens = append(rawExpr.Tokens, p.curToken)
		p.nextToken()
	}
	rule.Value = rawExpr

	if !p.curTokenIs(lexer.SEMICOLON) {
		p.peekError(lexer.SEMICOLON)
		return nil
	}
	return rule
}

func (p *Parser) parseRulesetStatement(program *ast.Program) *ast.Ruleset {
	ruleset := &ast.Ruleset{Token: p.curToken}
	ruleset.Selector = p.parseExpression(PREFIX)
	if !p.expectPeek(lexer.LBRACE) {
		return nil
	}
	ruleset.Body = p.parseBlockStatement(true, program)
	return ruleset
}

func (p *Parser) parseBlockStatement(inStyleBlock bool, program *ast.Program) *ast.BlockStatement {
	block := &ast.BlockStatement{Token: p.curToken, Statements: []ast.Statement{}}
	p.nextToken()
	for !p.curTokenIs(lexer.RBRACE) && !p.curTokenIs(lexer.EOF) {
		if p.curTokenIs(lexer.COMMENT) || p.curTokenIs(lexer.GEN_COMMENT) {
			p.nextToken()
			continue
		}
		stmt := p.parseStatement(inStyleBlock, program)
		if stmt != nil {
			block.Statements = append(block.Statements, stmt)
		}
		p.nextToken()
	}
	return block
}

func (p *Parser) parseExpression(precedence int) ast.Expression {
	prefix := p.prefixParseFns[p.curToken.Type]
	if prefix == nil {
		p.noPrefixParseFnError(p.curToken.Type)
		return nil
	}
	leftExp := prefix()
	for !p.peekTokenIs(lexer.SEMICOLON) && precedence < p.peekPrecedence() {
		infix := p.infixParseFns[p.peekToken.Type]
		if infix == nil {
			return leftExp
		}
		p.nextToken()
		leftExp = infix(leftExp)
	}
	return leftExp
}

func (p *Parser) parsePrefixExpression() ast.Expression {
	expression := &ast.PrefixExpression{Token: p.curToken, Operator: p.curToken.Literal}
	p.nextToken()
	expression.Right = p.parseExpression(PREFIX)
	return expression
}

func (p *Parser) parseInfixExpression(left ast.Expression) ast.Expression {
	expression := &ast.InfixExpression{Token: p.curToken, Operator: p.curToken.Literal, Left: left}
	precedence := p.curPrecedence()
	p.nextToken()
	expression.Right = p.parseExpression(precedence)
	return expression
}

func (p *Parser) parseIdentifier() ast.Expression {
	return &ast.Identifier{Token: p.curToken, Value: p.curToken.Literal}
}
func (p *Parser) parseStringLiteral() ast.Expression {
	return &ast.StringLiteral{Token: p.curToken, Value: p.curToken.Literal}
}
func (p *Parser) parseNumberLiteral() ast.Expression {
	return &ast.NumberLiteral{Token: p.curToken, Value: p.curToken.Literal}
}
func (p *Parser) peekPrecedence() int {
	if p, ok := precedences[p.peekToken.Type]; ok {
		return p
	}
	return LOWEST
}
func (p *Parser) curPrecedence() int {
	if p, ok := precedences[p.curToken.Type]; ok {
		return p
	}
	return LOWEST
}
func (p *Parser) registerPrefix(tokenType lexer.TokenType, fn prefixParseFn) {
	p.prefixParseFns[tokenType] = fn
}
func (p *Parser) registerInfix(tokenType lexer.TokenType, fn infixParseFn) {
	p.infixParseFns[tokenType] = fn
}
func (p *Parser) noPrefixParseFnError(t lexer.TokenType) {
	p.errors = append(p.errors, fmt.Sprintf("no prefix parse function for %s found", t))
}
func (p *Parser) curTokenIs(t lexer.TokenType) bool {
	return p.curToken.Type == t
}
func (p *Parser) peekTokenIs(t lexer.TokenType) bool {
	return p.peekToken.Type == t
}
func (p *Parser) expectPeek(t lexer.TokenType) bool {
	if p.peekTokenIs(t) {
		p.nextToken()
		return true
	}
	p.peekError(t)
	return false
}
func (p *Parser) peekError(t lexer.TokenType) {
	p.errors = append(p.errors, fmt.Sprintf("expected next token to be %s, got %s instead", t, p.peekToken.Type))
}

func (p *Parser) parseCallExpression(function ast.Expression) ast.Expression {
	exp := &ast.CallExpression{Token: p.curToken, Function: function}
	exp.Arguments = p.parseExpressionList(lexer.RPAREN)
	return exp
}

func (p *Parser) parseExpressionList(end lexer.TokenType) []ast.Expression {
	list := []ast.Expression{}
	if p.peekTokenIs(end) {
		p.nextToken()
		return list
	}
	p.nextToken()
	list = append(list, p.parseExpression(LOWEST))
	for p.peekTokenIs(lexer.COMMA) {
		p.nextToken()
		p.nextToken()
		list = append(list, p.parseExpression(LOWEST))
	}
	if !p.expectPeek(end) {
		return nil
	}
	return list
}

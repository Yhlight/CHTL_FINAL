package parser

import (
	"chtl/pkg/ast"
	"chtl/pkg/lexer"
	"testing"
)

func TestConditionalExpressionParsing(t *testing.T) {
	input := `width > 50px ? "red" : "blue"`
	l := lexer.New(input)
	tokens := []lexer.Token{}
	for tok := l.NextToken(); tok.Type != lexer.EOF; tok = l.NextToken() {
		tokens = append(tokens, tok)
	}

	p := NewExpressionParser(tokens)
	exp := p.Parse()

	condExp, ok := exp.(*ast.ConditionalExpression)
	if !ok {
		t.Fatalf("exp is not *ast.ConditionalExpression. got=%T", exp)
	}

	infix, ok := condExp.Condition.(*ast.InfixExpression)
	if !ok {
		t.Fatalf("condition is not *ast.InfixExpression. got=%T", condExp.Condition)
	}

	if infix.Left.String() != "width" {
		t.Errorf("infix.Left.String() wrong. expected=width, got=%s", infix.Left.String())
	}

	if infix.Operator != ">" {
		t.Errorf("infix.Operator wrong. expected=>, got=%s", infix.Operator)
	}

	if condExp.Consequence.String() != `"red"` {
		t.Errorf("condExp.Consequence.String() wrong. got=%s", condExp.Consequence.String())
	}

	if condExp.Alternative.String() != `"blue"` {
		t.Errorf("condExp.Alternative.String() wrong. got=%s", condExp.Alternative.String())
	}
}

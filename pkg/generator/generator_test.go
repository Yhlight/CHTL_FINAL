package generator

import (
	"chtl/pkg/lexer"
	"chtl/pkg/parser"
	"strings"
	"testing"
)

func TestHtmlGeneration(t *testing.T) {
	input := `div { id: "main"; }`
	expected := `<div id="main"></div>`

	l := lexer.New(input)
	p := parser.New(l)
	gen := New()
	program := p.ParseProgram()
	if len(p.Errors()) != 0 {
		t.Fatalf("parser has errors: %v", p.Errors())
	}

	html, err := gen.Generate(program)
	if err != nil {
		t.Fatalf("generation failed: %v", err)
	}

	if html != expected {
		t.Errorf("generated html wrong.\nexpected=%q\n     got=%q", expected, html)
	}
}

func TestArithmeticGeneration(t *testing.T) {
	input := `
div {
    style {
        width: 100px + 20em * 2;
    }
}
`
	l := lexer.New(input)
	p := parser.New(l)
	gen := New()
	program := p.ParseProgram()

	if len(p.Errors()) != 0 {
		t.Fatalf("parser has errors: %v", p.Errors())
	}

	html, err := gen.Generate(program)
	if err != nil {
		t.Fatalf("generation failed: %v", err)
	}

	// The AST for `100px + 20em * 2` is `(100px + (20em * 2))`
	// The generator should wrap this in calc()
	expectedStr := `width: calc(100px + (20em * 2))`
	if !strings.Contains(html, expectedStr) {
		t.Errorf("generated html wrong.\nexpected to contain %q\n                got=%q", expectedStr, html)
	}
}

package parser

import (
	"chtl/pkg/ast"
	"chtl/pkg/lexer"
	"strings"
	"testing"
)

func TestScriptStatement(t *testing.T) {
	input := `
div {
    script {
        const btn = {{.button}};
        btn.addEventListener('click', () => {
            console.log('clicked');
        });
    }
}
`
	l := lexer.New(input)
	p := New(l)
	program := p.ParseProgram()
	checkParserErrors(t, p)

	if len(program.Statements) != 1 {
		t.Fatalf("program.Statements does not contain 1 statement. got=%d", len(program.Statements))
	}

	element, ok := program.Statements[0].(*ast.Element)
	if !ok {
		t.Fatalf("statement is not *ast.Element, got %T", program.Statements[0])
	}

	if len(element.Body.Statements) != 1 {
		t.Fatalf("element body should have 1 statement. got=%d", len(element.Body.Statements))
	}

	script, ok := element.Body.Statements[0].(*ast.ScriptStatement)
	if !ok {
		t.Fatalf("statement is not *ast.ScriptStatement. got=%T", element.Body.Statements[0])
	}

	expectedContent := `
        const btn = {{.button}};
        btn.addEventListener('click', () => {
            console.log('clicked');
        });
    `
	// Compare trimmed content to avoid issues with leading/trailing whitespace from slicing
	if strings.TrimSpace(script.Content) != strings.TrimSpace(expectedContent) {
		t.Errorf("script content wrong.\nexpected=%q\n     got=%q", expectedContent, script.Content)
	}
}


func TestTemplateParsing(t *testing.T) {
	input := `
[Template] @Style DefaultText { color: black; }
div { style { @Style DefaultText; } }
`
	l := lexer.New(input)
	p := New(l)
	program := p.ParseProgram()
	checkParserErrors(t, p)

	if len(program.Templates) != 1 {
		t.Fatalf("program.Templates does not contain 1 template. got=%d", len(program.Templates))
	}
}


func checkParserErrors(t *testing.T, p *Parser) {
	errors := p.Errors()
	if len(errors) == 0 {
		return
	}
	t.Errorf("parser has %d errors", len(errors))
	for _, msg := range errors {
		t.Errorf("parser error: %q", msg)
	}
	t.FailNow()
}

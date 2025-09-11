package parser

import (
	"chtl/pkg/ast"
	"chtl/pkg/lexer"
	"testing"
)

func TestElementStatement(t *testing.T) {
	input := `div { id: "main"; }`

	l := lexer.New(input)
	p := New(l)
	program := p.ParseProgram()
	checkParserErrors(t, p)

	if len(program.Statements) != 1 {
		t.Fatalf("program.Statements does not contain 1 statements. got=%d", len(program.Statements))
	}

	stmt, ok := program.Statements[0].(*ast.Element)
	if !ok {
		t.Fatalf("program.Statements[0] is not ast.Element. got=%T", program.Statements[0])
	}

	if stmt.Name != "div" {
		t.Errorf("stmt.Name not 'div'. got=%q", stmt.Name)
	}
}

func TestImportStatement(t *testing.T) {
	input := `[Import] @Chtl from "module" as MyModule;`
	l := lexer.New(input)
	p := New(l)
	program := p.ParseProgram()
	checkParserErrors(t, p)

	if len(program.Statements) != 1 {
		t.Fatalf("program.Statements does not contain 1 statements. got=%d", len(program.Statements))
	}

	stmt, ok := program.Statements[0].(*ast.ImportStatement)
	if !ok {
		t.Fatalf("program.Statements[0] is not ast.ImportStatement. got=%T", program.Statements[0])
	}

	if stmt.ImportType != "@Chtl" {
		t.Errorf("stmt.ImportType not '@Chtl'. got=%q", stmt.ImportType)
	}

	if stmt.Alias.Value != "MyModule" {
		t.Errorf("stmt.Alias.Value not 'MyModule'. got=%q", stmt.Alias.Value)
	}
}

func TestTemplateUsageInElement(t *testing.T) {
	input := `
div {
    @Element Box;
}
`
	l := lexer.New(input)
	p := New(l)
	program := p.ParseProgram()
	checkParserErrors(t, p)

	if len(program.Statements) != 1 {
		t.Fatalf("program.Statements does not contain 1 statements. got=%d", len(program.Statements))
	}

	element, ok := program.Statements[0].(*ast.Element)
	if !ok {
		t.Fatalf("program.Statements[0] is not ast.Element. got=%T", program.Statements[0])
	}

	if len(element.Body.Statements) != 1 {
		t.Fatalf("element.Body.Statements does not contain 1 statement. got=%d", len(element.Body.Statements))
	}

	_, ok = element.Body.Statements[0].(*ast.TemplateUsage)
	if !ok {
		t.Fatalf("element.Body.Statements[0] is not ast.TemplateUsage. got=%T", element.Body.Statements[0])
	}
}

func TestFullProgramParsing(t *testing.T) {
	input := `
[Template] @Element Box {
    span { }
}
div {
    @Element Box;
}
`
	l := lexer.New(input)
	p := New(l)
	program := p.ParseProgram()
	checkParserErrors(t, p)

	if len(program.Templates) != 1 {
		t.Fatalf("program.Templates does not contain 1 template. got=%d", len(program.Templates))
	}

	if len(program.Statements) != 1 {
		t.Fatalf("program.Statements does not contain 1 statement. got=%d", len(program.Statements))
	}

	expected := `div { @Element Box; }`
	if program.String() != expected {
		t.Errorf("program.String() wrong.\nexpected=%q\n     got=%q", expected, program.String())
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

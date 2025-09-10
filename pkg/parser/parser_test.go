package parser

import (
	"chtl/pkg/ast"
	"chtl/pkg/lexer"
	"testing"
)

func TestImportStatement(t *testing.T) {
	input := `[Import] @Chtl from "./test.chtl" as MyModule;`

	l := lexer.New(input)
	p := New(l)
	program := p.ParseProgram()
	checkParserErrors(t, p)

	if len(program.Statements) != 1 {
		t.Fatalf("program.Statements does not contain 1 statement. got=%d", len(program.Statements))
	}

	stmt, ok := program.Statements[0].(*ast.ImportStatement)
	if !ok {
		t.Fatalf("stmt is not *ast.ImportStatement. got=%T", program.Statements[0])
	}

	if stmt.ImportType != "@Chtl" {
		t.Errorf("stmt.ImportType not '@Chtl'. got=%q", stmt.ImportType)
	}

	if str, ok := stmt.Path.(*ast.StringLiteral); ok {
		if str.Value != "./test.chtl" {
			t.Errorf("stmt.Path.Value not './test.chtl'. got=%q", str.Value)
		}
	} else {
		t.Errorf("stmt.Path not *ast.StringLiteral. got=%T", stmt.Path)
	}

	if stmt.Alias == nil {
		t.Fatalf("stmt.Alias is nil. expected 'MyModule'")
	}
	if stmt.Alias.Value != "MyModule" {
		t.Errorf("stmt.Alias.Value not 'MyModule'. got=%q", stmt.Alias.Value)
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

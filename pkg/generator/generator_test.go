package generator

import (
	"chtl/pkg/lexer"
	"chtl/pkg/parser"
	"strings"
	"testing"
)

func TestScriptGenerationWithCHTLJS(t *testing.T) {
	input := `
div {
    script {
        const btn = {{ .button }};
        const app = {{ #app }};
        const p_tags = {{ p }};
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

	// Check that the CHTL JS fragments were compiled correctly
	if !strings.Contains(html, "const btn = document.querySelector('.button');") {
		t.Errorf("script missing compiled class selector. got=%q", html)
	}
	if !strings.Contains(html, "const app = document.querySelector('#app');") {
		t.Errorf("script missing compiled id selector. got=%q", html)
	}
	if !strings.Contains(html, "const p_tags = document.querySelectorAll('p');") {
		t.Errorf("script missing compiled tag selector. got=%q", html)
	}
}

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

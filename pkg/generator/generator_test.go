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

	expectedStr := `width: calc(100px + (20em * 2))`
	if !strings.Contains(html, expectedStr) {
		t.Errorf("generated html wrong.\nexpected to contain %q\n                got=%q", expectedStr, html)
	}
}

func TestNewFeatures(t *testing.T) {
	testCases := []struct {
		name     string
		input    string
		expected []string
	}{
		{
			name: "Element Template",
			input: `
[Template] @Element Box {
    div { class: "box-inner"; }
}
div {
    @Element Box;
}
`,
			expected: []string{`<div><div class="box-inner"></div></div>`},
		},
		{
			name: "Origin Block",
			input: `
div {
    [Origin] @Html { <p>Raw HTML</p> }
}
`,
			expected: []string{`<div><p>Raw HTML</p></div>`},
		},
		{
			name: "Script Block",
			input: `
div {
    id: "my-div";
    script {
        const a = {{&}};
        a->style.color = "red";
    }
}
`,
			expected: []string{`<script>`, `const a = document.getElementById('my-div')`, `a.style.color = "red"`},
		},
		{
			name: "Var Template",
			input: `
[Template] @Var Colors {
    brand-red: "#ff0000";
}
div {
    style {
        color: Colors(brand-red);
    }
}
`,
			expected: []string{`<div style="color: #ff0000"></div>`},
		},
		{
			name: "Conditional Expression",
			input: `
div {
    style {
        width: 100px;
        color: width > 50px ? "red" : "blue";
    }
}
`,
			expected: []string{`width: 100px`, `color: red`},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			l := lexer.New(tc.input)
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

			for _, expectedStr := range tc.expected {
				if !strings.Contains(html, expectedStr) {
					t.Errorf("generated html wrong.\nexpected to contain %q\n                got=%q", expectedStr, html)
				}
			}
		})
	}
}

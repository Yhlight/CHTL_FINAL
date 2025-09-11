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

func TestProcessScriptContent(t *testing.T) {
	g := New()
	attrs := map[string]string{"id": "test-id"}
	input := `const a = {{&}};
a->style.color = 'red';`
	expected := `const a = document.getElementById('test-id');
a.style.color = 'red';`

	output, err := g.processScriptContent(input, attrs)
	if err != nil {
		t.Fatalf("processScriptContent failed: %v", err)
	}

	if output != expected {
		t.Errorf("script content wrong.\nexpected=%q\n     got=%q", expected, output)
	}
}

func TestNewFeatures(t *testing.T) {
	testCases := []struct {
		name     string
		input    string
		expected string
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
			expected: `<div><div class="box-inner"></div></div>`,
		},
		{
			name: "Origin Block",
			input: `
div {
    [Origin] @Html {
        <p>Raw HTML</p>
    }
}
`,
			expected: `<div><p>Raw HTML</p></div>`,
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
			expected: `<script>
const a = document.getElementById('my-div');
a.style.color = "red";
</script>`,
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
			expected: `<div style="color: #ff0000"></div>`,
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
			expected: `<div style="width: 100px; color: red"></div>`,
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

			if tc.name == "Conditional Expression" {
				if !strings.Contains(html, `width: 100px`) {
					t.Errorf("expected to contain `width: 100px`, got %q", html)
				}
				if !strings.Contains(html, `color: red`) {
					t.Errorf("expected to contain `color: red`, got %q", html)
				}
			} else {
				if !strings.Contains(html, tc.expected) {
					t.Errorf("generated html wrong.\nexpected to contain %q\n                got=%q", tc.expected, html)
				}
			}
		})
	}
}

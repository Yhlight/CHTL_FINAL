package scanner

import (
	"reflect"
	"testing"
)

func TestScanAndReplace(t *testing.T) {
	input := `const btn = {{.button}}; btn.addEventListener('click', () => { console.log({{btn.text}}); });`
	s := New()
	result := s.ScanAndReplace(input)

	// 1. Check the string with placeholders
	expectedString := `__CHTL_JS_PLACEHOLDER_0__{{.button}}__CHTL_JS_PLACEHOLDER_1__{{btn.text}}__CHTL_JS_PLACEHOLDER_2__`
	if result != expectedString {
		t.Errorf("ScanAndReplace string wrong.\nexpected=%q\n     got=%q", expectedString, result)
	}

	// 2. Check the map of replaced fragments
	if len(s.Replaced) != 3 {
		t.Fatalf("Replaced map should have 3 entries. got=%d", len(s.Replaced))
	}

	if val, ok := s.Replaced["__CHTL_JS_PLACEHOLDER_0__"]; !ok || val != "const btn = " {
		t.Errorf("Replaced map has wrong value for placeholder 0. got=%q", val)
	}
	if val, ok := s.Replaced["__CHTL_JS_PLACEHOLDER_1__"]; !ok || val != "; btn.addEventListener('click', () => { console.log(" {
		t.Errorf("Replaced map has wrong value for placeholder 1. got=%q", val)
	}
	if val, ok := s.Replaced["__CHTL_JS_PLACEHOLDER_2__"]; !ok || val != "); });" {
		t.Errorf("Replaced map has wrong value for placeholder 2. got=%q", val)
	}
}

func TestScanScript(t *testing.T) {
	tests := []struct {
		name     string
		input    string
		expected []ScriptFragment
	}{
		{
			name:  "Simple mix of JS and CHTL JS",
			input: `const btn = {{.button}}; btn.addEventListener('click', () => { console.log({{btn.text}}); });`,
			expected: []ScriptFragment{
				{Type: FragmentJS, Content: "const btn = "},
				{Type: FragmentCHTLJS, Content: ".button"},
				{Type: FragmentJS, Content: "; btn.addEventListener('click', () => { console.log("},
				{Type: FragmentCHTLJS, Content: "btn.text"},
				{Type: FragmentJS, Content: "); });"},
			},
		},
		{
			name:  "Starts with CHTL JS",
			input: `{{#app}}.innerHTML = 'loaded';`,
			expected: []ScriptFragment{
				{Type: FragmentCHTLJS, Content: "#app"},
				{Type: FragmentJS, Content: ".innerHTML = 'loaded';"},
			},
		},
		{
			name:  "Ends with CHTL JS",
			input: `console.log('the value is', {{someVar}});`,
			expected: []ScriptFragment{
				{Type: FragmentJS, Content: "console.log('the value is', "},
				{Type: FragmentCHTLJS, Content: "someVar"},
				{Type: FragmentJS, Content: ");"},
			},
		},
		{
			name:  "Only JS",
			input: `document.body.style.backgroundColor = 'red';`,
			expected: []ScriptFragment{
				{Type: FragmentJS, Content: "document.body.style.backgroundColor = 'red';"},
			},
		},
		{
			name:  "Only CHTL JS",
			input: `{{myObject}}`,
			expected: []ScriptFragment{
				{Type: FragmentCHTLJS, Content: "myObject"},
			},
		},
		{
			name:  "Unterminated CHTL JS block",
			input: `const a = {{ a + 1`,
			expected: []ScriptFragment{
				{Type: FragmentJS, Content: "const a = "},
				{Type: FragmentCHTLJS, Content: " a + 1"},
			},
		},
		{
			name:  "Empty input",
			input: "",
			expected: []ScriptFragment(nil),
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			s := New()
			fragments := s.ScanScript(tt.input)

			if !reflect.DeepEqual(fragments, tt.expected) {
				t.Errorf("ScanScript() got = %v, want %v", fragments, tt.expected)
			}
		})
	}
}

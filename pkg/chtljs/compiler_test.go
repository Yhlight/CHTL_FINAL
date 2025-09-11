package chtljs

import "testing"

func TestCompileFragment(t *testing.T) {
	tests := []struct {
		name     string
		input    string
		expected string
	}{
		{
			name:     "Class selector",
			input:    ".my-class",
			expected: "document.querySelector('.my-class')",
		},
		{
			name:     "ID selector",
			input:    "#app-root",
			expected: "document.querySelector('#app-root')",
		},
		{
			name:     "Tag selector",
			input:    "button",
			expected: "document.querySelectorAll('button')",
		},
		{
			name:     "Fragment with whitespace",
			input:    "  .whitespace   ",
			expected: "document.querySelector('.whitespace')",
		},
	}

	c := NewCompiler()

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			result, err := c.CompileFragment(tt.input)
			if err != nil {
				t.Fatalf("CompileFragment failed with error: %v", err)
			}
			if result != tt.expected {
				t.Errorf("CompileFragment() got = %v, want %v", result, tt.expected)
			}
		})
	}
}

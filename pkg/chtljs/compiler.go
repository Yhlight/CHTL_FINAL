package chtljs

import (
	"fmt"
	"strings"
)

// Compiler for CHTL JS fragments.
type Compiler struct{}

// NewCompiler creates a new CHTL JS compiler.
func NewCompiler() *Compiler {
	return &Compiler{}
}

// CompileFragment takes a single CHTL JS fragment (the content from within {{...}})
// and compiles it to standard JavaScript.
func (c *Compiler) CompileFragment(fragment string) (string, error) {
	// This is a very simplified implementation for the first step.
	// A full implementation would require a proper lexer and parser.

	trimmedFragment := strings.TrimSpace(fragment)

	switch {
	case strings.HasPrefix(trimmedFragment, "."):
		// Class selector: {{.box}} -> document.querySelector('.box')
		return fmt.Sprintf("document.querySelector('%s')", trimmedFragment), nil
	case strings.HasPrefix(trimmedFragment, "#"):
		// ID selector: {{#app}} -> document.querySelector('#app')
		return fmt.Sprintf("document.querySelector('%s')", trimmedFragment), nil
	default:
		// For now, assume anything else is a tag name.
		// {{button}} -> document.querySelectorAll('button')
		// We will ignore more complex selectors like `{{box}}` auto-detection for now.
		return fmt.Sprintf("document.querySelectorAll('%s')", trimmedFragment), nil
	}
}

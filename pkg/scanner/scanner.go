package scanner

import (
	"fmt"
	"strings"
)

type FragmentType int

const (
	FragmentJS FragmentType = iota
	FragmentCHTLJS
)

type ScriptFragment struct {
	Type    FragmentType
	Content string
}

// Scanner is responsible for splitting a source file into different language fragments.
type Scanner struct {
	placeholderCount int
	Replaced         map[string]string
}

func New() *Scanner {
	return &Scanner{
		Replaced: make(map[string]string),
	}
}

// ScanAndReplace takes an input string, replaces parts of it with placeholders,
// and returns the modified string and a map of the replaced fragments.
// For now, it's designed to replace standard JS within a script.
func (s *Scanner) ScanAndReplace(input string) string {
	var result strings.Builder
	fragments := s.ScanScript(input)

	for _, frag := range fragments {
		if frag.Type == FragmentJS {
			// This is a JS fragment, replace it with a placeholder
			placeholder := fmt.Sprintf("__CHTL_JS_PLACEHOLDER_%d__", s.placeholderCount)
			s.placeholderCount++
			s.Replaced[placeholder] = frag.Content
			result.WriteString(placeholder)
		} else {
			// This is a CHTL JS fragment, keep it as is, but with delimiters
			result.WriteString("{{")
			result.WriteString(frag.Content)
			result.WriteString("}}")
		}
	}

	return result.String()
}

// ScanScript takes a string of script content and splits it into JS and CHTL JS fragments.
func (s *Scanner) ScanScript(input string) []ScriptFragment {
	var fragments []ScriptFragment
	var currentPos int
	var inCHTLJS bool

	for i := 0; i < len(input); {
		if !inCHTLJS {
			if i+1 < len(input) && input[i:i+2] == "{{" {
				if i > currentPos {
					fragments = append(fragments, ScriptFragment{
						Type:    FragmentJS,
						Content: input[currentPos:i],
					})
				}
				i += 2
				currentPos = i
				inCHTLJS = true
			} else {
				i++
			}
		} else {
			if i+1 < len(input) && input[i:i+2] == "}}" {
				if i > currentPos {
					fragments = append(fragments, ScriptFragment{
						Type:    FragmentCHTLJS,
						Content: strings.TrimSpace(input[currentPos:i]),
					})
				}
				i += 2
				currentPos = i
				inCHTLJS = false
			} else {
				i++
			}
		}
	}

	if currentPos < len(input) {
		fragType := FragmentJS
		if inCHTLJS {
			fragType = FragmentCHTLJS
		}
		fragments = append(fragments, ScriptFragment{
			Type:    fragType,
			Content: input[currentPos:],
		})
	}

	return fragments
}

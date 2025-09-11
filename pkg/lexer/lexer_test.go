package lexer

import (
	"testing"
)

func TestNextToken(t *testing.T) {
	input := `
html {
    body {
        div {
            id: "main";
            class: box-class;
        }
    }
}
`
	tests := []struct {
		expectedType    TokenType
		expectedLiteral string
	}{
		{IDENT, "html"}, {LBRACE, "{"},
		{IDENT, "body"}, {LBRACE, "{"},
		{IDENT, "div"}, {LBRACE, "{"},
		{IDENT, "id"}, {COLON, ":"}, {STRING, "main"}, {SEMICOLON, ";"},
		{IDENT, "class"}, {COLON, ":"}, {IDENT, "box-class"}, {SEMICOLON, ";"},
		{RBRACE, "}"},
		{RBRACE, "}"},
		{RBRACE, "}"},
		{EOF, ""},
	}

	l := New(input)

	for i, tt := range tests {
		tok := l.NextToken()

		if tok.Type != tt.expectedType {
			t.Fatalf("tests[%d] - tokentype wrong. expected=%q, got=%q (literal: %q)",
				i, tt.expectedType, tok.Type, tok.Literal)
		}

		if tok.Literal != tt.expectedLiteral {
			t.Fatalf("tests[%d] - literal wrong. expected=%q, got=%q",
				i, tt.expectedLiteral, tok.Literal)
		}
	}
}

func TestComplexIdentifiers(t *testing.T) {
	input := `width-em: 100px;`
	tests := []struct {
		expectedType    TokenType
		expectedLiteral string
	}{
		{IDENT, "width-em"},
		{COLON, ":"},
		{NUMBER, "100"},
		{IDENT, "px"},
		{SEMICOLON, ";"},
		{EOF, ""},
	}

	l := New(input)
	for i, tt := range tests {
		tok := l.NextToken()

		if tok.Type != tt.expectedType {
			t.Fatalf("tests[%d] - tokentype wrong. expected=%q, got=%q (literal: %q)",
				i, tt.expectedType, tok.Type, tok.Literal)
		}

		if tok.Literal != tt.expectedLiteral {
			t.Fatalf("tests[%d] - literal wrong. expected=%q, got=%q",
				i, tt.expectedLiteral, tok.Literal)
		}
	}
}

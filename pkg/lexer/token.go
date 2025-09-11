package lexer

// TokenType is a string representing the type of a token.
type TokenType string

// Token represents a single token in the source code.
type Token struct {
	Type    TokenType
	Literal string
}

const (
	ILLEGAL TokenType = "ILLEGAL"
	EOF     TokenType = "EOF"

	// --- Identifiers & Literals ---
	IDENT  TokenType = "IDENT"
	STRING TokenType = "STRING"
	NUMBER TokenType = "NUMBER"

	// --- Operators ---
	ASSIGN   TokenType = "="
	COLON    TokenType = ":"
	PLUS     TokenType = "+"
	MINUS    TokenType = "-"
	BANG     TokenType = "!"
	ASTERISK TokenType = "*"
	SLASH    TokenType = "/"
	LT       TokenType = "<"
	GT       TokenType = ">"
	QUESTION TokenType = "?"

	// --- Delimiters ---
	LBRACE    TokenType = "{"
	RBRACE    TokenType = "}"
	LBRACKET  TokenType = "["
	RBRACKET  TokenType = "]"
	LPAREN    TokenType = "("
	RPAREN    TokenType = ")"
	COMMA     TokenType = ","
	AT        TokenType = "@"
	SEMICOLON TokenType = ";"
	DOT       TokenType = "."
	HASH      TokenType = "#"
	AMPERSAND TokenType = "&"

	// --- Keywords ---
	FROM TokenType = "from"
	AS   TokenType = "as"

	// --- Comments ---
	COMMENT     TokenType = "COMMENT"
	GEN_COMMENT TokenType = "GEN_COMMENT"
)

var keywords = map[string]TokenType{
	"from": FROM,
	"as":   AS,
}

func LookupIdent(ident string) TokenType {
	if tok, ok := keywords[ident]; ok {
		return tok
	}
	return IDENT
}

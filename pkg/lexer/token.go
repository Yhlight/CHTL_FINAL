package lexer

type TokenType string

type Token struct {
	Type     TokenType
	Literal  string
	Position int // The starting position of the token in the input string
}

const (
	ILLEGAL TokenType = "ILLEGAL"
	EOF     TokenType = "EOF"
	IDENT   TokenType = "IDENT"
	STRING  TokenType = "STRING"
	NUMBER  TokenType = "NUMBER"
	ASSIGN  TokenType = "="
	COLON   TokenType = ":"
	PLUS    TokenType = "+"
	MINUS   TokenType = "-"
	BANG    TokenType = "!"
	ASTERISK TokenType = "*"
	SLASH    TokenType = "/"
	LBRACE    TokenType = "{"
	RBRACE    TokenType = "}"
	LBRACKET  TokenType = "["
	RBRACKET  TokenType = "]"
	AT        TokenType = "@"
	SEMICOLON TokenType = ";"
	DOT       TokenType = "."
	HASH      TokenType = "#"
	AMPERSAND TokenType = "&"
	FROM      TokenType = "from"
	AS        TokenType = "as"
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

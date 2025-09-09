package lexer

// TokenType 词法单元类型
type TokenType int

const (
	// 基础类型
	EOF TokenType = iota
	ILLEGAL
	IDENTIFIER
	STRING
	NUMBER
	COMMENT

	// 分隔符
	LEFT_BRACE    // {
	RIGHT_BRACE   // }
	LEFT_BRACKET  // [
	RIGHT_BRACKET // ]
	LEFT_PAREN    // (
	RIGHT_PAREN   // )
	SEMICOLON     // ;
	COLON         // :
	COMMA         // ,
	DOT           // .
	EQUALS        // =
	QUESTION      // ?
	PIPE          // |
	AMPERSAND     // &
	HASH          // #
	AT            // @
	DOLLAR        // $
	UNDERSCORE    // _

	// 运算符
	PLUS          // +
	MINUS         // -
	MULTIPLY      // *
	DIVIDE        // /
	MODULO        // %
	POWER         // **
	ASSIGN        // :=
	EQUAL         // ==
	NOT_EQUAL     // !=
	LESS          // <
	LESS_EQUAL    // <=
	GREATER       // >
	GREATER_EQUAL // >=
	AND           // &&
	OR            // ||
	NOT           // !

	// 关键字
	KEYWORD_TEXT
	KEYWORD_STYLE
	KEYWORD_SCRIPT
	KEYWORD_TEMPLATE
	KEYWORD_CUSTOM
	KEYWORD_ELEMENT
	KEYWORD_VAR
	KEYWORD_FROM
	KEYWORD_ORIGIN
	KEYWORD_IMPORT
	KEYWORD_NAMESPACE
	KEYWORD_CONFIGURATION
	KEYWORD_USE
	KEYWORD_INHERIT
	KEYWORD_DELETE
	KEYWORD_INSERT
	KEYWORD_FUNCTION
	KEYWORD_LET
	KEYWORD_CONST
	KEYWORD_IF
	KEYWORD_FOR
	KEYWORD_WHILE
	KEYWORD_LOAD
	KEYWORD_LOCAL
	KEYWORD_SELECT
	KEYWORD_DELEGATE
	KEYWORD_ANIMATE
	KEYWORD_VIRTUAL
	KEYWORD_ROUTE
	KEYWORD_AFTER
	KEYWORD_BEFORE
	KEYWORD_REPLACE
	KEYWORD_AT_TOP
	KEYWORD_AT_BOTTOM
	KEYWORD_AS
	KEYWORD_EXCEPT
	KEYWORD_HTML5
	KEYWORD_HTML
	KEYWORD_JAVASCRIPT
	KEYWORD_CHTL
	KEYWORD_CJMOD
	KEYWORD_CONFIG

	// CHTL JS 关键字
	KEYWORD_VIR
	KEYWORD_LISTEN
	KEYWORD_ROUTER
	KEYWORD_UTIL
	KEYWORD_THEN
	KEYWORD_CHANGE
	KEYWORD_FILELOADER
	KEYWORD_INEVERAWAY
	KEYWORD_PRINTMYLOVE

	// 特殊符号
	ARROW          // ->
	DOUBLE_ARROW   // =>
	TRIPLE_DOT     // ...
	DOUBLE_COLON   // ::
	DOUBLE_PIPE    // ||
	DOUBLE_AND     // &&
)

// Token 词法单元
type Token struct {
	Type     TokenType
	Literal  string
	Position Position
}

// Position 位置信息
type Position struct {
	Line   int
	Column int
	Offset int
}

// String 返回 Token 类型的字符串表示
func (t TokenType) String() string {
	switch t {
	case EOF:
		return "EOF"
	case ILLEGAL:
		return "ILLEGAL"
	case IDENTIFIER:
		return "IDENTIFIER"
	case STRING:
		return "STRING"
	case NUMBER:
		return "NUMBER"
	case COMMENT:
		return "COMMENT"
	case LEFT_BRACE:
		return "LEFT_BRACE"
	case RIGHT_BRACE:
		return "RIGHT_BRACE"
	case LEFT_BRACKET:
		return "LEFT_BRACKET"
	case RIGHT_BRACKET:
		return "RIGHT_BRACKET"
	case LEFT_PAREN:
		return "LEFT_PAREN"
	case RIGHT_PAREN:
		return "RIGHT_PAREN"
	case SEMICOLON:
		return "SEMICOLON"
	case COLON:
		return "COLON"
	case COMMA:
		return "COMMA"
	case DOT:
		return "DOT"
	case EQUALS:
		return "EQUALS"
	case QUESTION:
		return "QUESTION"
	case PIPE:
		return "PIPE"
	case AMPERSAND:
		return "AMPERSAND"
	case HASH:
		return "HASH"
	case AT:
		return "AT"
	case DOLLAR:
		return "DOLLAR"
	case UNDERSCORE:
		return "UNDERSCORE"
	case PLUS:
		return "PLUS"
	case MINUS:
		return "MINUS"
	case MULTIPLY:
		return "MULTIPLY"
	case DIVIDE:
		return "DIVIDE"
	case MODULO:
		return "MODULO"
	case POWER:
		return "POWER"
	case ASSIGN:
		return "ASSIGN"
	case EQUAL:
		return "EQUAL"
	case NOT_EQUAL:
		return "NOT_EQUAL"
	case LESS:
		return "LESS"
	case LESS_EQUAL:
		return "LESS_EQUAL"
	case GREATER:
		return "GREATER"
	case GREATER_EQUAL:
		return "GREATER_EQUAL"
	case AND:
		return "AND"
	case OR:
		return "OR"
	case NOT:
		return "NOT"
	case KEYWORD_TEXT:
		return "KEYWORD_TEXT"
	case KEYWORD_STYLE:
		return "KEYWORD_STYLE"
	case KEYWORD_SCRIPT:
		return "KEYWORD_SCRIPT"
	case KEYWORD_TEMPLATE:
		return "KEYWORD_TEMPLATE"
	case KEYWORD_CUSTOM:
		return "KEYWORD_CUSTOM"
	case KEYWORD_ELEMENT:
		return "KEYWORD_ELEMENT"
	case KEYWORD_VAR:
		return "KEYWORD_VAR"
	case KEYWORD_ORIGIN:
		return "KEYWORD_ORIGIN"
	case KEYWORD_IMPORT:
		return "KEYWORD_IMPORT"
	case KEYWORD_NAMESPACE:
		return "KEYWORD_NAMESPACE"
	case KEYWORD_CONFIGURATION:
		return "KEYWORD_CONFIGURATION"
	case KEYWORD_USE:
		return "KEYWORD_USE"
	case KEYWORD_INHERIT:
		return "KEYWORD_INHERIT"
	case KEYWORD_DELETE:
		return "KEYWORD_DELETE"
	case KEYWORD_INSERT:
		return "KEYWORD_INSERT"
	case KEYWORD_FUNCTION:
		return "KEYWORD_FUNCTION"
	case KEYWORD_LET:
		return "KEYWORD_LET"
	case KEYWORD_CONST:
		return "KEYWORD_CONST"
	case KEYWORD_IF:
		return "KEYWORD_IF"
	case KEYWORD_FOR:
		return "KEYWORD_FOR"
	case KEYWORD_WHILE:
		return "KEYWORD_WHILE"
	case KEYWORD_LOAD:
		return "KEYWORD_LOAD"
	case KEYWORD_LOCAL:
		return "KEYWORD_LOCAL"
	case KEYWORD_SELECT:
		return "KEYWORD_SELECT"
	case KEYWORD_DELEGATE:
		return "KEYWORD_DELEGATE"
	case KEYWORD_ANIMATE:
		return "KEYWORD_ANIMATE"
	case KEYWORD_VIRTUAL:
		return "KEYWORD_VIRTUAL"
	case KEYWORD_ROUTE:
		return "KEYWORD_ROUTE"
	case KEYWORD_AFTER:
		return "KEYWORD_AFTER"
	case KEYWORD_BEFORE:
		return "KEYWORD_BEFORE"
	case KEYWORD_REPLACE:
		return "KEYWORD_REPLACE"
	case KEYWORD_AT_TOP:
		return "KEYWORD_AT_TOP"
	case KEYWORD_AT_BOTTOM:
		return "KEYWORD_AT_BOTTOM"
	case KEYWORD_FROM:
		return "KEYWORD_FROM"
	case KEYWORD_AS:
		return "KEYWORD_AS"
	case KEYWORD_EXCEPT:
		return "KEYWORD_EXCEPT"
	case KEYWORD_HTML5:
		return "KEYWORD_HTML5"
	case KEYWORD_HTML:
		return "KEYWORD_HTML"
	case KEYWORD_JAVASCRIPT:
		return "KEYWORD_JAVASCRIPT"
	case KEYWORD_CHTL:
		return "KEYWORD_CHTL"
	case KEYWORD_CJMOD:
		return "KEYWORD_CJMOD"
	case KEYWORD_CONFIG:
		return "KEYWORD_CONFIG"
	case KEYWORD_VIR:
		return "KEYWORD_VIR"
	case KEYWORD_LISTEN:
		return "KEYWORD_LISTEN"
	case KEYWORD_ROUTER:
		return "KEYWORD_ROUTER"
	case KEYWORD_UTIL:
		return "KEYWORD_UTIL"
	case KEYWORD_THEN:
		return "KEYWORD_THEN"
	case KEYWORD_CHANGE:
		return "KEYWORD_CHANGE"
	case KEYWORD_FILELOADER:
		return "KEYWORD_FILELOADER"
	case KEYWORD_INEVERAWAY:
		return "KEYWORD_INEVERAWAY"
	case KEYWORD_PRINTMYLOVE:
		return "KEYWORD_PRINTMYLOVE"
	case ARROW:
		return "ARROW"
	case DOUBLE_ARROW:
		return "DOUBLE_ARROW"
	case TRIPLE_DOT:
		return "TRIPLE_DOT"
	case DOUBLE_COLON:
		return "DOUBLE_COLON"
	case DOUBLE_PIPE:
		return "DOUBLE_PIPE"
	case DOUBLE_AND:
		return "DOUBLE_AND"
	default:
		return "UNKNOWN"
	}
}

// IsKeyword 检查是否为关键字
func (t TokenType) IsKeyword() bool {
	return t >= KEYWORD_TEXT && t <= KEYWORD_PRINTMYLOVE
}

// IsOperator 检查是否为运算符
func (t TokenType) IsOperator() bool {
	return t >= PLUS && t <= NOT
}

// IsDelimiter 检查是否为分隔符
func (t TokenType) IsDelimiter() bool {
	return t >= LEFT_BRACE && t <= UNDERSCORE
}
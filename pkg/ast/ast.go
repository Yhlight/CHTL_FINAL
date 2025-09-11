package ast

import (
	"chtl/pkg/lexer"
	"fmt"
	"strings"
)

type Node interface {
	TokenLiteral() string
	String() string
}
type Statement interface {
	Node
	statementNode()
}
type Expression interface {
	Node
	expressionNode()
}

type Program struct {
	Statements []Statement
	Templates  map[string]*TemplateDefinition
}

func (p *Program) TokenLiteral() string {
	if len(p.Statements) > 0 {
		return p.Statements[0].TokenLiteral()
	}
	return ""
}
func (p *Program) String() string {
	var out strings.Builder
	for _, s := range p.Statements {
		out.WriteString(s.String())
	}
	return out.String()
}

type Element struct {
	Token lexer.Token
	Name  string
	Body  *BlockStatement
}

func (e *Element) statementNode()       {}
func (e *Element) TokenLiteral() string { return e.Token.Literal }
func (e *Element) String() string {
	var out strings.Builder
	out.WriteString(e.Name)
	if e.Body != nil {
		out.WriteString(e.Body.String())
	}
	return out.String()
}

type ImportStatement struct {
	Token      lexer.Token // The '[' token
	ImportType string      // e.g., "@Chtl", "@Html"
	Path       Expression
	Alias      *Identifier
}

func (is *ImportStatement) statementNode()       {}
func (is *ImportStatement) TokenLiteral() string { return is.Token.Literal }
func (is *ImportStatement) String() string {
	var out strings.Builder
	out.WriteString("[Import] " + is.ImportType + " from " + is.Path.String())
	if is.Alias != nil {
		out.WriteString(" as " + is.Alias.String())
	}
	return out.String()
}

type Text struct {
	Token   lexer.Token
	Content Expression
}

func (t *Text) statementNode()       {}
func (t *Text) TokenLiteral() string { return t.Token.Literal }
func (t *Text) String() string       { return "text { " + t.Content.String() + " }" }

type Attribute struct {
	Token lexer.Token
	Name  string
	Value Expression
}

func (a *Attribute) statementNode()       {}
func (a *Attribute) TokenLiteral() string { return a.Token.Literal }
func (a *Attribute) String() string       { return a.Name + ": " + a.Value.String() + ";" }

type BlockStatement struct {
	Token      lexer.Token
	Statements []Statement
}

func (bs *BlockStatement) statementNode()       {}
func (bs *BlockStatement) TokenLiteral() string { return bs.Token.Literal }
func (bs *BlockStatement) String() string {
	var out strings.Builder
	out.WriteString(" { ")
	for _, s := range bs.Statements {
		out.WriteString(s.String())
	}
	out.WriteString(" }")
	return out.String()
}

type Style struct {
	Token lexer.Token
	Body  *BlockStatement
}

func (s *Style) statementNode()       {}
func (s *Style) TokenLiteral() string { return s.Token.Literal }
func (s *Style) String() string       { return "style" + s.Body.String() }

type StyleRule struct {
	Token lexer.Token
	Name  string
	Value Expression
}

func (sr *StyleRule) statementNode()       {}
func (sr *StyleRule) TokenLiteral() string { return sr.Token.Literal }
func (sr *StyleRule) String() string       { return sr.Name + ": " + sr.Value.String() + ";" }

type Ruleset struct {
	Token    lexer.Token
	Selector Expression
	Body     *BlockStatement
}

func (rs *Ruleset) statementNode()       {}
func (rs *Ruleset) TokenLiteral() string { return rs.Token.Literal }
func (rs *Ruleset) String() string       { return rs.Selector.String() + rs.Body.String() }

type TemplateDefinition struct {
	Token        lexer.Token
	TemplateType string
	Name         *Identifier
	Body         *BlockStatement
}

func (td *TemplateDefinition) statementNode()       {}
func (td *TemplateDefinition) TokenLiteral() string { return td.Token.Literal }
func (td *TemplateDefinition) String() string {
	return "[Template] @" + td.TemplateType + " " + td.Name.String() + td.Body.String()
}

type TemplateUsage struct {
	Token        lexer.Token
	TemplateType string
	Name         *Identifier
}

func (tu *TemplateUsage) statementNode()       {}
func (tu *TemplateUsage) expressionNode()      {}
func (tu *TemplateUsage) TokenLiteral() string { return tu.Token.Literal }
func (tu *TemplateUsage) String() string       { return "@" + tu.TemplateType + " " + tu.Name.String() + ";" }

type PrefixExpression struct {
	Token    lexer.Token
	Operator string
	Right    Expression
}

func (pe *PrefixExpression) expressionNode()      {}
func (pe *PrefixExpression) TokenLiteral() string { return pe.Token.Literal }
func (pe *PrefixExpression) String() string       { return pe.Operator + pe.Right.String() }

type InfixExpression struct {
	Token    lexer.Token
	Left     Expression
	Operator string
	Right    Expression
}

func (ie *InfixExpression) expressionNode()      {}
func (ie *InfixExpression) TokenLiteral() string { return ie.Token.Literal }
func (ie *InfixExpression) String() string {
	return "(" + ie.Left.String() + " " + ie.Operator + " " + ie.Right.String() + ")"
}

type OriginStatement struct {
	Token   lexer.Token
	Type    string
	Content string
}

func (os *OriginStatement) statementNode()       {}
func (os *OriginStatement) TokenLiteral() string { return os.Token.Literal }
func (os *OriginStatement) String() string {
	return fmt.Sprintf("[Origin] @%s { %s }", os.Type, os.Content)
}

type ScriptStatement struct {
	Token   lexer.Token
	Content string
}

func (ss *ScriptStatement) statementNode()       {}
func (ss *ScriptStatement) TokenLiteral() string { return ss.Token.Literal }
func (ss *ScriptStatement) String() string       { return "script { " + ss.Content + " }" }

type ConditionalExpression struct {
	Token       lexer.Token
	Condition   Expression
	Consequence Expression
	Alternative Expression
}

func (ce *ConditionalExpression) expressionNode()      {}
func (ce *ConditionalExpression) TokenLiteral() string { return ce.Token.Literal }
func (ce *ConditionalExpression) String() string {
	var out strings.Builder
	out.WriteString("(")
	out.WriteString(ce.Condition.String())
	out.WriteString(" ? ")
	out.WriteString(ce.Consequence.String())
	out.WriteString(" : ")
	out.WriteString(ce.Alternative.String())
	out.WriteString(")")
	return out.String()
}

type CallExpression struct {
	Token     lexer.Token
	Function  Expression
	Arguments []Expression
}

func (ce *CallExpression) expressionNode()      {}
func (ce *CallExpression) TokenLiteral() string { return ce.Token.Literal }
func (ce *CallExpression) String() string {
	var out strings.Builder
	args := []string{}
	for _, a := range ce.Arguments {
		args = append(args, a.String())
	}
	out.WriteString(ce.Function.String())
	out.WriteString("(")
	out.WriteString(strings.Join(args, ", "))
	out.WriteString(")")
	return out.String()
}

type Identifier struct {
	Token lexer.Token
	Value string
}

func (i *Identifier) expressionNode()      {}
func (i *Identifier) TokenLiteral() string { return i.Token.Literal }
func (i *Identifier) String() string       { return i.Value }

type StringLiteral struct {
	Token lexer.Token
	Value string
}

func (sl *StringLiteral) expressionNode()      {}
func (sl *StringLiteral) TokenLiteral() string { return sl.Token.Literal }
func (sl *StringLiteral) String() string       { return `"` + sl.Value + `"` }

type NumberLiteral struct {
	Token lexer.Token
	Value string
	Unit  string
}

func (nl *NumberLiteral) expressionNode()      {}
func (nl *NumberLiteral) TokenLiteral() string { return nl.Token.Literal }
func (nl *NumberLiteral) String() string       { return nl.Value + nl.Unit }

type RawExpression struct {
	Tokens []lexer.Token
}

func (re *RawExpression) expressionNode() {}
func (re *RawExpression) TokenLiteral() string {
	if len(re.Tokens) > 0 {
		return re.Tokens[0].Literal
	}
	return ""
}
func (re *RawExpression) String() string {
	var out strings.Builder
	for i, t := range re.Tokens {
		out.WriteString(t.Literal)
		if i < len(re.Tokens)-1 {
			out.WriteString(" ")
		}
	}
	return out.String()
}

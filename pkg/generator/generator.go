package generator

import (
	"chtl/pkg/ast"
	"chtl/pkg/parser"
	"fmt"
	"strings"
)

type Generator struct {
	globalStyles strings.Builder
	templates    map[string]*ast.TemplateDefinition
}

func New() *Generator {
	return &Generator{}
}

func (g *Generator) Generate(program *ast.Program) (string, error) {
	g.templates = program.Templates
	var b strings.Builder
	err := g.generateNode(program, &b)
	if err != nil {
		return "", err
	}

	if g.globalStyles.Len() > 0 {
		return "<style>\n" + g.globalStyles.String() + "</style>\n" + b.String(), nil
	}

	return b.String(), nil
}

func (g *Generator) generateNode(node ast.Node, b *strings.Builder) error {
	switch n := node.(type) {
	case *ast.Program:
		for _, stmt := range n.Statements {
			if err := g.generateNode(stmt, b); err != nil {
				return err
			}
		}
	case *ast.Element:
		return g.generateElement(n, b)
	case *ast.Text:
		content, err := g.expressionToString(n.Content, false)
		if err != nil { return err }
		b.WriteString(content)
	case *ast.Style, *ast.TemplateUsage:
		return nil
	default:
		return fmt.Errorf("unknown node type for generation: %T", n)
	}
	return nil
}

func (g *Generator) generateElement(n *ast.Element, b *strings.Builder) error {
	attrs := make(map[string]string)
	var bodyStmts []ast.Statement
	var inlineStyles []string

	var processStyleBody func(body *ast.BlockStatement) error
	processStyleBody = func(body *ast.BlockStatement) error {
		for _, styleStmt := range body.Statements {
			switch ss := styleStmt.(type) {
			case *ast.StyleRule:
				val, err := g.expressionToString(ss.Value, true) // Pass true to enable calc() wrapping
				if err != nil { return err }
				inlineStyles = append(inlineStyles, fmt.Sprintf("%s: %s", ss.Name, val))
			case *ast.Ruleset:
				selector, err := g.expressionToString(ss.Selector, false)
				if err != nil { return err }
				var rulesetBuilder strings.Builder
				for _, rule := range ss.Body.Statements {
					if r, ok := rule.(*ast.StyleRule); ok {
						val, err := g.expressionToString(r.Value, true)
						if err != nil { return err }
						rulesetBuilder.WriteString(fmt.Sprintf("  %s: %s;\n", r.Name, val))
					}
				}
				g.globalStyles.WriteString(fmt.Sprintf("%s {\n%s}\n", selector, rulesetBuilder.String()))
				if prefix, ok := ss.Selector.(*ast.PrefixExpression); ok {
					if ident, ok := prefix.Right.(*ast.Identifier); ok {
						if prefix.Operator == "." {
							attrs["class"] = strings.TrimSpace(attrs["class"] + " " + ident.Value)
						} else if prefix.Operator == "#" {
							attrs["id"] = ident.Value
						}
					}
				}
			case *ast.TemplateUsage:
				template, ok := g.templates[ss.Name.Value]
				if !ok { return fmt.Errorf("template %q not found", ss.Name.Value) }
				if err := processStyleBody(template.Body); err != nil { return err }
			}
		}
		return nil
	}

	if n.Body != nil {
		for _, stmt := range n.Body.Statements {
			switch s := stmt.(type) {
			case *ast.Attribute:
				val, err := g.expressionToString(s.Value, false)
				if err != nil { return err }
				attrs[s.Name] = val
			case *ast.Style:
				if err := processStyleBody(s.Body); err != nil { return err }
			default:
				bodyStmts = append(bodyStmts, stmt)
			}
		}
	}

	if len(inlineStyles) > 0 {
		attrs["style"] = strings.Join(inlineStyles, "; ")
	}

	b.WriteString("<")
	b.WriteString(n.Name)
	for key, val := range attrs {
		b.WriteString(fmt.Sprintf(` %s="%s"`, key, val))
	}
	b.WriteString(">")

	for _, stmt := range bodyStmts {
		if err := g.generateNode(stmt, b); err != nil {
			return err
		}
	}

	b.WriteString("</")
	b.WriteString(n.Name)
	b.WriteString(">")
	return nil
}

func (g *Generator) expressionToString(exp ast.Expression, useCalc bool) (string, error) {
	switch e := exp.(type) {
	case *ast.RawExpression:
		ep := parser.NewExpressionParser(e.Tokens)
		parsedExp := ep.Parse()
		if parsedExp == nil {
			return e.String(), nil
		}

		if _, ok := parsedExp.(*ast.InfixExpression); ok && useCalc {
			return "calc" + parsedExp.String(), nil
		}
		return parsedExp.String(), nil

	case *ast.StringLiteral: return e.Value, nil
	case *ast.Identifier: return e.Value, nil
	case *ast.NumberLiteral: return e.Value, nil
	case *ast.PrefixExpression:
		right, err := g.expressionToString(e.Right, useCalc)
		if err != nil { return "", err }
		return e.Operator + right, nil
	case *ast.InfixExpression:
		if useCalc {
			return "calc" + e.String(), nil
		}
		return e.String(), nil
	default:
		return "", fmt.Errorf("unhandled expression type for generation: %T", e)
	}
}

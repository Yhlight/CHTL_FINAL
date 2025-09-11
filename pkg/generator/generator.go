package generator

import (
	"chtl/pkg/ast"
	"chtl/pkg/lexer"
	"chtl/pkg/parser"
	"fmt"
	"regexp"
	"strconv"
	"strings"
)

type Generator struct {
	globalStyles  strings.Builder
	globalScripts strings.Builder
	templates     map[string]*ast.TemplateDefinition
}

func New() *Generator {
	return &Generator{}
}

func (g *Generator) Generate(program *ast.Program) (string, error) {
	g.templates = program.Templates
	var bodyBuilder strings.Builder
	err := g.generateNode(program, &bodyBuilder)
	if err != nil {
		return "", err
	}

	var finalHtml strings.Builder
	if g.globalStyles.Len() > 0 {
		finalHtml.WriteString("<style>\n")
		finalHtml.WriteString(g.globalStyles.String())
		finalHtml.WriteString("</style>\n")
	}

	finalHtml.WriteString(bodyBuilder.String())

	if g.globalScripts.Len() > 0 {
		finalHtml.WriteString("\n<script>\n")
		finalHtml.WriteString(g.globalScripts.String())
		finalHtml.WriteString("</script>\n")
	}

	return finalHtml.String(), nil
}

func (g *Generator) generateNode(node ast.Node, b *strings.Builder) error {
	switch n := node.(type) {
	case *ast.Program:
		for _, stmt := range n.Statements {
			if err := g.generateNode(stmt, b); err != nil {
				return err
			}
		}
	case *ast.OriginStatement:
		b.WriteString(n.Content)
	case *ast.Element:
		return g.generateElement(n, b)
	case *ast.Text:
		content, err := g.expressionToString(n.Content, false, nil)
		if err != nil {
			return err
		}
		b.WriteString(content)
	case *ast.Attribute:
		return nil
	case *ast.Style:
		return nil
	case *ast.Ruleset:
		return nil
	case *ast.TemplateUsage:
		return g.generateTemplateUsage(n, b)
	case *ast.ScriptStatement:
		return nil
	default:
		return fmt.Errorf("unknown node type for generation: %T", n)
	}
	return nil
}

func (g *Generator) generateTemplateUsage(tu *ast.TemplateUsage, b *strings.Builder) error {
	if tu.TemplateType == "Style" {
		return nil
	}

	template, ok := g.templates[tu.Name.Value]
	if !ok {
		return fmt.Errorf("template %q not found", tu.Name.Value)
	}

	if template.TemplateType != "Element" {
		return fmt.Errorf("template %q is a @%s template, not an @Element template", tu.Name.Value, template.TemplateType)
	}

	for _, stmt := range template.Body.Statements {
		if err := g.generateNode(stmt, b); err != nil {
			return err
		}
	}

	return nil
}

func (g *Generator) generateElement(n *ast.Element, b *strings.Builder) error {
	attrs := make(map[string]string)
	var inlineStyles []string
	var bodyContentStmts []ast.Statement

	if n.Body != nil {
		for _, stmt := range n.Body.Statements {
			switch s := stmt.(type) {
			case *ast.Attribute:
				val, err := g.expressionToString(s.Value, false, nil)
				if err != nil {
					return err
				}
				attrs[s.Name] = val
			case *ast.Style:
				if err := g.processStyleBody(s, attrs, &inlineStyles); err != nil {
					return err
				}
			case *ast.ScriptStatement:
				processedScript, err := g.processScriptContent(s.Content, attrs)
				if err != nil {
					return err
				}
				g.globalScripts.WriteString(processedScript)
				g.globalScripts.WriteString("\n")
			default:
				bodyContentStmts = append(bodyContentStmts, s)
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

	for _, stmt := range bodyContentStmts {
		if err := g.generateNode(stmt, b); err != nil {
			return err
		}
	}

	b.WriteString("</")
	b.WriteString(n.Name)
	b.WriteString(">")
	return nil
}

func (g *Generator) processStyleBody(style *ast.Style, attrs map[string]string, inlineStyles *[]string) error {
	styleCtx := make(map[string]ast.Expression)
	rulesets := []*ast.Ruleset{}
	templateUsages := []*ast.TemplateUsage{}

	if style.Body == nil {
		return nil
	}

	for _, styleStmt := range style.Body.Statements {
		switch ss := styleStmt.(type) {
		case *ast.StyleRule:
			styleCtx[ss.Name] = ss.Value
		case *ast.Ruleset:
			rulesets = append(rulesets, ss)
		case *ast.TemplateUsage:
			templateUsages = append(templateUsages, ss)
		}
	}

	for _, tu := range templateUsages {
		template, ok := g.templates[tu.Name.Value]
		if !ok {
			return fmt.Errorf("template %q not found", tu.Name.Value)
		}
		for _, stmt := range template.Body.Statements {
			if rule, ok := stmt.(*ast.StyleRule); ok {
				if _, exists := styleCtx[rule.Name]; !exists {
					styleCtx[rule.Name] = rule.Value
				}
			}
		}
	}

	for name, exp := range styleCtx {
		val, err := g.evaluateStyleExpression(exp, styleCtx)
		if err != nil {
			return err
		}
		*inlineStyles = append(*inlineStyles, fmt.Sprintf("%s: %s", name, val))
	}

	for _, ss := range rulesets {
		selector, err := g.expressionToString(ss.Selector, false, nil)
		if err != nil {
			return err
		}
		var rulesetBuilder strings.Builder
		for _, rule := range ss.Body.Statements {
			if r, ok := rule.(*ast.StyleRule); ok {
				val, err := g.expressionToString(r.Value, true, nil)
				if err != nil {
					return err
				}
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
	}
	return nil
}

func (g *Generator) expressionToString(exp ast.Expression, useCalc bool, ctx map[string]ast.Expression) (string, error) {
	switch e := exp.(type) {
	case *ast.RawExpression:
		p := parser.NewExpressionParser(e.Tokens)
		parsedExp := p.Parse()
		if parsedExp == nil {
			return e.String(), nil
		}
		return g.expressionToString(parsedExp, useCalc, ctx)

	case *ast.StringLiteral:
		return e.Value, nil
	case *ast.Identifier:
		return e.Value, nil
	case *ast.NumberLiteral:
		return e.Value + e.Unit, nil
	case *ast.PrefixExpression:
		right, err := g.expressionToString(e.Right, useCalc, ctx)
		if err != nil {
			return "", err
		}
		return e.Operator + right, nil
	case *ast.InfixExpression:
		left, err := g.expressionToString(e.Left, useCalc, ctx)
		if err != nil {
			return "", err
		}
		right, err := g.expressionToString(e.Right, useCalc, ctx)
		if err != nil {
			return "", err
		}
		str := fmt.Sprintf("(%s %s %s)", left, e.Operator, right)
		if useCalc {
			return "calc" + str, nil
		}
		return str, nil
	case *ast.CallExpression:
		val, err := g.evaluate(e, ctx)
		if err != nil {
			return "", err
		}
		return val.String(), nil
	default:
		return "", fmt.Errorf("unhandled expression type for string conversion: %T", e)
	}
}

var selectorRegex = regexp.MustCompile(`\{\{([^}]+)\}\}`)

func (g *Generator) processScriptContent(content string, attrs map[string]string) (string, error) {
	content = strings.TrimSpace(content)
	content = strings.ReplaceAll(content, "->", ".")

	output := selectorRegex.ReplaceAllStringFunc(content, func(match string) string {
		selector := strings.TrimSpace(selectorRegex.FindStringSubmatch(match)[1])

		if selector == "&" {
			if id, ok := attrs["id"]; ok {
				return fmt.Sprintf("document.getElementById('%s')", id)
			}
			if class, ok := attrs["class"]; ok {
				className := strings.Split(class, " ")[0]
				return fmt.Sprintf("document.querySelector('.%s')", className)
			}
			return "null /* CHTL Error: '&' used on element with no id or class */"
		}

		return fmt.Sprintf("document.querySelector('%s')", selector)
	})

	return output, nil
}

type StyleValue struct {
	Number   float64
	Unit     string
	Str      string
	IsNumber bool
}

func (v StyleValue) String() string {
	if v.IsNumber {
		return strconv.FormatFloat(v.Number, 'f', -1, 64) + v.Unit
	}
	return v.Str
}
func (g *Generator) evaluateStyleExpression(exp ast.Expression, ctx map[string]ast.Expression) (string, error) {
	isArithmetic := false
	if raw, ok := exp.(*ast.RawExpression); ok {
		for _, token := range raw.Tokens {
			if token.Type == lexer.PLUS || token.Type == lexer.MINUS || token.Type == lexer.ASTERISK || token.Type == lexer.SLASH {
				isArithmetic = true
				break
			}
		}
	}

	val, err := g.evaluate(exp, ctx)
	if err != nil {
		return "", err
	}

	if isArithmetic {
		return "calc" + val.String(), nil
	}
	return val.String(), nil
}

func (g *Generator) evaluate(exp ast.Expression, ctx map[string]ast.Expression) (StyleValue, error) {
	switch e := exp.(type) {
	case *ast.RawExpression:
		p := parser.NewExpressionParser(e.Tokens)
		parsedExp := p.Parse()
		if parsedExp == nil {
			return StyleValue{Str: e.String()}, nil
		}
		return g.evaluate(parsedExp, ctx)
	case *ast.Identifier:
		if ctx != nil {
			if valExp, ok := ctx[e.Value]; ok {
				return g.evaluate(valExp, ctx)
			}
		}
		return StyleValue{Str: e.Value}, nil
	case *ast.StringLiteral:
		return StyleValue{Str: e.Value}, nil
	case *ast.NumberLiteral:
		num, err := strconv.ParseFloat(e.Value, 64)
		if err != nil {
			return StyleValue{}, fmt.Errorf("could not parse number: %s", e.Value)
		}
		return StyleValue{IsNumber: true, Number: num, Unit: e.Unit}, nil
	case *ast.InfixExpression:
		left, err := g.evaluate(e.Left, ctx)
		if err != nil {
			return StyleValue{}, err
		}
		right, err := g.evaluate(e.Right, ctx)
		if err != nil {
			return StyleValue{}, err
		}

		if e.Operator == ">" {
			if left.IsNumber && right.IsNumber {
				if left.Number > right.Number {
					return StyleValue{Str: "true"}, nil
				}
				return StyleValue{Str: "false"}, nil
			}
		}
		return StyleValue{Str: "(" + left.String() + " " + e.Operator + " " + right.String() + ")"}, nil
	case *ast.ConditionalExpression:
		cond, err := g.evaluate(e.Condition, ctx)
		if err != nil {
			return StyleValue{}, err
		}
		if cond.Str == "true" {
			return g.evaluate(e.Consequence, ctx)
		}
		return g.evaluate(e.Alternative, ctx)
	case *ast.CallExpression:
		return g.evaluateCallExpression(e, ctx)
	}
	return StyleValue{}, fmt.Errorf("unknown expression type in evaluate: %T", exp)
}

func (g *Generator) evaluateCallExpression(ce *ast.CallExpression, ctx map[string]ast.Expression) (StyleValue, error) {
	templateName, ok := ce.Function.(*ast.Identifier)
	if !ok {
		return StyleValue{}, fmt.Errorf("template call must be an identifier, got %T", ce.Function)
	}

	template, ok := g.templates[templateName.Value]
	if !ok {
		return StyleValue{}, fmt.Errorf("template %q not found", templateName.Value)
	}
	if template.TemplateType != "Var" {
		return StyleValue{}, fmt.Errorf("template %q is not a @Var template", templateName.Value)
	}

	if len(ce.Arguments) != 1 {
		return StyleValue{}, fmt.Errorf("template @Var call expects 1 argument, got %d", len(ce.Arguments))
	}
	varName, ok := ce.Arguments[0].(*ast.Identifier)
	if !ok {
		return StyleValue{}, fmt.Errorf("template @Var argument must be an identifier, got %T", ce.Arguments[0])
	}

	for _, stmt := range template.Body.Statements {
		if rule, ok := stmt.(*ast.StyleRule); ok {
			if rule.Name == varName.Value {
				return g.evaluate(rule.Value, ctx)
			}
		}
	}

	return StyleValue{}, fmt.Errorf("variable %q not found in @Var template %q", varName.Value, templateName.Value)
}

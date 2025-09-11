import re
from antlr4 import InputStream, CommonTokenStream
from CHTL.ast.nodes import StyleUsageNode
from CHTL.css_expr.generated.CSSValueExpressionLexer import CSSValueExpressionLexer
from CHTL.css_expr.generated.CSSValueExpressionParser import CSSValueExpressionParser
from CHTL.css_expr.ast_builder import ExpressionAstBuilder

def _parse_expression(expr_string: str):
    """Helper function to parse an expression string and return our custom AST."""
    input_stream = InputStream(expr_string)
    lexer = CSSValueExpressionLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CSSValueExpressionParser(stream)
    tree = parser.parse()

    ast_builder = ExpressionAstBuilder()
    return ast_builder.visit(tree)

def _parse_properties_string(props_str: str):
    """Parses a string of CSS-like properties into a dictionary of name -> ExprAST."""
    properties = {}
    # A more robust regex to split properties, handling semicolons inside strings
    # This is still a simplification and might fail on edge cases, but good for now.
    for prop in re.split(r';(?=(?:[^"]*"[^"]*")*[^"]*$)', props_str):
        prop = prop.strip()
        if not prop:
            continue

        match = re.match(r'([a-zA-Z0-9_-]+)\s*:(.*)', prop)
        if match:
            name = match.group(1).strip()
            value_str = match.group(2).strip()
            if name and value_str:
                properties[name] = _parse_expression(value_str)
    return properties


def parse_style_content(css_text: str):
    """
    Separates inline properties, global rules, @Style usages, and delete statements
    from within a CHTL style block. Also extracts auto-generated class/id names.
    Property values are parsed into Expression ASTs.
    """
    # Define patterns
    comment_pattern = re.compile(r'/\*.*?\*/', re.DOTALL)
    line_comment_pattern = re.compile(r'//.*')
    delete_pattern = re.compile(r'delete\s+([^;]+);')
    style_usage_pattern = re.compile(r'@Style\s+([a-zA-Z0-9_-]+);')
    rule_pattern = re.compile(r'([^;{}]+){(.*?)}', re.DOTALL)

    # 1. Strip comments
    text = comment_pattern.sub('', css_text)
    text = line_comment_pattern.sub('', text)

    # 2. Find and extract delete statements
    deleted_properties = []
    matches = delete_pattern.findall(text)
    for prop_list_str in matches:
        props = [p.strip() for p in prop_list_str.split(',')]
        deleted_properties.extend(props)
    text = delete_pattern.sub('', text)

    # 3. Find and extract @Style usages
    style_usages = []
    matches = style_usage_pattern.findall(text)
    for name in matches:
        style_usages.append(StyleUsageNode(name=name))
    text = style_usage_pattern.sub('', text)

    # 4. Find and extract all rules, then categorize them
    global_rules = {} # Maps selector -> {prop_name: ExprAST}
    auto_classes = []
    auto_ids = []

    simple_class_selector = re.compile(r'^\.([a-zA-Z0-9_-]+)$')
    simple_id_selector = re.compile(r'^\#([a-zA-Z0-9_-]+)$')

    def rule_replacer(match):
        selector = match.group(1).strip()
        properties_str = match.group(2).strip()

        if selector and properties_str:
            # Parse the properties inside the rule
            properties_ast = _parse_properties_string(properties_str)
            global_rules[selector] = properties_ast

            # Check for auto-add candidates
            class_match = simple_class_selector.match(selector)
            if class_match:
                auto_classes.append(class_match.group(1))

            id_match = simple_id_selector.match(selector)
            if id_match:
                auto_ids.append(id_match.group(1))

        return ""

    text = rule_pattern.sub(rule_replacer, text)

    # 5. The remaining text is inline properties
    inline_properties_ast = _parse_properties_string(text)

    return {
        'inline': inline_properties_ast,
        'global': global_rules,
        'usages': style_usages,
        'deleted': deleted_properties,
        'auto_classes': auto_classes,
        'auto_ids': auto_ids
    }

import os
from lark import Lark, Transformer, Tree
from . import ast

class AstTransformer(Transformer):
    # --- Terminals ---
    def NAME(self, name):
        return name.value

    def ESCAPED_STRING(self, s):
        return s[1:-1].replace('\\"', '"').replace("\\'", "'")

    def NUMBER(self, n):
        if '.' in n:
            return float(n)
        return int(n)

    # --- Rules ---
    def file(self, items):
        return ast.Document(items)

    def element_content(self, items):
        return items

    def element(self, items):
        tag = items[0]
        content = items[1] # This is now the list from element_content

        attributes = []
        styles = []
        children = []
        for item in content:
            if isinstance(item, ast.AttributeNode):
                attributes.append(item)
            elif isinstance(item, ast.TextNode):
                children.append(item)
            elif isinstance(item, ast.StyleNode):
                styles.append(item)
            elif isinstance(item, list): # From style_block
                styles.extend(item)
            elif isinstance(item, ast.ElementNode):
                children.append(item)
        return ast.ElementNode(tag, attributes, styles, children)

    def attribute(self, items):
        name = items[0]
        value = items[1]
        if name == "text":
            return ast.TextNode(value)
        return ast.AttributeNode(name, value)

    def text_block(self, items):
        return ast.TextNode(items[0])

    def style_block(self, items):
        return items

    def style_rule(self, items):
        prop = items[0]
        value = items[1]
        return ast.StyleNode(prop, value)

    def literal(self, items):
        return items[0]

    # --- CSS Expressions ---
    def css_expression(self, items):
        # Unwraps the expression if it's not a binary op
        return items[0]

    def add(self, items): return ast.BinaryOpExpression(items[0], '+', items[1])
    def sub(self, items): return ast.BinaryOpExpression(items[0], '-', items[1])
    def mul(self, items): return ast.BinaryOpExpression(items[0], '*', items[1])
    def div(self, items): return ast.BinaryOpExpression(items[0], '/', items[1])
    def mod(self, items): return ast.BinaryOpExpression(items[0], '%', items[1])
    def pow(self, items): return ast.BinaryOpExpression(items[0], '**', items[1])

    def css_term(self, items):
        if len(items) == 2:
            return ast.Term(str(items[0]), items[1])
        value = items[0]
        if isinstance(value, (int, float)):
            return ast.Term(str(value))
        return ast.Term(value)

# --- Main Parser Function ---
grammar_path = os.path.join(os.path.dirname(__file__), 'chtl.lark')
with open(grammar_path) as f:
    grammar = f.read()

chtl_parser = Lark(grammar, start='file', parser='lalr', transformer=AstTransformer())

def parse_chtl(source_code: str) -> ast.Document:
    return chtl_parser.parse(source_code)

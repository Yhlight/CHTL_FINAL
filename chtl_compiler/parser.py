import os
from lark import Lark, Transformer, v_args
from . import ast

@v_args(inline=True)
class AstTransformer(Transformer):
    # --- Terminals ---
    def NAME(self, n): return n.value
    def ESCAPED_STRING(self, s): return s[1:-1]
    def NUMBER(self, n): return n.value
    def TEXT(self, t): return t.value
    def STYLE(self, t): return t.value

    # --- Rules ---
    def file(self, *children): return ast.Document(list(children))
    def literal(self, value): return value
    def element_content(self, *children): return list(children)

    def element(self, tag, content):
        attributes, styles, children = [], [], []
        for item in content:
            if isinstance(item, ast.AttributeNode): attributes.append(item)
            elif isinstance(item, ast.TextNode): children.append(item)
            elif isinstance(item, ast.ElementNode): children.append(item)
            elif isinstance(item, list): # From style_block
                styles.extend(item)
        return ast.ElementNode(tag, attributes, styles, children)

    def attribute(self, name, value):
        if name == "text": return ast.TextNode(value)
        return ast.AttributeNode(name, value)

    def text_block(self, _, value): return ast.TextNode(value)

    def style_block(self, _, *rules): return list(rules)

    def style_rule(self, prop, value): return ast.StyleNode(prop, value)

# --- Main Parser Function ---
grammar_path = os.path.join(os.path.dirname(__file__), 'chtl.lark')
with open(grammar_path) as f:
    grammar = f.read()

chtl_parser = Lark(grammar, start='file', parser='lalr', transformer=AstTransformer())

def parse_chtl(source_code: str) -> ast.Document:
    return chtl_parser.parse(source_code)

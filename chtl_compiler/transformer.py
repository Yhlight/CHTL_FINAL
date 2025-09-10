from lark import Transformer, v_args, Token
from .ast_nodes import ProgramNode, ElementNode, AttributeNode, TextNode, StyleBlockNode, StylePropertyNode

@v_args(inline=True)
class ChtlTransformer(Transformer):
    def __init__(self):
        super().__init__()

    def start(self, *statements):
        return ProgramNode(children=[s for s in statements if s])

    def element(self, identifier, *content):
        tag_name = identifier.value

        children = [item for item in content if isinstance(item, (ElementNode, TextNode))]
        attributes = [item for item in content if isinstance(item, AttributeNode)]
        style_block = next((item for item in content if isinstance(item, StyleBlockNode)), None)

        # Find if there is a 'text' attribute and convert it to a TextNode child
        text_attr = next((attr for attr in attributes if attr.name == 'text'), None)
        if text_attr:
            attributes.remove(text_attr)
            children.append(TextNode(content=text_attr.value))

        return ElementNode(
            tag_name=tag_name,
            children=children,
            attributes=attributes,
            style_block=style_block
        )

    def text_element(self, text_keyword, value):
        return TextNode(content=self._clean_value(value))

    def style_block(self, style_keyword, *properties):
        return StyleBlockNode(properties=list(properties))

    def style_property(self, key, value):
        # The value from the style_value rule is a Token
        return StylePropertyNode(name=key.value, value=value.value.strip())

    def attribute(self, identifier, value):
        key = identifier.value
        return AttributeNode(name=key, value=self._clean_value(value))

    def value(self, value_token):
        return value_token

    def _clean_value(self, value_token):
        if isinstance(value_token, Token):
            if value_token.type == 'QUOTED_STRING':
                return value_token.value[1:-1]
            return value_token.value
        return str(value_token)

    # Terminals
    def STYLE_KEYWORD(self, token): return token
    def TEXT(self, token): return token
    def IDENTIFIER(self, token): return token
    def UNQUOTED_VALUE(self, token): return token
    def QUOTED_STRING(self, token): return token
    def style_value(self, token): return token

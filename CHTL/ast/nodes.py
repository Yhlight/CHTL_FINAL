from typing import List, Union

class AstNode:
    """Base class for all AST nodes."""
    def accept(self, visitor):
        """Accept a visitor."""
        method_name = f'visit_{self.__class__.__name__.lower()}'
        visitor_method = getattr(visitor, method_name, visitor.visit_default)
        return visitor_method(self)

class DocumentNode(AstNode):
    """Represents the root of a CHTL document."""
    def __init__(self, children: List['AstNode']):
        self.children = children
        self.global_rules: List[str] = []

    def __repr__(self):
        return f"DocumentNode(children={self.children}, global_rules={len(self.global_rules)})"

class ElementNode(AstNode):
    """Represents an HTML element."""
    def __init__(self, tag_name: str, attributes: List['AttributeNode'], children: List['AstNode']):
        self.tag_name = tag_name
        self.attributes = attributes
        self.children = children

    def __repr__(self):
        return f"ElementNode(tag='{self.tag_name}', attributes={self.attributes}, children={self.children})"

class AttributeNode(AstNode):
    """Represents an attribute of an HTML element."""
    def __init__(self, name: str, value: str):
        self.name = name
        self.value = value

    def __repr__(self):
        return f"AttributeNode(name='{self.name}', value='{self.value}')"

class TextNode(AstNode):
    """Represents a text node."""
    def __init__(self, value: str):
        self.value = value

    def __repr__(self):
        return f"TextNode(value='{self.value}')"

class StyleNode(AstNode):
    """Represents a style block."""
    def __init__(self, raw_content: str, inline_styles: str, global_rules: List[str]):
        self.raw_content = raw_content
        self.inline_styles = inline_styles
        self.global_rules = global_rules

    def __repr__(self):
        return f"StyleNode(inline='{self.inline_styles}', global_rules={len(self.global_rules)})"

# A type hint for any node that can be a child of an element
ElementChild = Union[ElementNode, TextNode, StyleNode]

from dataclasses import dataclass, field
from typing import List, Union

@dataclass
class BaseNode:
    """Base class for all AST nodes."""
    pass

@dataclass
class AttributeNode(BaseNode):
    """Represents an HTML attribute."""
    name: str
    value: str
    lineno: int = 0

@dataclass
class TextNode(BaseNode):
    """Represents a text node."""
    value: str
    lineno: int = 0

@dataclass
class CommentNode(BaseNode):
    """Represents a comment node."""
    value: str
    lineno: int = 0

@dataclass
class CssPropertyNode(BaseNode):
    """Represents a single CSS property (e.g., 'width: 100px')."""
    name: str
    value: str
    lineno: int = 0

@dataclass
class CssRuleNode(BaseNode):
    """Represents a CSS rule block (e.g., '.box { ... }')."""
    selector: str
    properties: List[CssPropertyNode] = field(default_factory=list)
    lineno: int = 0

# StyleNode can contain either direct properties or full rules.
StyleContent = Union[CssPropertyNode, CssRuleNode]

@dataclass
class StyleNode(BaseNode):
    """Represents a style {} block."""
    children: List[StyleContent] = field(default_factory=list)
    lineno: int = 0

# Update the general Node type alias to include StyleNode.
# An ElementNode can now have a StyleNode as a child.
Node = Union['ElementNode', TextNode, CommentNode, StyleNode]

@dataclass
class ElementNode(BaseNode):
    """Represents an HTML element."""
    tag_name: str
    attributes: List[AttributeNode] = field(default_factory=list)
    children: List[Node] = field(default_factory=list)
    lineno: int = 0

@dataclass
class DocumentNode(BaseNode):
    """Represents the root of the document, containing a list of top-level nodes."""
    children: List[Node] = field(default_factory=list)
    lineno: int = 0

if __name__ == '__main__':
    # Example of how to build a simple AST manually with style nodes

    doc = DocumentNode(children=[
        ElementNode(
            lineno=1,
            tag_name='div',
            children=[
                StyleNode(lineno=2, children=[
                    # Inline style property
                    CssPropertyNode(name='color', value='red', lineno=3),
                    # Hoisted CSS rule
                    CssRuleNode(selector='.box', lineno=4, properties=[
                        CssPropertyNode(name='font-size', value='16px', lineno=5)
                    ])
                ])
            ]
        )
    ])

    import json
    class AstEncoder(json.JSONEncoder):
        def default(self, o):
            if isinstance(o, (BaseNode, DocumentNode, ElementNode, TextNode, AttributeNode, CommentNode, StyleNode, CssRuleNode, CssPropertyNode)):
                return vars(o)
            return super().default(o)

    print(json.dumps(doc, cls=AstEncoder, indent=2))

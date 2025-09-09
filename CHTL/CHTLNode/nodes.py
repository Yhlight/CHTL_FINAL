from dataclasses import dataclass, field
from typing import List, Union

@dataclass
class BaseNode:
    """Base class for all AST nodes.
    This is now more of a marker class, as lineno is defined in each subclass
    to avoid issues with dataclass inheritance and default argument order.
    """
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
    """Represents a comment node.

    The `value` will contain the full comment string, e.g., '// my comment'
    """
    value: str
    lineno: int = 0

# Type alias for children
Node = Union['ElementNode', TextNode, CommentNode]

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
    # Example of how to build a simple AST manually

    # Represents:
    # <div id="container">
    #   <!-- A comment -->
    #   Hello World
    # </div>

    doc = DocumentNode(children=[
        ElementNode(
            lineno=1,
            tag_name='div',
            attributes=[
                AttributeNode(name='id', value='container', lineno=1)
            ],
            children=[
                CommentNode(value='-- A comment', lineno=2),
                TextNode(value='Hello World', lineno=3)
            ]
        )
    ])

    import json
    # A simple way to visualize the AST
    class AstEncoder(json.JSONEncoder):
        def default(self, o):
            if isinstance(o, (BaseNode, DocumentNode, ElementNode, TextNode, AttributeNode, CommentNode)):
                # Use vars() for dataclasses to get a dict of their attributes
                return vars(o)
            return super().default(o)

    print(json.dumps(doc, cls=AstEncoder, indent=2))

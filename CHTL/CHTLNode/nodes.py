from dataclasses import dataclass, field
from typing import List, Union, Any

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

@dataclass
class TemplateUsageNode(BaseNode):
    """Represents the usage of a template (e.g., @Element Box;)."""
    template_type: str
    name: str
    lineno: int = 0

StyleContent = Union[CssPropertyNode, CssRuleNode, TemplateUsageNode]

@dataclass
class StyleNode(BaseNode):
    """Represents a style {} block."""
    children: List[StyleContent] = field(default_factory=list)
    lineno: int = 0

Node = Union['ElementNode', TextNode, CommentNode, StyleNode, TemplateUsageNode]

@dataclass
class ElementNode(BaseNode):
    """Represents an HTML element."""
    tag_name: str
    attributes: List[AttributeNode] = field(default_factory=list)
    children: List[Node] = field(default_factory=list)
    lineno: int = 0

@dataclass
class TemplateDefinitionNode(BaseNode):
    """Represents a [Template] @Type Name { ... } block."""
    template_type: str
    name: str
    content: List[Any] = field(default_factory=list)
    lineno: int = 0

DocumentContent = Union[ElementNode, CommentNode, TemplateDefinitionNode]

@dataclass
class DocumentNode(BaseNode):
    """Represents the root of the document, containing a list of top-level nodes."""
    children: List[DocumentContent] = field(default_factory=list)
    lineno: int = 0

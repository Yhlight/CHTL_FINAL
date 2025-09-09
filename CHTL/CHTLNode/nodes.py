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

# --- Nodes for Customization ---

@dataclass
class DeleteNode(BaseNode):
    """Represents a 'delete' operation in a customization block."""
    targets: List[str] = field(default_factory=list) # e.g., ['line-height', '@Style Theme']
    lineno: int = 0

@dataclass
class InsertNode(BaseNode):
    """Represents an 'insert' operation in a customization block."""
    position: str # 'after', 'before', 'replace', 'at top', 'at bottom'
    target_selector: str
    content: List[Any] = field(default_factory=list)
    lineno: int = 0

# A CustomUsageNode's body can contain specialization rules or elements with style overrides.
CustomizationContent = Union['ElementNode', DeleteNode, InsertNode]

@dataclass
class CustomUsageNode(BaseNode):
    """Represents the usage of a [Custom] template, including its specialization body."""
    template_type: str
    name: str
    body: List[CustomizationContent] = field(default_factory=list)
    lineno: int = 0

# --- End Customization Nodes ---

StyleContent = Union[CssPropertyNode, CssRuleNode, TemplateUsageNode, CustomUsageNode]

@dataclass
class StyleNode(BaseNode):
    """Represents a style {} block."""
    children: List[StyleContent] = field(default_factory=list)
    lineno: int = 0

Node = Union['ElementNode', TextNode, CommentNode, StyleNode, TemplateUsageNode, CustomUsageNode]

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

# A CustomDefinitionNode is structurally identical to a TemplateDefinitionNode
CustomDefinitionNode = TemplateDefinitionNode

DocumentContent = Union[ElementNode, CommentNode, TemplateDefinitionNode, CustomDefinitionNode]

@dataclass
class DocumentNode(BaseNode):
    """Represents the root of the document, containing a list of top-level nodes."""
    children: List[DocumentContent] = field(default_factory=list)
    lineno: int = 0

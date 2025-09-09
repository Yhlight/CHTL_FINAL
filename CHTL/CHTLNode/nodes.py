from dataclasses import dataclass, field
from typing import List, Union, Any, Optional

@dataclass
class BaseNode:
    """Base class for all AST nodes."""
    pass

@dataclass
class AttributeNode(BaseNode):
    name: str
    value: str
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class TextNode(BaseNode):
    value: str
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class CommentNode(BaseNode):
    value: str
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class CssPropertyNode(BaseNode):
    """Value is a simple string for now. Will be an expression tree later."""
    name: str
    value: str
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class CssRuleNode(BaseNode):
    selector: str
    properties: List[CssPropertyNode] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class TemplateUsageNode(BaseNode):
    template_type: str
    name: str
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class DeleteNode(BaseNode):
    targets: List[str] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class InsertNode(BaseNode):
    position: str
    target_selector: str
    content: List[Any] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

CustomizationContent = Union['ElementNode', DeleteNode, InsertNode]

@dataclass
class CustomUsageNode(BaseNode):
    template_type: str
    name: str
    body: List[CustomizationContent] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

StyleContent = Union[CssPropertyNode, CssRuleNode, TemplateUsageNode, CustomUsageNode]

@dataclass
class StyleNode(BaseNode):
    children: List[StyleContent] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)
    def add_child(self, child: StyleContent):
        child.parent = self
        self.children.append(child)

Node = Union['ElementNode', TextNode, CommentNode, StyleNode, TemplateUsageNode, CustomUsageNode]

@dataclass
class ElementNode(BaseNode):
    tag_name: str
    attributes: List[AttributeNode] = field(default_factory=list)
    children: List[Node] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)
    def add_child(self, child: Node):
        child.parent = self
        self.children.append(child)

@dataclass
class TemplateDefinitionNode(BaseNode):
    template_type: str
    name: str
    content: List[Any] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

CustomDefinitionNode = TemplateDefinitionNode

DocumentContent = Union[ElementNode, CommentNode, TemplateDefinitionNode, CustomDefinitionNode]

@dataclass
class DocumentNode(BaseNode):
    children: List[DocumentContent] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)
    def add_child(self, child: DocumentContent):
        child.parent = self
        self.children.append(child)

from dataclasses import dataclass, field
from typing import List, Union

# A simple base class for type hinting and potential future common functionality
class BaseNode:
    pass

@dataclass
class AttributeNode:
    """Represents an attribute of an element (e.g., id: "main")."""
    name: str
    value: str

@dataclass
class TextNode(BaseNode):
    """Represents a text node (e.g., text { "Hello" })."""
    value: str

@dataclass
class StylePropertyNode(BaseNode):
    """Represents a single CSS property (e.g., width: 100px)."""
    name: str
    value: str

@dataclass
class StyleSelectorRuleNode(BaseNode):
    """Represents a CSS rule with a selector (e.g., .box { ... })."""
    selector: str
    properties: List[StylePropertyNode] = field(default_factory=list)

@dataclass
class StyleNode(BaseNode):
    """Represents a style { ... } block within an element."""
    children: List[Union[StylePropertyNode, StyleSelectorRuleNode]] = field(default_factory=list)

@dataclass
class ElementNode(BaseNode):
    """Represents an element node (e.g., div { ... })."""
    tag_name: str
    attributes: List[AttributeNode] = field(default_factory=list)
    children: List[BaseNode] = field(default_factory=list)

@dataclass
class ProgramNode(BaseNode):
    """The root node of the AST, containing a list of top-level nodes."""
    children: List[BaseNode] = field(default_factory=list)

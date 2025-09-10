from dataclasses import dataclass, field
from typing import List, Union

# Forward declaration for type hints
class BaseNode:
    pass

@dataclass
class AttributeNode:
    """Represents an attribute of an element (e.g., id: "main")."""
    name: str
    value: str  # For now, the value is a simple string.

@dataclass
class TextNode(BaseNode):
    """Represents a text node (e.g., text { "Hello" })."""
    value: str

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

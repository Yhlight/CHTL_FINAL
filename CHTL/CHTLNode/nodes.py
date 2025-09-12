from dataclasses import dataclass, field
from typing import List, Any

class BaseNode: pass

@dataclass
class AttributeNode:
    name: str
    value: Any # Value can be a string or later, a more complex node

@dataclass
class TextNode(BaseNode):
    value: str

@dataclass
class ElementNode(BaseNode):
    tag_name: str
    attributes: List[AttributeNode] = field(default_factory=list)
    children: List[BaseNode] = field(default_factory=list)

@dataclass
class ProgramNode(BaseNode):
    children: List[BaseNode] = field(default_factory=list)

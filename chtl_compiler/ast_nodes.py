from dataclasses import dataclass, field
from typing import List, Union, Optional

# Base class for all nodes
class Node:
    pass

@dataclass
class StylePropertyNode(Node):
    name: str
    value: str

@dataclass
class StyleBlockNode(Node):
    properties: List[StylePropertyNode] = field(default_factory=list)

@dataclass
class AttributeNode(Node):
    name: str
    value: str

@dataclass
class TextNode(Node):
    content: str

@dataclass
class ElementNode(Node):
    tag_name: str
    children: List[Union['ElementNode', TextNode]] = field(default_factory=list)
    attributes: List[AttributeNode] = field(default_factory=list)
    style_block: Optional[StyleBlockNode] = None

@dataclass
class ProgramNode(Node):
    children: List[Union[ElementNode, TextNode]] = field(default_factory=list)

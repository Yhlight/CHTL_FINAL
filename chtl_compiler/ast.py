from dataclasses import dataclass, field
from typing import List, Union

@dataclass
class Node:
    pass

@dataclass
class StyleNode(Node):
    prop: str
    value: str

@dataclass
class AttributeNode(Node):
    name: str
    value: str

@dataclass
class TextNode(Node):
    content: str

@dataclass
class ElementNode(Node):
    tag: str
    attributes: List[AttributeNode]
    styles: List[StyleNode]
    children: List[Union['ElementNode', TextNode]]

@dataclass
class Document(Node):
    children: List[ElementNode] = field(default_factory=list)

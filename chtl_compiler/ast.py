from dataclasses import dataclass
from typing import List, Union

# --- Base Classes ---

class Node:
    """Base class for all AST nodes."""
    pass

class Expression(Node):
    """Base class for all expression nodes."""
    pass

# --- AST Node Classes ---

@dataclass
class Term(Expression):
    value: str
    unit: str = None

@dataclass
class BinaryOpExpression(Expression):
    left: Expression
    operator: str
    right: Expression

@dataclass
class StyleNode(Node):
    prop: str
    value: Expression

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
    children: List[ElementNode]

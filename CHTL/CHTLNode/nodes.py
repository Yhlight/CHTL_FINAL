from dataclasses import dataclass, field
from typing import List, Union

class BaseNode: pass
class ExpressionNode(BaseNode): pass

@dataclass
class AttributeNode:
    name: str
    value: str

@dataclass
class TextNode(BaseNode):
    value: str

@dataclass
class ValueWithUnitNode(ExpressionNode):
    value: Union[int, float]
    unit: str

@dataclass
class LiteralNode(ExpressionNode):
    value: Union[int, float, str]

@dataclass
class ReferenceNode(ExpressionNode):
    property_name: str
    selector_tokens: List['Token'] = field(default_factory=list) # e.g., [HASH, IDENTIFIER] for #box

@dataclass
class BinaryOpNode(ExpressionNode):
    left: ExpressionNode
    op: 'Token'
    right: ExpressionNode

@dataclass
class ConditionalNode(ExpressionNode):
    condition: ExpressionNode
    true_branch: ExpressionNode
    false_branch: ExpressionNode

@dataclass
class StylePropertyNode(BaseNode):
    name: str
    value_expression: ExpressionNode

@dataclass
class StyleSelectorRuleNode(BaseNode):
    selector_tokens: List['Token'] = field(default_factory=list)
    properties: List[StylePropertyNode] = field(default_factory=list)

@dataclass
class StyleNode(BaseNode):
    children: List[Union[StylePropertyNode, StyleSelectorRuleNode]] = field(default_factory=list)

@dataclass
class ElementNode(BaseNode):
    tag_name: str
    attributes: List[AttributeNode] = field(default_factory=list)
    children: List[BaseNode] = field(default_factory=list)

@dataclass
class ProgramNode(BaseNode):
    children: List[BaseNode] = field(default_factory=list)

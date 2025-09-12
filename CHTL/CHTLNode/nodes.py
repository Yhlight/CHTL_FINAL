from typing import List, Union

class BaseNode:
    def __repr__(self):
        attributes = ', '.join(f"{key}={repr(value)}" for key, value in self.__dict__.items())
        return f"{self.__class__.__name__}({attributes})"

# Expression Nodes
class ExprNode(BaseNode): pass
class BinaryOpNode(ExprNode):
    def __init__(self, left: 'ExprNode', op: 'Token', right: 'ExprNode'):
        self.left = left
        self.op = op
        self.right = right
class DimensionNode(ExprNode):
    def __init__(self, value: str, unit: str):
        self.value = value
        self.unit = unit
class IdentifierNode(ExprNode):
    def __init__(self, name: str):
        self.name = name
class ConditionalNode(ExprNode):
    def __init__(self, condition: 'ExprNode', true_branch: 'ExprNode', false_branch: 'ExprNode'):
        self.condition = condition
        self.true_branch = true_branch
        self.false_branch = false_branch
class AttributeReferenceNode(ExprNode):
    def __init__(self, selector: str, property_name: str):
        self.selector = selector
        self.property_name = property_name

# Core Nodes
class AttributeNode(BaseNode):
    def __init__(self, name: str, value: str):
        self.name = name
        self.value = value
class TextNode(BaseNode):
    def __init__(self, content: str):
        self.content = content
class CommentNode(BaseNode):
    def __init__(self, content: str):
        self.content = content
class StylePropertyNode(BaseNode):
    def __init__(self, property_name: str, value: 'ExprNode'):
        self.property_name = property_name
        self.value = value
class SelectorBlockNode(BaseNode):
    def __init__(self, selector: str, properties: List['StylePropertyNode']):
        self.selector = selector
        self.properties = properties
class StyleNode(BaseNode):
    def __init__(self, rules: List[Union['StylePropertyNode', 'SelectorBlockNode']]):
        self.rules = rules
class ElementNode(BaseNode):
    def __init__(self, tag_name: str, attributes: List[AttributeNode], children: List['Node']):
        self.tag_name = tag_name
        self.attributes = attributes
        self.children = children
class RootNode(BaseNode):
    def __init__(self, children: List['Node']):
        self.children = children

Node = Union[ElementNode, TextNode, CommentNode, StyleNode, SelectorBlockNode]

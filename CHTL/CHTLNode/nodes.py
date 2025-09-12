from typing import List, Union

class BaseNode:
    def __repr__(self):
        attributes = ', '.join(f"{key}={repr(value)}" for key, value in self.__dict__.items())
        return f"{self.__class__.__name__}({attributes})"

# Expression Nodes
class ExprNode(BaseNode): pass
class BinaryOpNode(ExprNode):
    def __init__(self, left: 'ExprNode', op: 'Token', right: 'ExprNode'):
        self.left, self.op, self.right = left, op, right
class DimensionNode(ExprNode):
    def __init__(self, value: str, unit: str):
        self.value, self.unit = value, unit
class IdentifierNode(ExprNode):
    def __init__(self, name: str): self.name = name
class ConditionalNode(ExprNode):
    def __init__(self, condition: 'ExprNode', true_branch: 'ExprNode', false_branch: 'ExprNode'):
        self.condition, self.true_branch, self.false_branch = condition, true_branch, false_branch
class AttributeReferenceNode(ExprNode):
    def __init__(self, selector: str, property_name: str):
        self.selector, self.property_name = selector, property_name
class VarUsageNode(ExprNode):
    def __init__(self, group_name: str, var_name: str):
        self.group_name, self.var_name = group_name, var_name

# Template & Custom Nodes
class VarDefinitionNode(BaseNode):
    def __init__(self, name: str, value: 'ExprNode'):
        self.name, self.value = name, value
class TemplateUsageNode(BaseNode):
    def __init__(self, template_type: str, name: str):
        self.template_type, self.name = template_type, name
class TemplateDefinitionNode(BaseNode):
    def __init__(self, template_type: str, name: str, body: list):
        self.template_type, self.name, self.body = template_type, name, body
class CustomDefinitionNode(TemplateDefinitionNode): pass
class CustomUsageNode(BaseNode):
    def __init__(self, template_type: str, name: str, body: list):
        self.template_type, self.name, self.body = template_type, name, body
class DeleteNode(BaseNode):
    def __init__(self, targets: list): self.targets = targets
class InsertNode(BaseNode):
    def __init__(self, position: str, target_selector: str, body: list):
        self.position, self.target_selector, self.body = position, target_selector, body

# Core Nodes
class AttributeNode(BaseNode):
    def __init__(self, name: str, value: str):
        self.name, self.value = name, value
class TextNode(BaseNode):
    def __init__(self, content: str): self.content = content
class CommentNode(BaseNode):
    def __init__(self, content: str): self.content = content
class StylePropertyNode(BaseNode):
    def __init__(self, property_name: str, value: 'ExprNode'):
        self.property_name, self.value = property_name, value
class SelectorBlockNode(BaseNode):
    def __init__(self, selector: str, properties: List['StylePropertyNode']):
        self.selector, self.properties = selector, properties
class StyleNode(BaseNode):
    def __init__(self, rules: List[Union['StylePropertyNode', 'SelectorBlockNode', 'TemplateUsageNode', 'CustomUsageNode', 'DeleteNode']]):
        self.rules = rules
class ElementNode(BaseNode):
    def __init__(self, tag_name: str, attributes: List[AttributeNode], children: List[Union['Node', 'TemplateUsageNode', 'CustomUsageNode', 'InsertNode', 'DeleteNode']]):
        self.tag_name, self.attributes, self.children = tag_name, attributes, children
class RootNode(BaseNode):
    def __init__(self, children: List[Union['Node', 'TemplateDefinitionNode', 'CustomDefinitionNode']]):
        self.children = children

Node = Union[ElementNode, TextNode, CommentNode, StyleNode, SelectorBlockNode]

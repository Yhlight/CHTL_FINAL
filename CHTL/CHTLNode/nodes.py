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

# --- Template-related Nodes ---

@dataclass
class TemplateDefinitionNode(BaseNode):
    """Represents [Template] @Style MyStyles { ... }"""
    template_type: str # e.g., "Style", "Element", "Var"
    name: str
    body: List[BaseNode] = field(default_factory=list) # Can contain properties or other elements

@dataclass
class TemplateUsageNode(BaseNode):
    """Represents @Style MyStyles; or @Style MyStyles { ... }"""
    template_type: str
    name: str
    specialization_body: List[BaseNode] = None

@dataclass
class ImportNode(BaseNode):
    """Represents [Import] @Chtl from "./file.chtl";"""
    import_type: str # e.g., Chtl, Style
    path: str
    alias: str = None

# --- Customization and Origin Nodes ---

@dataclass
class CustomDefinitionNode(BaseNode):
    """Represents [Custom] @Style MyStyles { ... }"""
    custom_type: str # e.g., "Style", "Element"
    name: str
    body: List[BaseNode] = field(default_factory=list)

@dataclass
class OriginNode(BaseNode):
    """Represents an [Origin] block with raw content."""
    origin_type: str # e.g., "Html", "Style"
    name: str = None # Optional name for the block
    content: str = ""

@dataclass
class DeleteNode(BaseNode):
    """Represents a 'delete' operation inside a [Custom] block."""
    targets: List[str] = field(default_factory=list)

@dataclass
class InsertNode(BaseNode):
    """Represents an 'insert' operation inside a [Custom] block."""
    position: str # e.g., "after", "before"
    target_selector: str
    body: List[BaseNode] = field(default_factory=list)

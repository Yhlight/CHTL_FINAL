from dataclasses import dataclass, field
from typing import List, Union, Any, Optional, Dict
from enum import Enum, auto
# Import Token for type hinting in expression nodes
from CHTL.CHTLLexer.keywords import Token

@dataclass
class BaseNode:
    """Base class for all AST nodes."""
    pass

# --- Expression and Selector Nodes ---

@dataclass
class ExpressionNode(BaseNode):
    """Base class for all nodes that can appear in a style expression."""
    pass

@dataclass
class LiteralNode(ExpressionNode):
    """Represents a literal value (e.g., '100px', 'red', 50)."""
    value: Any
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class PropertyReferenceNode(ExpressionNode):
    """Represents a reference to another property (e.g., .box.width)."""
    property_name: str
    selector: 'SelectorNode' # Changed to SelectorNode
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class BinaryOpNode(ExpressionNode):
    """Represents a binary operation (e.g., a + b, a > b)."""
    left: ExpressionNode
    op: Token
    right: ExpressionNode
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class LogicalOpNode(ExpressionNode):
    """Represents a logical operation (e.g., a && b)."""
    left: ExpressionNode
    op: Token
    right: ExpressionNode
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class ConditionalExpressionNode(ExpressionNode):
    """Represents a conditional (ternary) expression."""
    condition: ExpressionNode
    true_expr: ExpressionNode
    false_expr: Optional[ExpressionNode] = None
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class FunctionCallNode(ExpressionNode):
    """Represents a function call-like expression, used for @Var templates."""
    callee_name: str
    arguments: List[ExpressionNode] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

class SelectorType(Enum):
    TAG = auto()
    CLASS = auto()
    ID = auto()

@dataclass
class SelectorPartNode(BaseNode):
    type: SelectorType
    value: str
    index: Optional[int] = None

@dataclass
class SelectorNode(BaseNode):
    parts: List[SelectorPartNode] = field(default_factory=list)


# --- Core CHTL Nodes ---

@dataclass
class AttributeNode(BaseNode):
    name: str
    value: str
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class TextNode(BaseNode):
    value: str
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class CommentNode(BaseNode):
    value: str
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class CssPropertyNode(BaseNode):
    """A CSS property. Its value can be a simple string or a complex expression tree."""
    name: str
    value: Union[str, ExpressionNode]
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class CssRuleNode(BaseNode):
    selector: str
    properties: List[CssPropertyNode] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class TemplateUsageNode(BaseNode):
    template_type: str
    name: str
    from_namespace: Optional[str] = None
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class DeleteNode(BaseNode):
    targets: List[str] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class InsertNode(BaseNode):
    position: str
    target_selector: str
    content: List[Any] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

CustomizationContent = Union['ElementNode', DeleteNode, InsertNode]

@dataclass
class CustomUsageNode(BaseNode):
    template_type: str
    name: str
    from_namespace: Optional[str] = None
    body: List[CustomizationContent] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

StyleContent = Union[CssPropertyNode, CssRuleNode, TemplateUsageNode, CustomUsageNode]

@dataclass
class StyleNode(BaseNode):
    children: List[StyleContent] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)
    def add_child(self, child: StyleContent):
        child.parent = self
        self.children.append(child)

Node = Union['ElementNode', TextNode, CommentNode, StyleNode, TemplateUsageNode, CustomUsageNode]

@dataclass
class ElementNode(BaseNode):
    tag_name: str
    attributes: List[AttributeNode] = field(default_factory=list)
    children: List[Node] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)
    def add_child(self, child: Node):
        child.parent = self
        self.children.append(child)

@dataclass
class TemplateDefinitionNode(BaseNode):
    template_type: str
    name: str
    content: List[Any] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

CustomDefinitionNode = TemplateDefinitionNode

@dataclass
class ImportNode(BaseNode):
    """Represents an [Import] statement."""
    import_type: str  # e.g., 'Chtl', 'Style', 'JavaScript'
    construct_type: Optional[str] = None # e.g., 'Element', 'Style' for precise imports
    path: str = ""
    alias: Optional[str] = None
    imported_item: Optional[str] = None
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class ConfigNode(BaseNode):
    """Represents a [Configuration] block."""
    config: Dict[str, str] = field(default_factory=dict)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class OriginNode(BaseNode):
    """Represents a block of raw, untransformed code."""
    origin_type: str # e.g., 'Html', 'Style', 'JavaScript'
    name: Optional[str] = None
    content: str = ""
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)


@dataclass
class InfoNode(BaseNode):
    """Represents an [Info] block for a module."""
    metadata: Dict[str, str] = field(default_factory=dict)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

@dataclass
class ExportNode(BaseNode):
    """Represents an [Export] block for a module."""
    exported_items: Dict[str, List[str]] = field(default_factory=dict)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)

DocumentContent = Union[ElementNode, CommentNode, TemplateDefinitionNode, CustomDefinitionNode, StyleNode, ImportNode, ConfigNode, OriginNode, InfoNode, ExportNode]

@dataclass
class DocumentNode(BaseNode):
    children: List[DocumentContent] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['BaseNode'] = field(default=None, repr=False)
    def add_child(self, child: DocumentContent):
        child.parent = self
        self.children.append(child)

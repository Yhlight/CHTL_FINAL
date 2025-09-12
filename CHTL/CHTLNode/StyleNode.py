from .BaseNode import BaseNode
from .TemplateUsageNode import TemplateUsageNode
from typing import NamedTuple, List, Dict

class CssRule(NamedTuple):
    """A simple structure to hold a CSS rule (selector and properties)."""
    selector: str
    properties: Dict[str, List[BaseNode]]

class StyleNode(BaseNode):
    """
    Represents a style {} block in CHTL.
    It holds inline properties, global rules, and signals for auto-generation of attributes.
    """
    def __init__(self, properties: Dict[str, List[BaseNode]] = None, rules: List[CssRule] = None,
                 auto_classes: List[str] = None, auto_ids: List[str] = None,
                 template_usages: List[TemplateUsageNode] = None):
        # For inline styles, e.g., { width: 100px; }
        self.properties = properties if properties is not None else {}
        # For global rules, e.g., { .box { width: 100px; } }
        self.rules = rules if rules is not None else []
        # For auto-generated class="..."
        self.auto_classes_to_add = auto_classes if auto_classes is not None else []
        # For auto-generated id="..."
        self.auto_ids_to_add = auto_ids if auto_ids is not None else []
        # For template usages, e.g., @Style MyStyles;
        self.template_usages = template_usages if template_usages is not None else []

    def to_dict(self) -> dict:
        """Converts the style node to a dictionary for debugging."""
        def props_to_dict(props: Dict[str, List[BaseNode]]):
            # The value is now a list of nodes
            return {k: [node.to_dict() for node in v_list] for k, v_list in props.items()}

        return {
            "type": "StyleNode",
            "properties": props_to_dict(self.properties),
            "rules": [{'selector': r.selector, 'properties': props_to_dict(r.properties)} for r in self.rules],
            "auto_classes": self.auto_classes_to_add,
            "auto_ids": self.auto_ids_to_add,
            "template_usages": [usage.to_dict() for usage in self.template_usages],
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return (f"<StyleNode properties={len(self.properties)} rules={len(self.rules)} "
                f"auto_classes={len(self.auto_classes_to_add)} auto_ids={len(self.auto_ids_to_add)} "
                f"template_usages={len(self.template_usages)}>")

from .BaseNode import BaseNode
from typing import NamedTuple, List

class CssRule(NamedTuple):
    """A simple structure to hold a CSS rule (selector and properties)."""
    selector: str
    properties: dict[str, str]

class StyleNode(BaseNode):
    """
    Represents a style {} block in CHTL.
    It holds inline properties, global rules, and signals for auto-generation of attributes.
    """
    def __init__(self, properties: dict[str, str] = None, rules: List[CssRule] = None,
                 auto_classes: List[str] = None, auto_ids: List[str] = None):
        # For inline styles, e.g., { width: 100px; }
        self.properties = properties if properties is not None else {}
        # For global rules, e.g., { .box { width: 100px; } }
        self.rules = rules if rules is not None else []
        # For auto-generated class="..."
        self.auto_classes_to_add = auto_classes if auto_classes is not None else []
        # For auto-generated id="..."
        self.auto_ids_to_add = auto_ids if auto_ids is not None else []

    def to_dict(self) -> dict:
        """Converts the style node to a dictionary for debugging."""
        return {
            "type": "StyleNode",
            "properties": self.properties,
            "rules": [rule._asdict() for rule in self.rules],
            "auto_classes": self.auto_classes_to_add,
            "auto_ids": self.auto_ids_to_add,
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return (f"<StyleNode properties={len(self.properties)} rules={len(self.rules)} "
                f"auto_classes={len(self.auto_classes_to_add)} auto_ids={len(self.auto_ids_to_add)}>")

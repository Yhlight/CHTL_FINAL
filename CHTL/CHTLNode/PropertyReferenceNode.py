from .ExpressionNode import ExpressionNode
from typing import List

class PropertyReferenceNode(ExpressionNode):
    """Represents a reference to another element's property, e.g., box.width"""
    def __init__(self, selector_tokens: List, property_name: str):
        # Storing the raw tokens allows the generator to robustly interpret the selector later
        self.selector_tokens = selector_tokens
        self.property_name = property_name

    def to_dict(self) -> dict:
        return {
            "type": "PropertyReferenceNode",
            "selector_tokens": [token.literal for token in self.selector_tokens],
            "property_name": self.property_name
        }

    def __repr__(self) -> str:
        selector_str = "".join(t.literal for t in self.selector_tokens)
        return f"<PropertyReferenceNode ref='{selector_str}.{self.property_name}'>"

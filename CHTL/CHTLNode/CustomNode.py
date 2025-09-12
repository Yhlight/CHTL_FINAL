from .BaseNode import BaseNode
from typing import List, Union, Dict

class CustomNode(BaseNode):
    """
    Represents a [Custom] definition block.
    e.g., [Custom] @Style TextSet { color, font-size; }
    """
    def __init__(self, custom_type: str, custom_name: str, content: Union[list, dict]):
        self.custom_type = custom_type
        self.custom_name = custom_name
        self.content = content # For valueless style, list of strings.

    def to_dict(self) -> dict:
        """Converts the node to a dictionary for debugging."""
        content_dict = self.content # Default for simple lists or dicts
        if self.custom_type == 'Element':
            content_dict = [child.to_dict() for child in self.content]

        return {
            "type": "CustomNode",
            "custom_type": self.custom_type,
            "custom_name": self.custom_name,
            "content": content_dict,
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return f"<CustomNode type='{self.custom_type}' name='{self.custom_name}'>"

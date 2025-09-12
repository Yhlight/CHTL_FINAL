from .BaseNode import BaseNode
from typing import List, Dict, Union

class CustomUsageNode(BaseNode):
    """
    Represents the usage of a [Custom] definition, which can include specializations.
    e.g., @Style TextSet { color: red; }
    """
    def __init__(self, custom_type: str, custom_name: str, specializations: Union[list, dict]):
        self.custom_type = custom_type
        self.custom_name = custom_name
        # The specializations provided at the usage site.
        # For a style, this would be a dict of properties.
        # For an element, this would be a list of new/modified children.
        self.specializations = specializations

    def to_dict(self) -> dict:
        """Converts the node to a dictionary for debugging."""
        spec_dict = self.specializations # Default
        if self.custom_type == 'Element':
             spec_dict = [child.to_dict() for child in self.specializations]
        elif self.custom_type == 'Style':
             spec_dict = {k: [node.to_dict() for node in v_list] for k, v_list in self.specializations.items()}

        return {
            "type": "CustomUsageNode",
            "custom_type": self.custom_type,
            "custom_name": self.custom_name,
            "specializations": spec_dict,
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return f"<CustomUsageNode type='{self.custom_type}' name='{self.custom_name}'>"

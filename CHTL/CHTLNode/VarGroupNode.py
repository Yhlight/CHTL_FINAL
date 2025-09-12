from .BaseNode import BaseNode
from typing import Dict

class VarGroupNode(BaseNode):
    """
    Represents a [Template] @Var definition block.
    e.g., [Template] @Var MyVars { main-color: red; }
    """
    def __init__(self, name: str, variables: Dict[str, str]):
        self.name = name
        self.variables = variables

    def to_dict(self) -> dict:
        """Converts the node to a dictionary for debugging."""
        return {
            "type": "VarGroupNode",
            "name": self.name,
            "variables": self.variables,
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return f"<VarGroupNode name='{self.name}' vars={len(self.variables)}>"

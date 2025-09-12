from .BaseNode import BaseNode
from typing import Dict, List

class TemplateNode(BaseNode):
    """
    Represents a [Template] definition block.
    e.g., [Template] @Style MyStyles { ... }
    """
    def __init__(self, template_type: str, template_name: str, content: Dict[str, List[BaseNode]]):
        # The type of template, e.g., "Style", "Element", "Var"
        self.template_type = template_type
        # The name given to the template
        self.template_name = template_name
        # The content of the template. For a Style template, this is a dictionary of properties.
        self.content = content

    def to_dict(self) -> dict:
        """Converts the node to a dictionary for debugging."""
        return {
            "type": "TemplateNode",
            "template_type": self.template_type,
            "template_name": self.template_name,
            # Content serialization will depend on the template type
            "content": {k: [node.to_dict() for node in v_list] for k, v_list in self.content.items()}
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return f"<TemplateNode type='{self.template_type}' name='{self.template_name}'>"

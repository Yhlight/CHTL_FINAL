from .BaseNode import BaseNode

class TemplateUsageNode(BaseNode):
    """
    Represents the usage of a template within another block.
    e.g., @Style MyStyles;
    """
    def __init__(self, template_type: str, template_name: str):
        # The type of template being used, e.g., "Style"
        self.template_type = template_type
        # The name of the template being used
        self.template_name = template_name

    def to_dict(self) -> dict:
        """Converts the node to a dictionary for debugging."""
        return {
            "type": "TemplateUsageNode",
            "template_type": self.template_type,
            "template_name": self.template_name,
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return f"<TemplateUsageNode type='{self.template_type}' name='{self.template_name}'>"

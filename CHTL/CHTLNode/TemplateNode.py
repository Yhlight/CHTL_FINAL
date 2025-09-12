from .BaseNode import BaseNode
from .TemplateUsageNode import TemplateUsageNode
from typing import Dict, List, Union

class TemplateNode(BaseNode):
    """
    Represents a [Template] definition block.
    e.g., [Template] @Style MyStyles { ... }
    """
    def __init__(self, template_type: str, template_name: str, content: Union[dict, list], usages: List[TemplateUsageNode] = None):
        # The type of template, e.g., "Style", "Element", "Var"
        self.template_type = template_type
        # The name given to the template
        self.template_name = template_name
        # The content of the template (properties, child nodes, or variables)
        self.content = content
        # A list of other templates this template inherits from
        self.template_usages = usages if usages is not None else []

    def to_dict(self) -> dict:
        """Converts the node to a dictionary for debugging."""
        content_dict = {}
        if self.template_type == 'Style':
            # For styles, content is a dict of properties
            content_dict = {k: [node.to_dict() for node in v_list] for k, v_list in self.content.items()}
        elif self.template_type == 'Element':
            # For elements, content is a list of child nodes
            content_dict = [child.to_dict() for child in self.content]
        elif self.template_type == 'Var':
            # For vars, content is a dict of strings
            content_dict = self.content

        return {
            "type": "TemplateNode",
            "template_type": self.template_type,
            "template_name": self.template_name,
            "content": content_dict,
            "template_usages": [usage.to_dict() for usage in self.template_usages],
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return f"<TemplateNode type='{self.template_type}' name='{self.template_name}' usages={len(self.template_usages)}>"

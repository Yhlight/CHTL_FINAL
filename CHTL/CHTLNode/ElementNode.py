from .BaseNode import BaseNode

class ElementNode(BaseNode):
    """
    Represents an element in CHTL, which corresponds to an HTML tag.
    e.g., div { ... }
    """
    def __init__(self, tag_name: str, children: list[BaseNode] = None, attributes: dict[str, str] = None):
        self.tag_name = tag_name
        self.children = children if children is not None else []
        self.attributes = attributes if attributes is not None else {}

    def to_dict(self) -> dict:
        """Converts the element node to a dictionary."""
        return {
            "type": "ElementNode",
            "tag_name": self.tag_name,
            "attributes": self.attributes,
            "children": [child.to_dict() for child in self.children]
        }

    def __repr__(self) -> str:
        return f"<ElementNode tag='{self.tag_name}' attributes={len(self.attributes)} children={len(self.children)}>"

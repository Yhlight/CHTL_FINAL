from .BaseNode import BaseNode

class StyleNode(BaseNode):
    """
    Represents a style {} block in CHTL.
    It holds CSS properties that will be applied to its parent element.
    For this phase, it only handles inline styles.
    """
    def __init__(self, properties: dict[str, str] = None):
        self.properties = properties if properties is not None else {}

    def to_dict(self) -> dict:
        """Converts the style node to a dictionary for debugging."""
        return {
            "type": "StyleNode",
            "properties": self.properties
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return f"<StyleNode properties={len(self.properties)}>"

from .BaseNode import BaseNode

class TextNode(BaseNode):
    """
    Represents a text node in CHTL.
    e.g., text { "some value" }
    """
    def __init__(self, value: str):
        self.value = value

    def to_dict(self) -> dict:
        """Converts the text node to a dictionary."""
        return {
            "type": "TextNode",
            "value": self.value
        }

    def __repr__(self) -> str:
        # Truncate long text for cleaner representation
        display_value = (self.value[:15] + '...') if len(self.value) > 15 else self.value
        return f"<TextNode value='{display_value}'>"

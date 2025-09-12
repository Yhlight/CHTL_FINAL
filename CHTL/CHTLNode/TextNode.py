from .ExpressionNode import ExpressionNode

class TextNode(ExpressionNode):
    """
    Represents a text node in CHTL.
    It can also represent a literal value within a CSS expression (e.g., 'solid').
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

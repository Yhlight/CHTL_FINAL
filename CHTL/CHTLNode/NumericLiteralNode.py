from .ExpressionNode import ExpressionNode
from typing import Union

class NumericLiteralNode(ExpressionNode):
    """Represents a numeric literal with an optional unit, e.g., 100px or 50."""
    def __init__(self, value: float, unit: Union[str, None] = None):
        self.value = value
        self.unit = unit

    def to_dict(self) -> dict:
        return {"type": "NumericLiteralNode", "value": self.value, "unit": self.unit}

    def __repr__(self) -> str:
        return f"<NumericLiteralNode value={self.value} unit='{self.unit}'>"

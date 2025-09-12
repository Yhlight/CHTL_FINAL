from .ExpressionNode import ExpressionNode
from typing import List

class FunctionCallNode(ExpressionNode):
    """
    Represents a function-like call in an expression.
    e.g., ThemeColor(tableColor)
    """
    def __init__(self, function: ExpressionNode, arguments: List[ExpressionNode]):
        # The function being called (e.g., an identifier like 'ThemeColor')
        self.function = function
        # The list of arguments passed to the call
        self.arguments = arguments

    def to_dict(self) -> dict:
        """Converts the node to a dictionary for debugging."""
        return {
            "type": "FunctionCallNode",
            "function": self.function.to_dict(),
            "arguments": [arg.to_dict() for arg in self.arguments],
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return f"<FunctionCallNode function={self.function} args={len(self.arguments)}>"

from .ExpressionNode import ExpressionNode

class InfixExpressionNode(ExpressionNode):
    """Represents an infix operation, e.g., left + right."""
    def __init__(self, left: ExpressionNode, operator: str, right: ExpressionNode):
        self.left = left
        self.operator = operator
        self.right = right

    def to_dict(self) -> dict:
        return {
            "type": "InfixExpressionNode",
            "operator": self.operator,
            "left": self.left.to_dict(),
            "right": self.right.to_dict()
        }

    def __repr__(self) -> str:
        return f"<InfixExpressionNode op='{self.operator}'>"

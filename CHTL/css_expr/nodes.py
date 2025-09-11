from typing import Any, List

class ExprNode:
    """Base class for expression AST nodes."""
    def __repr__(self):
        # A generic representation to make debugging easier
        attrs = {k: v for k, v in self.__dict__.items() if not k.startswith('_')}
        return f"{self.__class__.__name__}({', '.join(f'{k}={v!r}' for k, v in attrs.items())})"

class LiteralNode(ExprNode):
    def __init__(self, value: Any):
        self.value = value

class PropertyRefNode(ExprNode):
    def __init__(self, path: List[str]):
        self.path = path

class UnaryOpNode(ExprNode):
    def __init__(self, operator: str, operand: ExprNode):
        self.operator = operator
        self.operand = operand

class BinaryOpNode(ExprNode):
    def __init__(self, left: ExprNode, operator: str, right: ExprNode):
        self.left = left
        self.operator = operator
        self.right = right

class TernaryOpNode(ExprNode):
    def __init__(self, condition: ExprNode, true_expr: ExprNode, false_expr: ExprNode):
        self.condition = condition
        self.true_expr = true_expr
        self.false_expr = false_expr

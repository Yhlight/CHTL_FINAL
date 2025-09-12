from .BaseNode import BaseNode

class ExpressionNode(BaseNode):
    """
    A base class for all nodes that are part of a mathematical expression.
    This helps in type checking within the parser and generator.
    """
    def to_dict(self) -> dict:
        raise NotImplementedError("Each expression node must implement the to_dict method.")

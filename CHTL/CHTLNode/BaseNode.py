class BaseNode:
    """
    A base class for all nodes in the Abstract Syntax Tree (AST).
    It provides a common interface for nodes.
    """
    def to_dict(self) -> dict:
        """
        Converts the node and its children to a dictionary representation.
        Useful for debugging and visualization of the AST.
        """
        raise NotImplementedError("Each node must implement the to_dict method.")

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        return f"<{self.__class__.__name__}>"

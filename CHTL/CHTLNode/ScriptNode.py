from .BaseNode import BaseNode
from typing import Dict

class ScriptNode(BaseNode):
    """
    Represents a script {} block in CHTL.
    It can hold either a 'src' attribute for external scripts
    or inline JavaScript content.
    """
    def __init__(self, attributes: Dict[str, str] = None, content: str = ""):
        # Attributes like src, async, defer
        self.attributes = attributes if attributes is not None else {}
        # Inline script content
        self.content = content

    def to_dict(self) -> dict:
        """Converts the script node to a dictionary for debugging."""
        return {
            "type": "ScriptNode",
            "attributes": self.attributes,
            "content": self.content,
        }

    def __repr__(self) -> str:
        """Provides a simple string representation of the node."""
        if self.attributes.get("src"):
            return f'<ScriptNode src="{self.attributes["src"]}">'
        return f"<ScriptNode content_len={len(self.content)}>"

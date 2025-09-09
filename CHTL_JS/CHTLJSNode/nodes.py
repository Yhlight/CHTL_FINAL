from dataclasses import dataclass, field
from typing import List, Any, Optional

@dataclass
class CHTLJSBaseNode:
    """Base class for all CHTL JS AST nodes."""
    pass

@dataclass
class CHTLJSDocumentNode(CHTLJSBaseNode):
    """Represents the root of a CHTL JS AST."""
    children: List[Any] = field(default_factory=list)
    lineno: int = 0
    parent: Optional['CHTLJSBaseNode'] = field(default=None, repr=False)

    def add_child(self, child: Any):
        child.parent = self
        self.children.append(child)

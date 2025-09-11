from dataclasses import dataclass, field
from typing import List

class CHTLJS_BaseNode:
    pass

@dataclass
class EnhancedSelectorNode(CHTLJS_BaseNode):
    """Represents an enhanced selector like {{button}}."""
    selector_text: str

@dataclass
class EventListenerNode(CHTLJS_BaseNode):
    """Represents a single event listener like click: () => { ... }."""
    event_name: str
    callback_code: str # The raw JS code for the callback

@dataclass
class ListenBlockNode(CHTLJS_BaseNode):
    """Represents a listen { ... } block."""
    target: EnhancedSelectorNode
    listeners: List[EventListenerNode] = field(default_factory=list)

@dataclass
class CHTLJS_ProgramNode(CHTLJS_BaseNode):
    """The root node for a CHTL JS fragment."""
    children: List[CHTLJS_BaseNode] = field(default_factory=list)

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
class PropertyNode(CHTLJS_BaseNode):
    """Represents a simple key: value property."""
    key: str
    value: str # The value is stored as a raw string (which could be a placeholder)

@dataclass
class KeyframeNode(CHTLJS_BaseNode):
    """Represents a single keyframe in an animation, e.g., { at: 0.5, opacity: 0 }."""
    properties: List[PropertyNode] = field(default_factory=list)

@dataclass
class AnimateNode(CHTLJS_BaseNode):
    """Represents a structured animate { ... } block."""
    target: str = None
    duration: str = None
    easing: str = None
    begin: List[PropertyNode] = field(default_factory=list)
    when: List[KeyframeNode] = field(default_factory=list)
    end: List[PropertyNode] = field(default_factory=list)
    loop: str = None
    direction: str = None
    delay: str = None
    callback: str = None # This will hold the placeholder

@dataclass
class CHTLJS_ProgramNode(CHTLJS_BaseNode):
    """The root node for a CHTL JS fragment."""
    children: List[CHTLJS_BaseNode] = field(default_factory=list)

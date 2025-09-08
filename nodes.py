class Node:
    pass

class CommentNode(Node):
    def __init__(self, value):
        self.value = value.lstrip('- ')
    def __repr__(self):
        return f"CommentNode('{self.value}')"

class ElementNode(Node):
    def __init__(self, tag_name, children=None, attributes=None):
        self.tag_name = tag_name
        self.children = children or []
        self.attributes = attributes or {} # Added attribute storage
    def __repr__(self):
        return f"ElementNode(tag='{self.tag_name}', attrs={self.attributes}, children={self.children})"

class TextNode(Node):
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return f"TextNode('{self.value}')"

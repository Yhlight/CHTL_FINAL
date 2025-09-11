from typing import List

class ChtlJsAstNode:
    """Base class for all CHTL JS AST nodes."""
    pass

class ProgramNode(ChtlJsAstNode):
    def __init__(self, statements: List['StatementNode']):
        self.statements = statements

class StatementNode(ChtlJsAstNode):
    def __init__(self, expression):
        self.expression = expression

class SelectorNode(ChtlJsAstNode):
    def __init__(self, selector_text: str):
        self.selector_text = selector_text

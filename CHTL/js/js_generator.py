from .ast.nodes import ProgramNode, StatementNode, SelectorNode

class JavaScriptGenerator:
    """
    Walks the CHTL JS AST and generates standard JavaScript code.
    """
    def visit(self, node):
        method_name = f'visit_{node.__class__.__name__.lower()}'
        visitor_method = getattr(self, method_name, self.visit_default)
        return visitor_method(node)

    def visit_default(self, node):
        raise TypeError(f"No visit method for node type {type(node).__name__}")

    def visit_programnode(self, node: ProgramNode):
        return "".join(self.visit(stmt) for stmt in node.statements)

    def visit_statementnode(self, node: StatementNode):
        return self.visit(node.expression) + ";"

    def visit_selectornode(self, node: SelectorNode):
        return f'document.querySelector("{node.selector_text}")'

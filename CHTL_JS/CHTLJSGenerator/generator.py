from CHTL_JS.CHTLJSNode import nodes

class CHTLJSGenerator:
    def generate(self, ast: nodes.CHTLJS_ProgramNode) -> str:
        """The main entry point for the CHTL JS generator."""
        # The generated code should be wrapped in a script tag or similar
        # by the main CHTL generator. This just generates the raw JS.
        return "".join(self._visit(node) for node in ast.children if node)

    def _visit(self, node):
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, lambda n: "")
        return visitor(node)

    def _visit_listenblocknode(self, node: nodes.ListenBlockNode) -> str:
        # A simple querySelector is used for now. The spec implies a more
        # complex selector mapping, but this is a good start.
        selector = node.target.selector_text

        # A real implementation would need to handle different selector types (.class, #id)
        # and might need to escape the selector string.
        js_str = f"document.querySelector('{selector}').addEventListener("

        # This only handles the first listener for now.
        if node.listeners:
            listener = node.listeners[0]
            js_str += f"'{listener.event_name}', {listener.callback_code});"

        return js_str

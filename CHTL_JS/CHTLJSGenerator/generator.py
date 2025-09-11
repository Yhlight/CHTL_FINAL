from CHTL_JS.CHTLJSNode import nodes

class CHTLJSGenerator:
    def __init__(self, js_fragments: dict):
        self.js_fragments = js_fragments

    def generate(self, ast: nodes.CHTLJS_ProgramNode) -> str:
        return "".join(self._visit(node) for node in ast.children if node)

    def _visit(self, node):
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, lambda n: "")
        return visitor(node)

    def _visit_listenblocknode(self, node: nodes.ListenBlockNode) -> str:
        # Reconstruct the JS code from the AST
        # Strip the outer {{}} from the selector text
        selector_content = node.target.selector_text[2:-2]

        js_str = f"document.querySelector('{selector_content}')"

        # This implementation handles multiple listeners
        for listener in node.listeners:
            raw_callback = self.js_fragments.get(listener.callback_code.strip(), "() => {}")
            js_str += f".addEventListener('{listener.event_name}', {raw_callback});\n"

        return js_str.strip()

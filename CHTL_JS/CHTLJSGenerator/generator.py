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

    def _visit_animatenode(self, node: nodes.AnimateNode) -> str:
        # 1. Construct the keyframes array
        keyframes = []
        if node.begin:
            props = ", ".join([f"{p.key}: '{p.value}'" for p in node.begin])
            keyframes.append(f"{{ offset: 0, {props} }}")

        for kf_node in node.when:
            at_prop = next((p for p in kf_node.properties if p.key == 'at'), None)
            if at_prop:
                offset = float(at_prop.value) / 100.0 # Convert 50 to 0.5
                other_props = [p for p in kf_node.properties if p.key != 'at']
                props_str = ", ".join([f"{p.key}: '{p.value}'" for p in other_props])
                keyframes.append(f"{{ offset: {offset}, {props_str} }}")

        if node.end:
            props = ", ".join([f"{p.key}: '{p.value}'" for p in node.end])
            keyframes.append(f"{{ offset: 1, {props} }}")

        keyframes_str = f"[{', '.join(keyframes)}]"

        # 2. Construct the options object
        options = {}
        if node.duration: options['duration'] = int(node.duration)
        if node.easing: options['easing'] = f"'{node.easing}'"
        if node.loop: options['iterations'] = int(node.loop) if int(node.loop) != -1 else 'Infinity'
        if node.direction: options['direction'] = f"'{node.direction}'"
        if node.delay: options['delay'] = int(node.delay)

        options_str = ", ".join([f"{key}: {value}" for key, value in options.items()])
        options_obj_str = f"{{ {options_str} }}"

        # 3. Get the target
        # Assuming target is an enhanced selector like '{{#box}}'
        selector = node.target.strip()[2:-2] if node.target else 'null'

        # 4. Construct the final JS
        js_str = f"const anim = document.querySelector('{selector}').animate({keyframes_str}, {options_obj_str});"

        # 5. Add the onfinish callback if it exists
        if node.callback:
            raw_callback = self.js_fragments.get(node.callback.strip(), "() => {}")
            js_str += f"\nanim.onfinish = {raw_callback};"

        return js_str

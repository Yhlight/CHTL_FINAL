from CHTL_JS.CHTLJSNode import nodes

class CHTLJSGenerator:
    def __init__(self, js_fragments: dict):
        self.js_fragments = js_fragments
        self.delegated_events = {} # Registry for event delegation

    def generate(self, ast: nodes.CHTLJS_ProgramNode) -> str:
        # First pass: visit all nodes. _visit_delegatenode will populate the registry.
        direct_js = "".join(self._visit(node) for node in ast.children if node)

        # Second pass: generate the consolidated delegate listeners from the registry
        delegated_js = []
        for parent_selector, events in self.delegated_events.items():
            # Create a separate listener for each event type on this parent
            for event_name, data in events.items():
                target_checks = []
                # Using sorted to have a predictable order for testing
                for target_selector in sorted(list(data['target_selectors'])):
                    callback_logic = ""
                    # This is still a simplification; a real implementation might need to map
                    # callbacks to specific targets if the syntax supported it. For now, all
                    # callbacks for this parent/event combo fire if any target matches.
                    for callback_placeholder in data['callbacks']:
                         raw_callback = self.js_fragments.get(callback_placeholder.strip(), "() => {}")
                         callback_logic += f"({raw_callback})(event);"

                    target_checks.append(f"if (event.target.matches('{target_selector}')) {{ {callback_logic} }}")

                listener_body = " else ".join(target_checks)

                # Create the event listener for this specific event type
                delegated_js.append(f"""
                document.querySelector('{parent_selector}').addEventListener('{event_name}', (event) => {{
                    {listener_body}
                }});
                """)

        return direct_js + "\n" + "\n".join(delegated_js)


    def _visit(self, node):
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, lambda n: "")
        return visitor(node)

    def _visit_delegatenode(self, node: nodes.DelegateNode):
        parent_selector = node.parent_selector.selector_text[2:-2]

        if parent_selector not in self.delegated_events:
            self.delegated_events[parent_selector] = {}

        for listener in node.listeners:
            event_name = listener.event_name

            if event_name not in self.delegated_events[parent_selector]:
                self.delegated_events[parent_selector][event_name] = {'target_selectors': set(), 'callbacks': []}

            for target in node.target_selectors:
                self.delegated_events[parent_selector][event_name]['target_selectors'].add(target[2:-2])

            self.delegated_events[parent_selector][event_name]['callbacks'].append(listener.callback_code)

        return "" # Don't return JS directly

    def _visit_listenblocknode(self, node: nodes.ListenBlockNode) -> str:
        selector_content = node.target.selector_text[2:-2]
        js_str = f"document.querySelector('{selector_content}')"
        for listener in node.listeners:
            raw_callback = self.js_fragments.get(listener.callback_code.strip(), "() => {}")
            js_str += f".addEventListener('{listener.event_name}', {raw_callback});\n"
        return js_str.strip()

    def _visit_animatenode(self, node: nodes.AnimateNode) -> str:
        keyframes = []
        if node.begin:
            props = ", ".join([f"{p.key}: '{p.value}'" for p in node.begin])
            keyframes.append(f"{{ offset: 0, {props} }}")

        for kf_node in node.when:
            at_prop = next((p for p in kf_node.properties if p.key == 'at'), None)
            if at_prop:
                offset = float(at_prop.value) / 100.0
                other_props = [p for p in kf_node.properties if p.key != 'at']
                props_str = ", ".join([f"{p.key}: '{p.value}'" for p in other_props])
                keyframes.append(f"{{ offset: {offset}, {props_str} }}")

        if node.end:
            props = ", ".join([f"{p.key}: '{p.value}'" for p in node.end])
            keyframes.append(f"{{ offset: 1, {props} }}")

        keyframes_str = f"[{', '.join(keyframes)}]"

        options = {}
        if node.duration: options['duration'] = int(node.duration)
        if node.easing: options['easing'] = f"'{node.easing}'"
        if node.loop: options['iterations'] = int(node.loop) if int(node.loop) != -1 else 'Infinity'
        if node.direction: options['direction'] = f"'{node.direction}'"
        if node.delay: options['delay'] = int(node.delay)

        options_obj_str = f"{{ {', '.join([f'{k}: {v}' for k, v in options.items()])} }}"

        selector = node.target.strip()[2:-2] if node.target else 'null'

        js_str = f"const anim = document.querySelector('{selector}').animate({keyframes_str}, {options_obj_str});"

        if node.callback:
            raw_callback = self.js_fragments.get(node.callback.strip(), "() => {}")
            js_str += f"\nanim.onfinish = {raw_callback};"

        return js_str

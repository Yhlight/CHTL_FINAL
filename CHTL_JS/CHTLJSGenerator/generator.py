from CHTL_JS.CHTLJSNode import nodes
import json

class CHTLJSGenerator:
    def __init__(self, js_fragments: dict):
        self.js_fragments = js_fragments
        self.delegated_events = {}
        self.virtual_objects = {}
        self.routes = []
        self.router_config = {}

    def generate(self, ast: nodes.CHTLJS_ProgramNode) -> str:
        self._collect_virtual_objects(ast)

        direct_js_parts = [self._visit(node) for node in ast.children if node]
        direct_js = "".join(filter(None, direct_js_parts))

        delegated_js = self._generate_delegated_events()
        router_js = self._generate_router_logic()

        return direct_js + "\n" + delegated_js + "\n" + router_js

    def _generate_delegated_events(self) -> str:
        delegated_js = []
        for parent_selector, events in self.delegated_events.items():
            for event_name, data in events.items():
                target_checks = []
                for target_selector in sorted(list(data['target_selectors'])):
                    callback_logic = ""
                    for callback_placeholder in data['callbacks']:
                         raw_callback = self.js_fragments.get(callback_placeholder.strip(), "() => {}")
                         callback_logic += f"({raw_callback})(event);"
                    target_checks.append(f"if (event.target.matches('{target_selector}')) {{ {callback_logic} }}")

                listener_body = " else ".join(target_checks)
                delegated_js.append(f"document.querySelector('{parent_selector}').addEventListener('{event_name}', (event) => {{ {listener_body} }});")
        return "\n".join(delegated_js)

    def _generate_router_logic(self) -> str:
        if not self.routes:
            return ""

        mode = self.router_config.get('mode', 'history').strip("'\"")
        root_container_selector = self.router_config.get('root', 'body').strip("'\"")
        routes_json = json.dumps(self.routes, indent=4)
        event_name = 'popstate' if mode == 'history' else 'hashchange'

        return f"""
const CHTL_ROUTES = {routes_json};
const CHTL_ROUTER_ROOT = document.querySelector('{root_container_selector}');

const navigate = (url) => {{
    window.history.pushState(null, null, url);
    router();
}};

const router = async () => {{
    const path = window.location.pathname;
    const potentialMatches = CHTL_ROUTES.map(route => ({{
        route: route,
        isMatch: path === route.path
    }}));

    let match = potentialMatches.find(potentialMatch => potentialMatch.isMatch);
    if (!match) {{
        match = {{
            route: CHTL_ROUTES.find(r => r.path === '/404') || CHTL_ROUTES[0],
            isMatch: true
        }};
    }}

    CHTL_ROUTES.forEach(route => {{
        const page = document.querySelector(route.view);
        if (page) {{
            page.style.display = 'none';
        }}
    }});

    const view = document.querySelector(match.route.view);
    if (view) {{
        view.style.display = 'block';
    }}
}};

window.addEventListener('{event_name}', router);
document.addEventListener('DOMContentLoaded', () => {{
    document.body.addEventListener('click', e => {{
        if (e.target.matches('[data-link]')) {{
            e.preventDefault();
            navigate(e.target.href);
        }}
    }});
    router();
}});
"""

    def _collect_virtual_objects(self, node):
        if isinstance(node, nodes.VirtualObjectNode):
            self.virtual_objects[node.name] = node.value
        if hasattr(node, 'children'):
            for child in node.children:
                if child: self._collect_virtual_objects(child)

    def _visit(self, node):
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, lambda n: "")
        return visitor(node)

    def _visit_routernode(self, node: nodes.RouterNode):
        route_defs = {}
        for prop in node.properties:
            if prop.key not in route_defs:
                route_defs[prop.key] = []
            route_defs[prop.key].append(prop.value)

        if 'url' in route_defs and 'page' in route_defs:
            for i, url in enumerate(route_defs['url']):
                if i < len(route_defs['page']):
                    self.routes.append({
                        "path": url.strip("'\""),
                        "view": route_defs['page'][i].strip()[2:-2]
                    })
        if 'root' in route_defs: self.router_config['root'] = route_defs['root'][0]
        if 'mode' in route_defs: self.router_config['mode'] = route_defs['mode'][0]
        return ""


    def _visit_virtualobjectnode(self, node: nodes.VirtualObjectNode):
        return self._visit(node.value)

    def _visit_expressionstatementnode(self, node: nodes.ExpressionStatementNode):
        return self._visit(node.expression)

    def _visit_memberaccessnode(self, node: nodes.MemberAccessNode) -> str:
        virtual_obj = self.virtual_objects.get(node.object_name)
        if not virtual_obj:
            return f"/* ERROR: Virtual object '{node.object_name}' not found. */"
        if isinstance(virtual_obj, nodes.ListenBlockNode):
            for listener in virtual_obj.listeners:
                if listener.event_name == node.member_name:
                    return self.js_fragments.get(listener.callback_code, "() => {}")
        return f"/* ERROR: Member '{node.member_name}' not found on virtual object '{node.object_name}'. */"

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
        return ""

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

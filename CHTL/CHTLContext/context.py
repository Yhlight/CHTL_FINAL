from typing import Dict, List, Any, Optional
from CHTL.CHTLNode.nodes import ImportNode

class CompilationContext:
    """
    A central object to hold state and data across compiler passes.
    Manages symbols within a hierarchical namespace structure.
    """
    def __init__(self):
        self.root_namespace: Dict[str, Any] = {"symbols": {}, "children": {}}
        self.namespace_stack: List[Dict] = [self.root_namespace]

    def _get_namespace_node(self, path: List[str], create_if_missing: bool = False) -> Optional[Dict]:
        current_node = self.root_namespace
        for part in path:
            if part not in current_node["children"]:
                if create_if_missing:
                    current_node["children"][part] = {"symbols": {}, "children": {}}
                else:
                    return None
            current_node = current_node["children"][part]
        return current_node

    def push_namespace(self, name: str):
        current_scope = self.namespace_stack[-1]
        if name not in current_scope["children"]:
            current_scope["children"][name] = {"symbols": {}, "children": {}}
        self.namespace_stack.append(current_scope["children"][name])

    def pop_namespace(self):
        if len(self.namespace_stack) > 1:
            self.namespace_stack.pop()

    def add_symbol(self, name: str, content: Any, symbol_type: str):
        current_scope = self.namespace_stack[-1]
        if symbol_type not in current_scope["symbols"]:
            current_scope["symbols"][symbol_type] = {}
        current_scope["symbols"][symbol_type][name] = content

    def get_symbol(self, name: str, namespace_path: Optional[List[str]], symbol_type: str) -> Any:
        if namespace_path:
            namespace_node = self._get_namespace_node(namespace_path)
            if namespace_node and symbol_type in namespace_node["symbols"]:
                return namespace_node["symbols"][symbol_type].get(name)
        else:
            for scope in reversed(self.namespace_stack):
                if symbol_type in scope["symbols"] and name in scope["symbols"][symbol_type]:
                    return scope["symbols"][symbol_type][name]
        return None

    def merge_from(self, source_context: 'CompilationContext', target_namespace: str, import_details: ImportNode):
        """Merges symbols from a source context based on import rules."""
        # Get the target namespace in the main context, creating it if necessary
        self.push_namespace(target_namespace)
        target_node = self.namespace_stack[-1]

        # The symbols we want to import are in the root of the source context
        source_symbols = source_context.root_namespace["symbols"]

        # TODO: Implement the logic for precise, type, and wildcard imports here.
        # For now, we'll implement a simple merge of all symbols.
        for symbol_type, symbols in source_symbols.items():
            if symbol_type not in target_node["symbols"]:
                target_node["symbols"][symbol_type] = {}
            for name, content in symbols.items():
                target_node["symbols"][symbol_type][name] = content

        self.pop_namespace()


    # Convenience methods
    def add_element_template(self, name: str, content: List[Any]):
        self.add_symbol(name, content, "element_template")

    def get_element_template(self, name: str, namespace_path: Optional[List[str]] = None) -> Optional[List[Any]]:
        return self.get_symbol(name, namespace_path, "element_template")

    # ... other convenience methods can be added as needed

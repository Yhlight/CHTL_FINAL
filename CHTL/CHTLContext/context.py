from typing import Dict, List, Any, Optional

class CompilationContext:
    """
    A central object to hold state and data across compiler passes.
    Manages symbols within a hierarchical namespace structure.
    """
    def __init__(self):
        # The root of the namespace tree
        self.root_namespace: Dict[str, Any] = {"symbols": {}, "children": {}}
        # Stack to keep track of the current namespace during parsing
        self.namespace_stack: List[Dict] = [self.root_namespace]

    def _get_namespace_node(self, path: List[str], create_if_missing: bool = False) -> Optional[Dict]:
        """Navigates to a namespace node in the tree."""
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
        """Enters a new namespace scope."""
        current_scope = self.namespace_stack[-1]
        if name not in current_scope["children"]:
            current_scope["children"][name] = {"symbols": {}, "children": {}}
        self.namespace_stack.append(current_scope["children"][name])

    def pop_namespace(self):
        """Exits the current namespace scope."""
        if len(self.namespace_stack) > 1:
            self.namespace_stack.pop()

    def add_symbol(self, name: str, content: Any, symbol_type: str = "element"):
        """Adds a symbol to the current namespace."""
        current_scope = self.namespace_stack[-1]
        if symbol_type not in current_scope["symbols"]:
            current_scope["symbols"][symbol_type] = {}
        current_scope["symbols"][symbol_type][name] = content

    def get_symbol(self, name: str, namespace_path: Optional[List[str]] = None, symbol_type: str = "element") -> Any:
        """
        Retrieves a symbol from a given namespace.
        If no namespace is provided, it searches up from the current scope.
        """
        if namespace_path:
            # Absolute path lookup
            namespace_node = self._get_namespace_node(namespace_path)
            if namespace_node and symbol_type in namespace_node["symbols"]:
                return namespace_node["symbols"][symbol_type].get(name)
        else:
            # Search from current scope up to the root
            for i in range(len(self.namespace_stack) - 1, -1, -1):
                scope = self.namespace_stack[i]
                if symbol_type in scope["symbols"] and name in scope["symbols"][symbol_type]:
                    return scope["symbols"][symbol_type][name]
        return None

    # Convenience methods for specific symbol types
    def add_element_template(self, name: str, content: List[Any]):
        self.add_symbol(name, content, "element_template")

    def get_element_template(self, name: str, namespace_path: Optional[List[str]] = None) -> Optional[List[Any]]:
        return self.get_symbol(name, namespace_path, "element_template")

    def add_style_template(self, name: str, content: List[Any]):
        self.add_symbol(name, content, "style_template")

    def get_style_template(self, name: str, namespace_path: Optional[List[str]] = None) -> Optional[List[Any]]:
        return self.get_symbol(name, namespace_path, "style_template")

    def add_var_template(self, name: str, content: List[Any]):
        self.add_symbol(name, content, "var_template")

    def get_var_template(self, name: str, namespace_path: Optional[List[str]] = None) -> Optional[List[Any]]:
        return self.get_symbol(name, namespace_path, "var_template")

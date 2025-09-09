from typing import Dict, List, Any, Optional
from collections import defaultdict

class CompilationContext:
    """
    A central object to hold state and data across compiler passes.
    It acts as a symbol table for templates, supporting namespaces.
    """
    DEFAULT_NAMESPACE = "__main__"

    def __init__(self):
        # The outer key is the namespace, the inner key is the template name.
        self.style_templates: Dict[str, Dict[str, List[Any]]] = defaultdict(dict)
        self.element_templates: Dict[str, Dict[str, List[Any]]] = defaultdict(dict)
        self.var_templates: Dict[str, Dict[str, List[Any]]] = defaultdict(dict)

    def add_style_template(self, name: str, content: List[Any], namespace: str = DEFAULT_NAMESPACE):
        self.style_templates[namespace][name] = content

    def get_style_template(self, name: str, namespace: Optional[str] = None) -> List[Any]:
        if namespace:
            return self.style_templates.get(namespace, {}).get(name)
        # If no namespace is provided, search the default namespace first.
        # A more advanced implementation might have a list of 'using' namespaces to search.
        return self.style_templates.get(self.DEFAULT_NAMESPACE, {}).get(name)

    def add_element_template(self, name: str, content: List[Any], namespace: str = DEFAULT_NAMESPACE):
        self.element_templates[namespace][name] = content

    def get_element_template(self, name: str, namespace: Optional[str] = None) -> List[Any]:
        if namespace:
            return self.element_templates.get(namespace, {}).get(name)
        return self.element_templates.get(self.DEFAULT_NAMESPACE, {}).get(name)

    def add_var_template(self, name: str, content: List[Any], namespace: str = DEFAULT_NAMESPACE):
        self.var_templates[namespace][name] = content

    def get_var_template(self, name: str, namespace: Optional[str] = None) -> List[Any]:
        if namespace:
            return self.var_templates.get(namespace, {}).get(name)
        return self.var_templates.get(self.DEFAULT_NAMESPACE, {}).get(name)

if __name__ == '__main__':
    # Example usage
    ctx = CompilationContext()
    ctx.add_element_template("MyBox", ["div_node_1", "span_node_1"])

    retrieved_template = ctx.get_element_template("MyBox")
    print(f"Retrieved template 'MyBox': {retrieved_template}")

    retrieved_missing = ctx.get_element_template("MissingBox")
    print(f"Retrieved missing template 'MissingBox': {retrieved_missing}")

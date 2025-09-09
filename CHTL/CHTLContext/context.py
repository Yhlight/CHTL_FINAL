from typing import Dict, List, Any

class CompilationContext:
    """
    A central object to hold state and data across compiler passes.
    For now, it primarily acts as a symbol table for templates.
    """
    def __init__(self):
        # The key is the template name (str)
        # The value is the list of nodes that make up the template's content
        self.style_templates: Dict[str, List[Any]] = {}
        self.element_templates: Dict[str, List[Any]] = {}
        self.var_templates: Dict[str, List[Any]] = {}

    def add_style_template(self, name: str, content: List[Any]):
        if name in self.style_templates:
            # Handle or warn about redefinition if necessary
            pass
        self.style_templates[name] = content

    def get_style_template(self, name: str) -> List[Any]:
        return self.style_templates.get(name)

    def add_element_template(self, name: str, content: List[Any]):
        if name in self.element_templates:
            pass
        self.element_templates[name] = content

    def get_element_template(self, name: str) -> List[Any]:
        return self.element_templates.get(name)

    def add_var_template(self, name: str, content: List[Any]):
        if name in self.var_templates:
            pass
        self.var_templates[name] = content

    def get_var_template(self, name: str) -> List[Any]:
        return self.var_templates.get(name)

if __name__ == '__main__':
    # Example usage
    ctx = CompilationContext()
    ctx.add_element_template("MyBox", ["div_node_1", "span_node_1"])

    retrieved_template = ctx.get_element_template("MyBox")
    print(f"Retrieved template 'MyBox': {retrieved_template}")

    retrieved_missing = ctx.get_element_template("MissingBox")
    print(f"Retrieved missing template 'MissingBox': {retrieved_missing}")

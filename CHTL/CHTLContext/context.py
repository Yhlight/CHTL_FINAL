from typing import Dict, List, Any, Optional
from collections import defaultdict
from CHTL.CHTLLexer.keywords import TokenType, KEYWORDS as DEFAULT_KEYWORDS

class CompilationContext:
    """
    A central object to hold state and data across compiler passes.
    It acts as a symbol table for templates and holds compiler configuration.
    """
    DEFAULT_NAMESPACE = "__main__"

    def __init__(self):
        # The outer key is the namespace, the inner key is the template name.
        self.style_templates: Dict[str, Dict[str, List[Any]]] = defaultdict(dict)
        self.element_templates: Dict[str, Dict[str, List[Any]]] = defaultdict(dict)
        self.var_templates: Dict[str, Dict[str, List[Any]]] = defaultdict(dict)

        # -- Configuration Data --
        # Holds the mapping of keyword strings to their TokenType.
        # Initialized with the default keywords from the lexer.
        self.keywords: Dict[str, TokenType] = DEFAULT_KEYWORDS.copy()

        # Holds other compiler settings from [Configuration] blocks.
        self.settings: Dict[str, Any] = {
            "INDEX_INITIAL_COUNT": 0,
            "DEBUG_MODE": False,
            "DISABLE_STYLE_AUTO_ADD_CLASS": False,
            "DISABLE_STYLE_AUTO_ADD_ID": False,
        }

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

    def apply_config_string(self, config_string: str):
        """
        Parses a string of configuration settings (the inner content of a
        [Configuration] block) and updates the context.
        This is a simplified parser for a specific job.
        """
        # This simplified parser will just handle key = value; for now.
        # A more robust solution might use regex or a more stateful parser.
        lines = [line.strip() for line in config_string.split('\n')]
        for line in lines:
            if not line or line.startswith('//') or line.startswith('#'):
                continue

            # For now, we won't handle the [Name] block here, just simple settings.
            if '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                # Remove trailing semicolon and strip whitespace/quotes
                value = value.strip().rstrip(';').strip()
                if value.startswith('"') and value.endswith('"'):
                    value = value[1:-1]

                # Simple type conversion
                if value.lower() == 'true':
                    self.settings[key] = True
                elif value.lower() == 'false':
                    self.settings[key] = False
                elif value.isdigit():
                    self.settings[key] = int(value)
                else:
                    self.settings[key] = value

if __name__ == '__main__':
    # Example usage
    ctx = CompilationContext()
    ctx.add_element_template("MyBox", ["div_node_1", "span_node_1"])

    retrieved_template = ctx.get_element_template("MyBox")
    print(f"Retrieved template 'MyBox': {retrieved_template}")

    retrieved_missing = ctx.get_element_template("MissingBox")
    print(f"Retrieved missing template 'MissingBox': {retrieved_missing}")

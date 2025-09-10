from typing import Dict, List, Any
from CHTL.CHTLLexer.keywords import KEYWORDS, TokenType

class CompilationContext:
    """
    A central object to hold state and data across compiler passes.
    It holds symbol tables for templates, modules, and configuration settings.
    """
    def __init__(self):
        self.style_templates: Dict[str, List[Any]] = {}
        self.element_templates: Dict[str, List[Any]] = {}
        self.var_templates: Dict[str, List[Any]] = {}
        self.modules: Dict[str, Any] = {} # For module system
        self.keyword_config: Dict[str, str] = {}

    def set_keyword_config(self, config: Dict[str, str]):
        self.keyword_config = config

    def get_keywords(self) -> Dict[str, TokenType]:
        if not self.keyword_config:
            return KEYWORDS

        # Create a new keyword mapping based on the configuration
        new_keywords = KEYWORDS.copy()
        for old_keyword, new_keyword in self.keyword_config.items():
            # Find the TokenType associated with the original keyword
            token_type = None
            for key, t_type in KEYWORDS.items():
                if key.lower() == old_keyword.lower():
                    token_type = t_type
                    # Remove the old keyword mapping
                    del new_keywords[key]
                    break
            if token_type:
                new_keywords[new_keyword] = token_type
        return new_keywords

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

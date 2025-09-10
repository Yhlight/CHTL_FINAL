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
        """
        config_key_to_tokentype = {
            "KEYWORD_INHERIT": TokenType.INHERIT,
            "KEYWORD_DELETE": TokenType.DELETE,
            "KEYWORD_INSERT": TokenType.INSERT,
            "KEYWORD_AFTER": TokenType.AFTER,
            "KEYWORD_BEFORE": TokenType.BEFORE,
            "KEYWORD_REPLACE": TokenType.REPLACE,
            "KEYWORD_ATTOP": TokenType.AT_KEYWORD, # Note: at top is two words, lexer handles 'at'
            "KEYWORD_ATBOTTOM": TokenType.BOTTOM, # Note: lexer handles 'bottom'
            "KEYWORD_FROM": TokenType.FROM,
            "KEYWORD_AS": TokenType.AS,
            "KEYWORD_EXCEPT": TokenType.EXCEPT,
            "KEYWORD_USE": TokenType.USE,
            "KEYWORD_HTML5": TokenType.HTML5,
            "KEYWORD_TEXT": TokenType.TEXT,
            "KEYWORD_STYLE": TokenType.STYLE,
            "KEYWORD_SCRIPT": TokenType.SCRIPT,
            "KEYWORD_CUSTOM": TokenType.CUSTOM,
            "KEYWORD_TEMPLATE": TokenType.TEMPLATE,
            "KEYWORD_ORIGIN": TokenType.ORIGIN,
            "KEYWORD_IMPORT": TokenType.IMPORT,
            "KEYWORD_NAMESPACE": TokenType.NAMESPACE,
        }

        lines = [line.strip() for line in config_string.split('\n')]
        in_name_block = False
        brace_level = 0

        for line in lines:
            if not line or line.startswith('//'):
                continue

            if '[Name]' in line:
                in_name_block = True
                continue

            if in_name_block:
                if '{' in line: brace_level += 1
                if '}' in line: brace_level -= 1
                if brace_level == 0 and '}' in line:
                    in_name_block = False
                    continue

                if '=' in line:
                    key, value_str = line.split('=', 1)
                    key = key.strip()
                    value_str = value_str.strip().rstrip(';').strip()

                    token_type_to_update = config_key_to_tokentype.get(key)
                    if not token_type_to_update:
                        continue

                    # Remove all old keywords pointing to this TokenType
                    old_keywords = [k for k, v in self.keywords.items() if v == token_type_to_update]
                    for old_key in old_keywords:
                        del self.keywords[old_key]

                    # Add new keyword(s)
                    if value_str.startswith('[') and value_str.endswith(']'):
                        new_keys = [k.strip() for k in value_str[1:-1].split(',') if k.strip()]
                        for new_key in new_keys:
                            self.keywords[new_key] = token_type_to_update
                    elif value_str:
                        self.keywords[value_str] = token_type_to_update

            elif '=' in line: # Handle simple settings
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip().rstrip(';').strip()
                if value.startswith('"') and value.endswith('"'):
                    value = value[1:-1]

                if value.lower() == 'true': self.settings[key] = True
                elif value.lower() == 'false': self.settings[key] = False
                elif value.isdigit(): self.settings[key] = int(value)
                else: self.settings[key] = value

if __name__ == '__main__':
    # Example usage
    ctx = CompilationContext()
    ctx.add_element_template("MyBox", ["div_node_1", "span_node_1"])

    retrieved_template = ctx.get_element_template("MyBox")
    print(f"Retrieved template 'MyBox': {retrieved_template}")

    retrieved_missing = ctx.get_element_template("MissingBox")
    print(f"Retrieved missing template 'MissingBox': {retrieved_missing}")

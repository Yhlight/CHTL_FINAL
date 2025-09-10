from typing import Dict, List, Any, Optional
from CHTL.CHTLLexer.keywords import KEYWORDS, TokenType

class CompilationContext:
    DEFAULT_NAMESPACE = "__main__"

    def __init__(self):
        self.style_templates: Dict[str, Dict[str, List[Any]]] = {self.DEFAULT_NAMESPACE: {}}
        self.element_templates: Dict[str, Dict[str, List[Any]]] = {self.DEFAULT_NAMESPACE: {}}
        self.var_templates: Dict[str, Dict[str, List[Any]]] = {self.DEFAULT_NAMESPACE: {}}
        self.origin_blocks: Dict[str, Dict[str, str]] = {self.DEFAULT_NAMESPACE: {}}
        self.modules: Dict[str, Dict[str, Any]] = {}
        self.keyword_config: Dict[str, str] = {}
        self.doctype: Optional[str] = None

    def add_module_info(self, namespace: str, info: Dict[str, Any]):
        self.modules[namespace] = info

    def set_doctype(self, doctype: str):
        self.doctype = doctype

    def set_keyword_config(self, config: Dict[str, str]):
        self.keyword_config = config

    def get_keywords(self) -> Dict[str, TokenType]:
        if not self.keyword_config:
            return KEYWORDS
        new_keywords = KEYWORDS.copy()
        for old_keyword, new_keyword in self.keyword_config.items():
            for key, t_type in KEYWORDS.items():
                if key.lower() == old_keyword.lower():
                    del new_keywords[key]
                    new_keywords[new_keyword] = t_type
                    break
        return new_keywords

    def _get_namespace_store(self, store: dict, namespace: Optional[str]) -> dict:
        if namespace is None:
            namespace = self.DEFAULT_NAMESPACE
        if namespace not in store:
            store[namespace] = {}
        return store[namespace]

    def add_style_template(self, name: str, content: List[Any], namespace: Optional[str] = None):
        store = self._get_namespace_store(self.style_templates, namespace)
        store[name] = content

    def get_style_template(self, name: str, namespace: Optional[str] = None) -> Optional[List[Any]]:
        if namespace is None: namespace = self.DEFAULT_NAMESPACE
        store = self._get_namespace_store(self.style_templates, namespace)
        template = store.get(name)
        if not template and namespace != self.DEFAULT_NAMESPACE:
            return self.get_style_template(name, self.DEFAULT_NAMESPACE)
        return template

    def add_element_template(self, name: str, content: List[Any], namespace: Optional[str] = None):
        store = self._get_namespace_store(self.element_templates, namespace)
        store[name] = content

    def get_element_template(self, name: str, namespace: Optional[str] = None) -> Optional[List[Any]]:
        if namespace is None: namespace = self.DEFAULT_NAMESPACE
        store = self._get_namespace_store(self.element_templates, namespace)
        template = store.get(name)
        if not template and namespace != self.DEFAULT_NAMESPACE:
            return self.get_element_template(name, self.DEFAULT_NAMESPACE)
        return template

    def add_var_template(self, name: str, content: List[Any], namespace: Optional[str] = None):
        store = self._get_namespace_store(self.var_templates, namespace)
        store[name] = content

    def get_var_template(self, name: str, namespace: Optional[str] = None) -> Optional[List[Any]]:
        if namespace is None: namespace = self.DEFAULT_NAMESPACE
        store = self._get_namespace_store(self.var_templates, namespace)
        return store.get(name)

    def add_origin_block(self, name: str, content: str, namespace: Optional[str] = None):
        store = self._get_namespace_store(self.origin_blocks, namespace)
        store[name] = content

    def get_origin_block(self, name: str, namespace: Optional[str] = None) -> Optional[str]:
        if namespace is None: namespace = self.DEFAULT_NAMESPACE
        store = self._get_namespace_store(self.origin_blocks, namespace)
        return store.get(name)

    def merge_context(self, other_context: 'CompilationContext', as_namespace: str, export_list: Optional[Dict] = None):
        # If there's an export list, only merge the specified items.
        # For now, we ignore the export list and merge everything.
        source_element_templates = other_context.element_templates.get(self.DEFAULT_NAMESPACE, {})
        for name, content in source_element_templates.items():
            self.add_element_template(name, content, namespace=as_namespace)

        source_style_templates = other_context.style_templates.get(self.DEFAULT_NAMESPACE, {})
        for name, content in source_style_templates.items():
            self.add_style_template(name, content, namespace=as_namespace)

        source_var_templates = other_context.var_templates.get(self.DEFAULT_NAMESPACE, {})
        for name, content in source_var_templates.items():
            self.add_var_template(name, content, namespace=as_namespace)

        source_origin_blocks = other_context.origin_blocks.get(self.DEFAULT_NAMESPACE, {})
        for name, content in source_origin_blocks.items():
            self.add_origin_block(name, content, namespace=as_namespace)

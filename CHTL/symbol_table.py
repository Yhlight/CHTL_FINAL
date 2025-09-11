from typing import Dict, Any, Optional, List
from CHTL.ast.nodes import DefinitionNode

class SymbolTable:
    """
    A symbol table to store and retrieve template/custom definitions,
    with support for nested namespaces.
    """
    def __init__(self):
        """
        Initializes the symbol table with a root scope.
        The structure is a nested dictionary representing scopes.
        Each scope contains a '__symbols__' key for its own definitions.
        """
        self.root: Dict[str, Any] = {'__symbols__': {}}

    def _get_or_create_scope(self, namespace: str) -> Dict:
        """Navigates to a namespace, creating nested scopes if they don't exist."""
        if namespace == 'global':
            return self.root
        parts = namespace.split('.')
        current_scope = self.root
        for part in parts:
            if part not in current_scope:
                current_scope[part] = {'__symbols__': {}}
            current_scope = current_scope[part]
        return current_scope

    def define(self, definition: DefinitionNode, namespace: str = 'global'):
        """
        Adds a new definition to the symbol table under a given namespace.
        """
        scope = self._get_or_create_scope(namespace)
        key = (definition.name, definition.def_type)

        if key in scope['__symbols__']:
            print(f"Warning: Symbol '{definition.name}' of type '{definition.def_type}' is being redefined in namespace '{namespace}'.")

        scope['__symbols__'][key] = definition

    def lookup(self, name: str, def_type: str, namespace: str = 'global') -> Optional[DefinitionNode]:
        """
        Looks up a symbol in a specific namespace, traversing up to the global scope if not found.
        """
        if namespace == '.': namespace = 'global'

        namespace_parts = []
        if namespace and namespace != 'global':
            namespace_parts = namespace.split('.')

        # Check from the most specific scope (e.g., a.b.c) up to the root (global)
        for i in range(len(namespace_parts), -1, -1):
            parts_to_check = namespace_parts[:i]

            current_scope = self.root
            path_found = True
            # Traverse to the target scope for this iteration (e.g., self.root[a][b])
            for part in parts_to_check:
                if part in current_scope:
                    current_scope = current_scope.get(part, {})
                else:
                    path_found = False
                    break

            # If scope exists, check for the symbol within its __symbols__ dict
            if path_found:
                symbols = current_scope.get('__symbols__', {})
                key = (name, def_type)
                if key in symbols:
                    return symbols[key]

        # If not found anywhere, return None
        return None

    def merge(self, other_table: 'SymbolTable'):
        """
        Recursively merges another symbol table's root scope into this one.
        """
        if not isinstance(other_table, SymbolTable):
            return

        def _recursive_merge(target_scope, source_scope):
            for key, value in source_scope.items():
                if key == '__symbols__':
                    target_scope.setdefault('__symbols__', {}).update(value)
                elif isinstance(value, dict) and isinstance(target_scope.get(key), dict):
                    _recursive_merge(target_scope[key], value)
                else:
                    # This will overwrite non-dict keys, which is the desired behavior
                    # for merging namespaces.
                    target_scope[key] = value

        _recursive_merge(self.root, other_table.root)

    def __repr__(self):
        # A simple representation for now
        return f"SymbolTable(root={self.root})"

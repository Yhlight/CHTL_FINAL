from typing import Dict, Any, Optional, List
from CHTL.ast.nodes import DefinitionNode

class SymbolTable:
    """
    A symbol table to store and retrieve template/custom definitions,
    with support for namespaces.
    """
    def __init__(self):
        """
        Initializes the symbol table.
        The internal dictionary stores symbols with a key of (namespace, name, type).
        """
        self._symbols: Dict[tuple, DefinitionNode] = {}

    def define(self, definition: DefinitionNode, namespace: str = 'global'):
        """
        Adds a new definition to the symbol table under a given namespace.
        """
        key = (namespace, definition.name, definition.def_type)
        if key in self._symbols:
            print(f"Warning: Symbol '{definition.name}' of type '{definition.def_type}' is being redefined in namespace '{namespace}'.")
        self._symbols[key] = definition

    def lookup(self, name: str, def_type: str, namespace: str = 'global') -> Optional[DefinitionNode]:
        """
        Looks up a symbol in a specific namespace.
        """
        key = (namespace, name, def_type)
        return self._symbols.get(key)

    def get_symbols_in_namespace(self, namespace: str) -> List[DefinitionNode]:
        """
        Retrieves all symbols defined within a specific namespace.
        """
        return [node for key, node in self._symbols.items() if key[0] == namespace]

    def __repr__(self):
        # Group symbols by namespace for a more readable representation
        namespaces = {}
        for (ns, name, type), node in self._symbols.items():
            if ns not in namespaces:
                namespaces[ns] = []
            namespaces[ns].append(f"({name}, {type})")
        return f"SymbolTable(namespaces={namespaces})"

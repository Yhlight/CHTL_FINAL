from typing import Dict, Any, Optional
from CHTL.ast.nodes import DefinitionNode

class SymbolTable:
    """
    A simple symbol table to store and retrieve template/custom definitions.
    """
    def __init__(self):
        """
        Initializes the symbol table.
        The internal dictionary stores symbols with a key of (name, type).
        e.g., ('MyBox', 'Element') -> ElementDefinitionNode(...)
        """
        self._symbols: Dict[tuple, DefinitionNode] = {}

    def define(self, definition: DefinitionNode):
        """
        Adds a new definition to the symbol table.

        Args:
            definition: The DefinitionNode to add.
        """
        key = (definition.name, definition.def_type)
        if key in self._symbols:
            # For now, we'll just print a warning. In a real compiler,
            # this might be a hard error depending on the language rules.
            print(f"Warning: Symbol '{definition.name}' of type '{definition.def_type}' is being redefined.")
        self._symbols[key] = definition

    def lookup(self, name: str, def_type: str) -> Optional[DefinitionNode]:
        """
        Looks up a symbol in the table.

        Args:
            name: The name of the symbol.
            def_type: The type of the symbol ('Style', 'Element', 'Var').

        Returns:
            The found DefinitionNode, or None if it doesn't exist.
        """
        key = (name, def_type)
        return self._symbols.get(key)

    def __repr__(self):
        return f"SymbolTable(symbols={list(self._symbols.keys())})"

import os
from typing import Dict, Any, Tuple, Set
from CHTL.compiler import compile_single_file
from CHTL.symbol_table import SymbolTable
from CHTL.html_generator import HtmlGenerator
from CHTL.template_expander import TemplateExpander
from CHTL.ast.nodes import AstNode, DocumentNode, ConfigurationNode, ImportNode

class CompilerDriver:
    """
    Manages the compilation of a CHTL project, handling multiple files
    and dependencies recursively.
    """
    def __init__(self, entry_filepath: str):
        self.entry_filepath = os.path.abspath(entry_filepath)
        self.project_root = os.path.dirname(self.entry_filepath)
        self.main_symbol_table = SymbolTable()
        self.processed_files: Set[str] = set()
        self.config = {}

    def _find_and_process_imports(self, ast: DocumentNode, current_filepath: str):
        """Finds and compiles imported CHTL files."""
        import_nodes = [node for node in ast.children if isinstance(node, ImportNode)]
        for node in import_nodes:
            if node.import_type == '@Chtl':
                import_path = os.path.abspath(os.path.join(os.path.dirname(current_filepath), node.path))

                if import_path not in self.processed_files:
                    print(f"Importing: {import_path}")
                    imported_table = self._compile_file(import_path)

                    # The imported file manages its own namespaces. We just merge them.
                    self.main_symbol_table.merge(imported_table)

    def _compile_file(self, filepath: str) -> SymbolTable:
        """Compiles a single file and returns its symbol table."""
        if filepath in self.processed_files:
            return SymbolTable() # Avoid circular imports

        self.processed_files.add(filepath)

        # Each file gets its own symbol table initially
        file_symbol_table = SymbolTable()
        ast, _ = compile_single_file(filepath, file_symbol_table)

        # Recursively handle imports within this file
        if isinstance(ast, DocumentNode):
            self._find_and_process_imports(ast, filepath)

        return file_symbol_table


    def compile_project(self):
        """The main entry point to compile a CHTL project."""
        print(f"Compiling entry file: {self.entry_filepath}")

        # First, compile all imported files to populate the main symbol table
        initial_symbols = self._compile_file(self.entry_filepath)
        self.main_symbol_table.merge(initial_symbols)

        # Now, re-compile the entry file, but this time with the full symbol table
        final_ast, final_registry = compile_single_file(self.entry_filepath, self.main_symbol_table)

        # Process configuration and remove directives from the final AST
        if isinstance(final_ast, DocumentNode):
            new_children = []
            for node in final_ast.children:
                if isinstance(node, ConfigurationNode):
                    if node.name is None:
                        for setting in node.settings:
                            self.config[setting.name] = setting.value
                elif isinstance(node, ImportNode):
                    # Imports have already been processed, remove them from the tree
                    pass
                else:
                    new_children.append(node)
            final_ast.children = new_children
        print(f"Loaded config: {self.config}")

        # Run expansion on the final AST
        expander = TemplateExpander(symbol_table=self.main_symbol_table)
        expander.expand(final_ast)

        # The final generation happens after all files are processed and linked.
        html_generator = HtmlGenerator(registry=final_registry)
        html_output = html_generator.visit(final_ast)
        return html_output

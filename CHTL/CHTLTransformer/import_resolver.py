import os
from CHTL.CHTLNode.nodes import DocumentNode, ImportNode, Node
from CHTL.CHTLLoader.loader import Loader
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from typing import Set

class ImportResolver:
    """
    Walks the AST to find and process [Import] statements.
    It loads, parses, and integrates symbols from other files into the
    main compilation context.
    """
    def __init__(self, context: CompilationContext, current_file_path: str):
        self.main_context = context
        self.loader = Loader()
        self.current_dir = os.path.dirname(os.path.abspath(current_file_path))
        # Keep track of which files have been parsed to avoid circular imports
        self.processed_files: Set[str] = set()

    def resolve(self, start_node: Node):
        """Public method to start the resolution process."""
        self._visit(start_node)

    def _visit(self, node: Node):
        """Walks the AST tree."""
        method_name = f'_visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self._generic_visit)
        visitor(node)

    def _generic_visit(self, node: Node):
        """Default visitor, continues traversal."""
        if hasattr(node, 'children'):
            for child in node.children:
                self._visit(child)

    def _parse_imported_file(self, file_path: str) -> CompilationContext:
        """Loads and parses a file into a new, temporary context."""
        absolute_path = os.path.abspath(os.path.join(self.current_dir, file_path))

        if absolute_path in self.processed_files:
            # Avoid circular dependency by not re-parsing an already seen file in this chain
            return CompilationContext() # Return an empty context

        self.processed_files.add(absolute_path)

        file_content = self.loader.load(file_path, self.current_dir)

        temp_context = CompilationContext()
        imported_lexer = Lexer(file_content)
        imported_tokens = imported_lexer.tokenize()

        # The imported file should define its own namespace, or it defaults to the filename
        imported_parser = Parser(imported_tokens, temp_context)
        imported_ast = imported_parser.parse()

        # Recursively resolve imports within the imported file, using its temp context
        nested_resolver = ImportResolver(temp_context, absolute_path)
        nested_resolver.resolve(imported_ast)

        return temp_context

    def _visit_ImportNode(self, node: ImportNode):
        """Processes an import statement by selectively copying symbols."""
        try:
            temp_context = self._parse_imported_file(node.file_path)

            # Determine the target namespace in the main context
            target_namespace = node.alias or os.path.splitext(os.path.basename(node.file_path))[0]

            # This is where the new merge logic will go.
            # We'll need to add a `merge_from` method to CompilationContext.
            # It will take the source context, the import details, and the target namespace.
            self.main_context.merge_from(
                source_context=temp_context,
                target_namespace=target_namespace,
                import_details=node
            )

        except (FileNotFoundError, IOError) as e:
            print(f"Warning: Could not process import for '{node.file_path}'. Reason: {e}")

    def _visit_ElementNode(self, node: ElementNode):
        self._generic_visit(node)

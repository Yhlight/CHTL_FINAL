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
        self.context = context
        self.loader = Loader()
        self.current_file_path = os.path.dirname(os.path.abspath(current_file_path))
        self.processed_imports: Set[str] = set()

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

    def _visit_ImportNode(self, node: ImportNode):
        """Processes an import statement."""
        try:
            # Resolve and load the file content
            # The loader will cache files, so we don't need to check processed_imports here
            # if the path resolution is canonical.
            file_content = self.loader.load(node.file_path, self.current_file_path)

            # For now, we are implementing a simplified "full file" import.
            # The logic for specific item imports will be added later.
            print(f"Importing symbols from: {node.file_path}")

            # The imported file needs to be parsed within the same context
            # to add its symbols to our shared namespace tree.
            # We also need to tell its resolver where it lives.
            imported_lexer = Lexer(file_content)
            imported_tokens = imported_lexer.tokenize()

            # The parser for the imported file needs to know its own namespace.
            # By default, this is the filename without extension.
            imported_filename = os.path.splitext(os.path.basename(node.file_path))[0]

            self.context.push_namespace(node.alias or imported_filename)

            # Parse the imported file. The parser will add symbols to the new namespace.
            imported_parser = Parser(imported_tokens, self.context)
            imported_ast = imported_parser.parse()

            # After parsing, we might need to recursively resolve imports in that file too.
            # This creates a new resolver with the context of the imported file.
            nested_resolver = ImportResolver(self.context, node.file_path)
            nested_resolver.resolve(imported_ast)

            self.context.pop_namespace()

        except (FileNotFoundError, IOError) as e:
            print(f"Warning: Could not process import for '{node.file_path}'. Reason: {e}")

        # After processing, we don't need to visit children of the import node
        # as it has no children.

    def _visit_ElementNode(self, node: ElementNode):
        # Continue traversal for nested elements
        self._generic_visit(node)

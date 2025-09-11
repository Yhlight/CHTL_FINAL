from CHTL.compiler import compile_single_file, process_scripts
from CHTL.symbol_table import SymbolTable
from CHTL.html_generator import HtmlGenerator
from CHTL.template_expander import TemplateExpander
from CHTL.ast.nodes import DocumentNode, ConfigurationNode

class CompilerDriver:
    """
    Manages the compilation of a CHTL project, handling multiple files
    and dependencies.
    """
    def __init__(self, entry_filepath: str):
        self.entry_filepath = entry_filepath
        self.file_asts = {} # Cache for parsed file ASTs
        self.symbol_table = SymbolTable()
        self.config = {} # Will be initialized later

    def _process_config(self, ast: DocumentNode):
        """
        Finds and applies configuration settings from the AST.
        It also removes the configuration nodes from the AST so that
        later passes don't need to handle them.
        """
        new_children = []
        for node in ast.children:
            if isinstance(node, ConfigurationNode):
                # We only apply un-named configurations for now
                if node.name is None:
                    for setting in node.settings:
                        self.config[setting.name] = setting.value
            else:
                new_children.append(node)
        ast.children = new_children

    def compile_project(self):
        """
        The main entry point to compile a CHTL project.
        """
        print(f"Compiling entry file: {self.entry_filepath}")

        ast, registry = compile_single_file(self.entry_filepath, self.symbol_table)

        # Process configuration first
        self._process_config(ast)
        print(f"Loaded config: {self.config}")

        # Run expansion and script processing on the final AST
        expander = TemplateExpander(symbol_table=self.symbol_table)
        expander.expand(ast)

        # Script processing is now handled entirely by the HtmlGenerator by finding ScriptNodes.
        # The old process_scripts function is obsolete.

        # The final generation happens after all files are processed and linked.
        html_generator = HtmlGenerator(registry=registry)
        html_output = html_generator.visit(ast)
        return html_output

from Scanner.CHTLUnifiedScanner import CodeFragment, FragmentType
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator
from CHTL.CHTLContext.context import CompilationContext

class CompilerDispatcher:
    def __init__(self, fragments: list[CodeFragment], context: CompilationContext, current_file_path: str):
        self.fragments = fragments
        self.current_file_path = current_file_path
        self.context = context
        self.html_output = ""
        self.css_output = ""
        self.js_output = ""

    def dispatch(self):
        """
        Routes fragments to the appropriate compilers.
        """
        for fragment in self.fragments:
            if fragment.type == FragmentType.CHTL:
                self.compile_chtl(fragment.content)
            elif fragment.type == FragmentType.CSS:
                # In the future, this will call the CSS compiler
                self.css_output += fragment.content
            elif fragment.type == FragmentType.JS:
                # In the future, this will call the JS compiler
                self.js_output += fragment.content
            elif fragment.type == FragmentType.CHTL_JS:
                # In the future, this will call the CHTL JS compiler
                pass

    def compile_chtl(self, source_code: str):
        """
        Runs the full CHTL compilation pipeline.
        """
        # The lexer now needs the context to get the correct keywords
        lexer = Lexer(source_code, self.context)
        tokens = lexer.tokenize()
        parser = Parser(tokens, self.context)
        ast = parser.parse()
        transformer = ASTTransformer(ast, self.context, self.current_file_path)
        transformed_ast = transformer.transform()
        generator = HTMLGenerator(transformed_ast)

        html, css = generator.generate_parts()
        self.html_output += html
        self.css_output += css


    def merge_outputs(self, use_default_structure: bool = False) -> str:
        """
        Merges the compiled HTML, CSS, and JS into a single HTML document.
        """
        if not use_default_structure:
            parts = []
            if self.css_output:
                parts.append(f"<style>\n{self.css_output}\n</style>")
            if self.html_output:
                parts.append(self.html_output)
            if self.js_output:
                parts.append(f"<script>\n{self.js_output}\n</script>")
            return "\n".join(parts)
        else:
            final_html = f"""
<!DOCTYPE html>
<html>
<head>
    <style>
{self.css_output}
    </style>
</head>
<body>
{self.html_output}
    <script>
{self.js_output}
    </script>
</body>
</html>
"""
            return final_html

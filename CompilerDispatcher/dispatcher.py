from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator
from Scanner.CHTLUnifiedScanner import CodeFragment, FragmentType

class CompilerDispatcher:
    def __init__(self, fragments: list[CodeFragment], context: CompilationContext, source_file_path: str):
        self.fragments = fragments
        self.context = context
        self.source_file_path = source_file_path
        self.html_output = ""
        self.css_output = ""
        self.js_output = ""

    def dispatch(self):
        """
        Routes fragments to the appropriate compilers.
        """
        for fragment in self.fragments:
            if fragment.type == FragmentType.CHTL:
                # CHTL fragments are compiled fully
                html, css = self.compile_chtl(fragment.content)
                self.html_output += html
                self.css_output += css
            elif fragment.type == FragmentType.CSS:
                # For now, just extract the content of top-level style blocks
                # and add it to the CSS output.
                # A real CSS compiler would parse and process this.
                # Remove the 'style {' and '}' wrapper.
                inner_content = fragment.content.strip()
                if inner_content.startswith("style {"):
                    inner_content = inner_content[len("style {"):-1].strip()
                self.css_output += inner_content
            elif fragment.type == FragmentType.CHTL_JS:
                # For now, just add a placeholder for JS output
                self.js_output += f"\n// CHTL_JS block found:\n{fragment.content}\n"

    def compile_chtl(self, source_code: str) -> (str, str):
        """
        Runs the full CHTL compilation pipeline.
        """
        lexer = Lexer(source_code, self.context)
        tokens = lexer.tokenize()
        parser = Parser(tokens, self.context)
        ast = parser.parse()
        transformer = ASTTransformer(ast, self.context, self.source_file_path)
        transformed_ast = transformer.transform()
        generator = HTMLGenerator(transformed_ast, self.context)

        html, css = generator.generate_parts()
        return html, css

    def merge_outputs(self, use_default_structure: bool = False) -> str:
        """
        Merges the compiled HTML, CSS, and JS into a single HTML document.
        """
        if not use_default_structure:
            parts = []
            if self.css_output.strip():
                parts.append(f"<style>\n{self.css_output.strip()}\n</style>")
            if self.html_output.strip():
                parts.append(self.html_output.strip())
            if self.js_output.strip():
                parts.append(f"<script>\n{self.js_output.strip()}\n</script>")
            return "\n".join(parts)
        else:
            final_html = f"""<!DOCTYPE html>
<html>
<head>
    <style>
{self.css_output.strip()}
    </style>
</head>
<body>
{self.html_output.strip()}
    <script>
{self.js_output.strip()}
    </script>
</body>
</html>"""
            return final_html

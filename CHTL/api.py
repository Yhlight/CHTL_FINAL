from .CHTLLexer.lexer import Lexer
from .CHTLParser.parser import Parser
from .CHTLTransformer.transformer import ASTTransformer
from .CHTLGenerator.generator import HTMLGenerator
from .CHTLContext.context import CompilationContext

def compile_chtl(source_code: str) -> str:
    """
    Runs the CHTL compilation pipeline on a CHTL code fragment.
    This is the main API entry point for the CHTL compiler.
    """
    if not source_code.strip():
        return ""

    context = CompilationContext()
    lexer = Lexer(source_code)
    tokens = lexer.tokenize()
    parser = Parser(tokens, context)
    ast = parser.parse()
    transformer = ASTTransformer(ast, context)
    transformed_ast = transformer.transform()
    generator = HTMLGenerator(transformed_ast)
    html_output = generator.generate()

    return html_output

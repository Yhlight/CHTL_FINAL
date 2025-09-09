from CHTL.CHTLLexer.lexer import Token, TokenType
from CHTL.CHTLNode.nodes import ExpressionNode, LiteralNode, BinaryOpNode

# Operator precedence mapping
PRECEDENCE = {
    TokenType.OR: 1,
    TokenType.AND: 2,
    TokenType.EQUALS: 3, # Assuming == for conditionals later
    TokenType.LT: 4, TokenType.GT: 4,
    TokenType.PLUS: 5, TokenType.MINUS: 5,
    TokenType.STAR: 6, TokenType.SLASH: 6, TokenType.PERCENT: 6,
    TokenType.POWER: 7,
}

class ExpressionParser:
    def __init__(self, parser):
        self.parser = parser # A reference to the main parser to consume tokens

    def parse_expression(self, precedence=0):
        # Prefix parsing
        left = self.parse_prefix()

        # Infix parsing
        while not self.parser.current_token().type in [TokenType.SEMICOLON, TokenType.RBRACE, TokenType.EOF]:
            op_token = self.parser.current_token()

            if op_token.type not in PRECEDENCE or PRECEDENCE[op_token.type] <= precedence:
                break

            self.parser.consume()
            right = self.parse_expression(PRECEDENCE[op_token.type])
            left = BinaryOpNode(left=left, op=op_token, right=right)

        return left

    def parse_prefix(self):
        token = self.parser.current_token()
        if token.type == TokenType.IDENTIFIER:
            # Could be a variable, a property reference, or a literal like '100px'
            # For now, treat as a simple literal.
            self.parser.consume()
            return LiteralNode(value=token.value)
        elif token.type == TokenType.UNQUOTED_LITERAL:
            # This is how we get numbers
            self.parser.consume()
            return LiteralNode(value=token.value)
        else:
            raise RuntimeError(f"Unexpected token in expression: {token}")

if __name__ == '__main__':
    from CHTL.CHTLLexer.lexer import Lexer
    from CHTL.CHTLParser.parser import Parser
    from CHTL.CHTLContext.context import CompilationContext
    import json

    source = "width: 100px + 50px * 2;"

    # We need a dummy parser to feed the expression parser
    class DummyParser:
        def __init__(self, tokens):
            self.tokens = tokens
            self.pos = 0
        def current_token(self): return self.tokens[self.pos]
        def consume(self): self.pos += 1

    lexer = Lexer(source)
    tokens = lexer.tokenize()

    # Manually remove the 'width:' part for this test
    dummy_parser = DummyParser(tokens[2:])

    exp_parser = ExpressionParser(dummy_parser)
    exp_ast = exp_parser.parse_expression()

    class AstEncoder(json.JSONEncoder):
        def default(self, o):
            return {k: v for k, v in vars(o).items() if k != 'parent'}

    print(json.dumps(exp_ast, cls=AstEncoder, indent=2))

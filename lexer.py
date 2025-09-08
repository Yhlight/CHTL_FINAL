import re
from collections import namedtuple

Token = namedtuple('Token', ['type', 'value', 'line', 'col'])

class Lexer:
    def __init__(self, source):
        self.source = source
        self.line = 1
        self.col = 1

        token_specification = [
            ('STRING',   r'\"[^\"]*\"'),
            ('NUMERIC_ID',r'[0-9][A-Za-z0-9_.-]*'),     # For values like 16px, 1.5em
            ('ID',       r'[A-Za-z_][A-Za-z0-9_-]*'),
            ('LBRACE',   r'\{'),
            ('RBRACE',   r'\}'),
            ('COLON',    r':'),
            ('SEMI',     r';'),
            ('GEN_CMNT', r'--[^\n]*'),
            ('NEWLINE',  r'\n'),
            ('COMMENT_ML', r'/\*.*?\*/'),
            ('COMMENT_SL', r'//[^\n]*'),
            ('WHITESPACE', r'[ \t]+'),
            ('MISMATCH', r'.'),
        ]
        self.tok_regex = re.compile('|'.join('(?P<%s>%s)' % pair for pair in token_specification), re.DOTALL)

    def tokenize(self):
        for mo in self.tok_regex.finditer(self.source):
            kind = mo.lastgroup
            value = mo.group()

            if kind == 'STRING':
                value = value[1:-1]
            elif kind == 'NEWLINE':
                self.line += 1
                self.col = 1
                continue
            elif kind in ('COMMENT_ML', 'COMMENT_SL', 'WHITESPACE'):
                if kind == 'COMMENT_ML': self.line += value.count('\n')
                self.col += len(value)
                continue
            elif kind == 'MISMATCH':
                raise RuntimeError(f'{value!r} unexpected on line {self.line}')

            yield Token(kind, value, self.line, self.col)
            self.col += len(value)

        yield Token('EOF', '', self.line, self.col)

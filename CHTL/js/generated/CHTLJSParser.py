# Generated from grammars/CHTLJS.g4 by ANTLR 4.13.2
# encoding: utf-8
from antlr4 import *
from io import StringIO
import sys
if sys.version_info[1] > 5:
	from typing import TextIO
else:
	from typing.io import TextIO

def serializedATN():
    return [
        4,1,7,36,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,1,0,5,0,12,8,0,
        10,0,12,0,15,9,0,1,0,1,0,1,1,1,1,3,1,21,8,1,1,2,1,2,1,3,1,3,1,3,
        1,3,1,3,1,3,1,4,4,4,32,8,4,11,4,12,4,33,1,4,0,0,5,0,2,4,6,8,0,1,
        1,0,4,6,33,0,13,1,0,0,0,2,18,1,0,0,0,4,22,1,0,0,0,6,24,1,0,0,0,8,
        31,1,0,0,0,10,12,3,2,1,0,11,10,1,0,0,0,12,15,1,0,0,0,13,11,1,0,0,
        0,13,14,1,0,0,0,14,16,1,0,0,0,15,13,1,0,0,0,16,17,5,0,0,1,17,1,1,
        0,0,0,18,20,3,4,2,0,19,21,5,1,0,0,20,19,1,0,0,0,20,21,1,0,0,0,21,
        3,1,0,0,0,22,23,3,6,3,0,23,5,1,0,0,0,24,25,5,2,0,0,25,26,5,2,0,0,
        26,27,3,8,4,0,27,28,5,3,0,0,28,29,5,3,0,0,29,7,1,0,0,0,30,32,7,0,
        0,0,31,30,1,0,0,0,32,33,1,0,0,0,33,31,1,0,0,0,33,34,1,0,0,0,34,9,
        1,0,0,0,3,13,20,33
    ]

class CHTLJSParser ( Parser ):

    grammarFileName = "CHTLJS.g4"

    atn = ATNDeserializer().deserialize(serializedATN())

    decisionsToDFA = [ DFA(ds, i) for i, ds in enumerate(atn.decisionToState) ]

    sharedContextCache = PredictionContextCache()

    literalNames = [ "<INVALID>", "';'", "'{'", "'}'", "'.'", "'#'" ]

    symbolicNames = [ "<INVALID>", "<INVALID>", "LDBLACE", "RDBLACE", "DOT",
                      "HASH", "IDENTIFIER", "WS" ]

    RULE_program = 0
    RULE_statement = 1
    RULE_expression = 2
    RULE_selector = 3
    RULE_selectorContent = 4

    ruleNames =  [ "program", "statement", "expression", "selector", "selectorContent" ]

    EOF = Token.EOF
    T__0=1
    LDBLACE=2
    RDBLACE=3
    DOT=4
    HASH=5
    IDENTIFIER=6
    WS=7

    def __init__(self, input:TokenStream, output:TextIO = sys.stdout):
        super().__init__(input, output)
        self.checkVersion("4.13.2")
        self._interp = ParserATNSimulator(self, self.atn, self.decisionsToDFA, self.sharedContextCache)
        self._predicates = None




    class ProgramContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def EOF(self):
            return self.getToken(CHTLJSParser.EOF, 0)

        def statement(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLJSParser.StatementContext)
            else:
                return self.getTypedRuleContext(CHTLJSParser.StatementContext,i)


        def getRuleIndex(self):
            return CHTLJSParser.RULE_program

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterProgram" ):
                listener.enterProgram(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitProgram" ):
                listener.exitProgram(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitProgram" ):
                return visitor.visitProgram(self)
            else:
                return visitor.visitChildren(self)




    def program(self):

        localctx = CHTLJSParser.ProgramContext(self, self._ctx, self.state)
        self.enterRule(localctx, 0, self.RULE_program)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 13
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==2:
                self.state = 10
                self.statement()
                self.state = 15
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 16
            self.match(CHTLJSParser.EOF)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class StatementContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def expression(self):
            return self.getTypedRuleContext(CHTLJSParser.ExpressionContext,0)


        def getRuleIndex(self):
            return CHTLJSParser.RULE_statement

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterStatement" ):
                listener.enterStatement(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitStatement" ):
                listener.exitStatement(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitStatement" ):
                return visitor.visitStatement(self)
            else:
                return visitor.visitChildren(self)




    def statement(self):

        localctx = CHTLJSParser.StatementContext(self, self._ctx, self.state)
        self.enterRule(localctx, 2, self.RULE_statement)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 18
            self.expression()
            self.state = 20
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==1:
                self.state = 19
                self.match(CHTLJSParser.T__0)


        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class ExpressionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def selector(self):
            return self.getTypedRuleContext(CHTLJSParser.SelectorContext,0)


        def getRuleIndex(self):
            return CHTLJSParser.RULE_expression

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterExpression" ):
                listener.enterExpression(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitExpression" ):
                listener.exitExpression(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitExpression" ):
                return visitor.visitExpression(self)
            else:
                return visitor.visitChildren(self)




    def expression(self):

        localctx = CHTLJSParser.ExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 4, self.RULE_expression)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 22
            self.selector()
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class SelectorContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def LDBLACE(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLJSParser.LDBLACE)
            else:
                return self.getToken(CHTLJSParser.LDBLACE, i)

        def selectorContent(self):
            return self.getTypedRuleContext(CHTLJSParser.SelectorContentContext,0)


        def RDBLACE(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLJSParser.RDBLACE)
            else:
                return self.getToken(CHTLJSParser.RDBLACE, i)

        def getRuleIndex(self):
            return CHTLJSParser.RULE_selector

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterSelector" ):
                listener.enterSelector(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitSelector" ):
                listener.exitSelector(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitSelector" ):
                return visitor.visitSelector(self)
            else:
                return visitor.visitChildren(self)




    def selector(self):

        localctx = CHTLJSParser.SelectorContext(self, self._ctx, self.state)
        self.enterRule(localctx, 6, self.RULE_selector)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 24
            self.match(CHTLJSParser.LDBLACE)
            self.state = 25
            self.match(CHTLJSParser.LDBLACE)
            self.state = 26
            self.selectorContent()
            self.state = 27
            self.match(CHTLJSParser.RDBLACE)
            self.state = 28
            self.match(CHTLJSParser.RDBLACE)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class SelectorContentContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def IDENTIFIER(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLJSParser.IDENTIFIER)
            else:
                return self.getToken(CHTLJSParser.IDENTIFIER, i)

        def DOT(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLJSParser.DOT)
            else:
                return self.getToken(CHTLJSParser.DOT, i)

        def HASH(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLJSParser.HASH)
            else:
                return self.getToken(CHTLJSParser.HASH, i)

        def getRuleIndex(self):
            return CHTLJSParser.RULE_selectorContent

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterSelectorContent" ):
                listener.enterSelectorContent(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitSelectorContent" ):
                listener.exitSelectorContent(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitSelectorContent" ):
                return visitor.visitSelectorContent(self)
            else:
                return visitor.visitChildren(self)




    def selectorContent(self):

        localctx = CHTLJSParser.SelectorContentContext(self, self._ctx, self.state)
        self.enterRule(localctx, 8, self.RULE_selectorContent)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 31
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while True:
                self.state = 30
                _la = self._input.LA(1)
                if not((((_la) & ~0x3f) == 0 and ((1 << _la) & 112) != 0)):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 33
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                if not ((((_la) & ~0x3f) == 0 and ((1 << _la) & 112) != 0)):
                    break

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx

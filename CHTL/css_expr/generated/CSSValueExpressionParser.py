# Generated from grammars/CSSValueExpression.g4 by ANTLR 4.13.2
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
        4,1,25,116,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
        6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,1,0,1,0,
        1,1,1,1,1,1,1,1,1,1,1,1,3,1,35,8,1,1,2,1,2,1,2,5,2,40,8,2,10,2,12,
        2,43,9,2,1,3,1,3,1,3,5,3,48,8,3,10,3,12,3,51,9,3,1,4,1,4,1,4,5,4,
        56,8,4,10,4,12,4,59,9,4,1,5,1,5,1,5,5,5,64,8,5,10,5,12,5,67,9,5,
        1,6,1,6,1,6,5,6,72,8,6,10,6,12,6,75,9,6,1,7,1,7,1,7,5,7,80,8,7,10,
        7,12,7,83,9,7,1,8,1,8,1,8,5,8,88,8,8,10,8,12,8,91,9,8,1,9,1,9,1,
        9,3,9,96,8,9,1,10,1,10,1,10,1,10,1,10,1,10,3,10,104,8,10,1,11,1,
        11,1,12,1,12,1,12,5,12,111,8,12,10,12,12,12,114,9,12,1,12,0,0,13,
        0,2,4,6,8,10,12,14,16,18,20,22,24,0,5,1,0,11,12,1,0,7,10,1,0,1,2,
        1,0,3,5,1,0,20,24,114,0,26,1,0,0,0,2,28,1,0,0,0,4,36,1,0,0,0,6,44,
        1,0,0,0,8,52,1,0,0,0,10,60,1,0,0,0,12,68,1,0,0,0,14,76,1,0,0,0,16,
        84,1,0,0,0,18,95,1,0,0,0,20,103,1,0,0,0,22,105,1,0,0,0,24,107,1,
        0,0,0,26,27,3,2,1,0,27,1,1,0,0,0,28,34,3,4,2,0,29,30,5,18,0,0,30,
        31,3,2,1,0,31,32,5,19,0,0,32,33,3,2,1,0,33,35,1,0,0,0,34,29,1,0,
        0,0,34,35,1,0,0,0,35,3,1,0,0,0,36,41,3,6,3,0,37,38,5,14,0,0,38,40,
        3,6,3,0,39,37,1,0,0,0,40,43,1,0,0,0,41,39,1,0,0,0,41,42,1,0,0,0,
        42,5,1,0,0,0,43,41,1,0,0,0,44,49,3,8,4,0,45,46,5,13,0,0,46,48,3,
        8,4,0,47,45,1,0,0,0,48,51,1,0,0,0,49,47,1,0,0,0,49,50,1,0,0,0,50,
        7,1,0,0,0,51,49,1,0,0,0,52,57,3,10,5,0,53,54,7,0,0,0,54,56,3,10,
        5,0,55,53,1,0,0,0,56,59,1,0,0,0,57,55,1,0,0,0,57,58,1,0,0,0,58,9,
        1,0,0,0,59,57,1,0,0,0,60,65,3,12,6,0,61,62,7,1,0,0,62,64,3,12,6,
        0,63,61,1,0,0,0,64,67,1,0,0,0,65,63,1,0,0,0,65,66,1,0,0,0,66,11,
        1,0,0,0,67,65,1,0,0,0,68,73,3,14,7,0,69,70,7,2,0,0,70,72,3,14,7,
        0,71,69,1,0,0,0,72,75,1,0,0,0,73,71,1,0,0,0,73,74,1,0,0,0,74,13,
        1,0,0,0,75,73,1,0,0,0,76,81,3,16,8,0,77,78,7,3,0,0,78,80,3,16,8,
        0,79,77,1,0,0,0,80,83,1,0,0,0,81,79,1,0,0,0,81,82,1,0,0,0,82,15,
        1,0,0,0,83,81,1,0,0,0,84,89,3,18,9,0,85,86,5,6,0,0,86,88,3,18,9,
        0,87,85,1,0,0,0,88,91,1,0,0,0,89,87,1,0,0,0,89,90,1,0,0,0,90,17,
        1,0,0,0,91,89,1,0,0,0,92,93,7,2,0,0,93,96,3,18,9,0,94,96,3,20,10,
        0,95,92,1,0,0,0,95,94,1,0,0,0,96,19,1,0,0,0,97,104,3,24,12,0,98,
        104,3,22,11,0,99,100,5,15,0,0,100,101,3,2,1,0,101,102,5,16,0,0,102,
        104,1,0,0,0,103,97,1,0,0,0,103,98,1,0,0,0,103,99,1,0,0,0,104,21,
        1,0,0,0,105,106,7,4,0,0,106,23,1,0,0,0,107,112,5,24,0,0,108,109,
        5,17,0,0,109,111,5,24,0,0,110,108,1,0,0,0,111,114,1,0,0,0,112,110,
        1,0,0,0,112,113,1,0,0,0,113,25,1,0,0,0,114,112,1,0,0,0,11,34,41,
        49,57,65,73,81,89,95,103,112
    ]

class CSSValueExpressionParser ( Parser ):

    grammarFileName = "CSSValueExpression.g4"

    atn = ATNDeserializer().deserialize(serializedATN())

    decisionsToDFA = [ DFA(ds, i) for i, ds in enumerate(atn.decisionToState) ]

    sharedContextCache = PredictionContextCache()

    literalNames = [ "<INVALID>", "'+'", "'-'", "'*'", "'/'", "'%'", "'**'",
                     "'>'", "'>='", "'<'", "'<='", "'=='", "'!='", "'&&'",
                     "'||'", "'('", "')'", "'.'", "'?'", "':'" ]

    symbolicNames = [ "<INVALID>", "ADD", "SUB", "MUL", "DIV", "MOD", "POW",
                      "GT", "GTE", "LT", "LTE", "EQ", "NEQ", "AND", "OR",
                      "LPAR", "RPAR", "DOT", "QUESTION", "COLON", "DIMENSION",
                      "NUMBER", "STRING", "COLOR", "IDENTIFIER", "WS" ]

    RULE_parse = 0
    RULE_expression = 1
    RULE_logicalOrExpression = 2
    RULE_logicalAndExpression = 3
    RULE_equalityExpression = 4
    RULE_relationalExpression = 5
    RULE_additiveExpression = 6
    RULE_multiplicativeExpression = 7
    RULE_powerExpression = 8
    RULE_unaryExpression = 9
    RULE_atom = 10
    RULE_literal = 11
    RULE_propertyReference = 12

    ruleNames =  [ "parse", "expression", "logicalOrExpression", "logicalAndExpression",
                   "equalityExpression", "relationalExpression", "additiveExpression",
                   "multiplicativeExpression", "powerExpression", "unaryExpression",
                   "atom", "literal", "propertyReference" ]

    EOF = Token.EOF
    ADD=1
    SUB=2
    MUL=3
    DIV=4
    MOD=5
    POW=6
    GT=7
    GTE=8
    LT=9
    LTE=10
    EQ=11
    NEQ=12
    AND=13
    OR=14
    LPAR=15
    RPAR=16
    DOT=17
    QUESTION=18
    COLON=19
    DIMENSION=20
    NUMBER=21
    STRING=22
    COLOR=23
    IDENTIFIER=24
    WS=25

    def __init__(self, input:TokenStream, output:TextIO = sys.stdout):
        super().__init__(input, output)
        self.checkVersion("4.13.2")
        self._interp = ParserATNSimulator(self, self.atn, self.decisionsToDFA, self.sharedContextCache)
        self._predicates = None




    class ParseContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def expression(self):
            return self.getTypedRuleContext(CSSValueExpressionParser.ExpressionContext,0)


        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_parse

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterParse" ):
                listener.enterParse(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitParse" ):
                listener.exitParse(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitParse" ):
                return visitor.visitParse(self)
            else:
                return visitor.visitChildren(self)




    def parse(self):

        localctx = CSSValueExpressionParser.ParseContext(self, self._ctx, self.state)
        self.enterRule(localctx, 0, self.RULE_parse)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 26
            self.expression()
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

        def logicalOrExpression(self):
            return self.getTypedRuleContext(CSSValueExpressionParser.LogicalOrExpressionContext,0)


        def QUESTION(self):
            return self.getToken(CSSValueExpressionParser.QUESTION, 0)

        def expression(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CSSValueExpressionParser.ExpressionContext)
            else:
                return self.getTypedRuleContext(CSSValueExpressionParser.ExpressionContext,i)


        def COLON(self):
            return self.getToken(CSSValueExpressionParser.COLON, 0)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_expression

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

        localctx = CSSValueExpressionParser.ExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 2, self.RULE_expression)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 28
            self.logicalOrExpression()
            self.state = 34
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==18:
                self.state = 29
                self.match(CSSValueExpressionParser.QUESTION)
                self.state = 30
                self.expression()
                self.state = 31
                self.match(CSSValueExpressionParser.COLON)
                self.state = 32
                self.expression()


        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class LogicalOrExpressionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def logicalAndExpression(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CSSValueExpressionParser.LogicalAndExpressionContext)
            else:
                return self.getTypedRuleContext(CSSValueExpressionParser.LogicalAndExpressionContext,i)


        def OR(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.OR)
            else:
                return self.getToken(CSSValueExpressionParser.OR, i)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_logicalOrExpression

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterLogicalOrExpression" ):
                listener.enterLogicalOrExpression(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitLogicalOrExpression" ):
                listener.exitLogicalOrExpression(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitLogicalOrExpression" ):
                return visitor.visitLogicalOrExpression(self)
            else:
                return visitor.visitChildren(self)




    def logicalOrExpression(self):

        localctx = CSSValueExpressionParser.LogicalOrExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 4, self.RULE_logicalOrExpression)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 36
            self.logicalAndExpression()
            self.state = 41
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==14:
                self.state = 37
                self.match(CSSValueExpressionParser.OR)
                self.state = 38
                self.logicalAndExpression()
                self.state = 43
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class LogicalAndExpressionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def equalityExpression(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CSSValueExpressionParser.EqualityExpressionContext)
            else:
                return self.getTypedRuleContext(CSSValueExpressionParser.EqualityExpressionContext,i)


        def AND(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.AND)
            else:
                return self.getToken(CSSValueExpressionParser.AND, i)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_logicalAndExpression

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterLogicalAndExpression" ):
                listener.enterLogicalAndExpression(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitLogicalAndExpression" ):
                listener.exitLogicalAndExpression(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitLogicalAndExpression" ):
                return visitor.visitLogicalAndExpression(self)
            else:
                return visitor.visitChildren(self)




    def logicalAndExpression(self):

        localctx = CSSValueExpressionParser.LogicalAndExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 6, self.RULE_logicalAndExpression)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 44
            self.equalityExpression()
            self.state = 49
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==13:
                self.state = 45
                self.match(CSSValueExpressionParser.AND)
                self.state = 46
                self.equalityExpression()
                self.state = 51
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class EqualityExpressionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def relationalExpression(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CSSValueExpressionParser.RelationalExpressionContext)
            else:
                return self.getTypedRuleContext(CSSValueExpressionParser.RelationalExpressionContext,i)


        def EQ(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.EQ)
            else:
                return self.getToken(CSSValueExpressionParser.EQ, i)

        def NEQ(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.NEQ)
            else:
                return self.getToken(CSSValueExpressionParser.NEQ, i)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_equalityExpression

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterEqualityExpression" ):
                listener.enterEqualityExpression(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitEqualityExpression" ):
                listener.exitEqualityExpression(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitEqualityExpression" ):
                return visitor.visitEqualityExpression(self)
            else:
                return visitor.visitChildren(self)




    def equalityExpression(self):

        localctx = CSSValueExpressionParser.EqualityExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 8, self.RULE_equalityExpression)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 52
            self.relationalExpression()
            self.state = 57
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==11 or _la==12:
                self.state = 53
                _la = self._input.LA(1)
                if not(_la==11 or _la==12):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 54
                self.relationalExpression()
                self.state = 59
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class RelationalExpressionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def additiveExpression(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CSSValueExpressionParser.AdditiveExpressionContext)
            else:
                return self.getTypedRuleContext(CSSValueExpressionParser.AdditiveExpressionContext,i)


        def GT(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.GT)
            else:
                return self.getToken(CSSValueExpressionParser.GT, i)

        def GTE(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.GTE)
            else:
                return self.getToken(CSSValueExpressionParser.GTE, i)

        def LT(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.LT)
            else:
                return self.getToken(CSSValueExpressionParser.LT, i)

        def LTE(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.LTE)
            else:
                return self.getToken(CSSValueExpressionParser.LTE, i)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_relationalExpression

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterRelationalExpression" ):
                listener.enterRelationalExpression(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitRelationalExpression" ):
                listener.exitRelationalExpression(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitRelationalExpression" ):
                return visitor.visitRelationalExpression(self)
            else:
                return visitor.visitChildren(self)




    def relationalExpression(self):

        localctx = CSSValueExpressionParser.RelationalExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 10, self.RULE_relationalExpression)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 60
            self.additiveExpression()
            self.state = 65
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 1920) != 0):
                self.state = 61
                _la = self._input.LA(1)
                if not((((_la) & ~0x3f) == 0 and ((1 << _la) & 1920) != 0)):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 62
                self.additiveExpression()
                self.state = 67
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class AdditiveExpressionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def multiplicativeExpression(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CSSValueExpressionParser.MultiplicativeExpressionContext)
            else:
                return self.getTypedRuleContext(CSSValueExpressionParser.MultiplicativeExpressionContext,i)


        def ADD(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.ADD)
            else:
                return self.getToken(CSSValueExpressionParser.ADD, i)

        def SUB(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.SUB)
            else:
                return self.getToken(CSSValueExpressionParser.SUB, i)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_additiveExpression

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterAdditiveExpression" ):
                listener.enterAdditiveExpression(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitAdditiveExpression" ):
                listener.exitAdditiveExpression(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitAdditiveExpression" ):
                return visitor.visitAdditiveExpression(self)
            else:
                return visitor.visitChildren(self)




    def additiveExpression(self):

        localctx = CSSValueExpressionParser.AdditiveExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 12, self.RULE_additiveExpression)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 68
            self.multiplicativeExpression()
            self.state = 73
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==1 or _la==2:
                self.state = 69
                _la = self._input.LA(1)
                if not(_la==1 or _la==2):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 70
                self.multiplicativeExpression()
                self.state = 75
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class MultiplicativeExpressionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def powerExpression(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CSSValueExpressionParser.PowerExpressionContext)
            else:
                return self.getTypedRuleContext(CSSValueExpressionParser.PowerExpressionContext,i)


        def MUL(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.MUL)
            else:
                return self.getToken(CSSValueExpressionParser.MUL, i)

        def DIV(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.DIV)
            else:
                return self.getToken(CSSValueExpressionParser.DIV, i)

        def MOD(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.MOD)
            else:
                return self.getToken(CSSValueExpressionParser.MOD, i)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_multiplicativeExpression

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterMultiplicativeExpression" ):
                listener.enterMultiplicativeExpression(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitMultiplicativeExpression" ):
                listener.exitMultiplicativeExpression(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitMultiplicativeExpression" ):
                return visitor.visitMultiplicativeExpression(self)
            else:
                return visitor.visitChildren(self)




    def multiplicativeExpression(self):

        localctx = CSSValueExpressionParser.MultiplicativeExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 14, self.RULE_multiplicativeExpression)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 76
            self.powerExpression()
            self.state = 81
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 56) != 0):
                self.state = 77
                _la = self._input.LA(1)
                if not((((_la) & ~0x3f) == 0 and ((1 << _la) & 56) != 0)):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 78
                self.powerExpression()
                self.state = 83
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class PowerExpressionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def unaryExpression(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CSSValueExpressionParser.UnaryExpressionContext)
            else:
                return self.getTypedRuleContext(CSSValueExpressionParser.UnaryExpressionContext,i)


        def POW(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.POW)
            else:
                return self.getToken(CSSValueExpressionParser.POW, i)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_powerExpression

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterPowerExpression" ):
                listener.enterPowerExpression(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitPowerExpression" ):
                listener.exitPowerExpression(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitPowerExpression" ):
                return visitor.visitPowerExpression(self)
            else:
                return visitor.visitChildren(self)




    def powerExpression(self):

        localctx = CSSValueExpressionParser.PowerExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 16, self.RULE_powerExpression)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 84
            self.unaryExpression()
            self.state = 89
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==6:
                self.state = 85
                self.match(CSSValueExpressionParser.POW)
                self.state = 86
                self.unaryExpression()
                self.state = 91
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class UnaryExpressionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def unaryExpression(self):
            return self.getTypedRuleContext(CSSValueExpressionParser.UnaryExpressionContext,0)


        def ADD(self):
            return self.getToken(CSSValueExpressionParser.ADD, 0)

        def SUB(self):
            return self.getToken(CSSValueExpressionParser.SUB, 0)

        def atom(self):
            return self.getTypedRuleContext(CSSValueExpressionParser.AtomContext,0)


        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_unaryExpression

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterUnaryExpression" ):
                listener.enterUnaryExpression(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitUnaryExpression" ):
                listener.exitUnaryExpression(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitUnaryExpression" ):
                return visitor.visitUnaryExpression(self)
            else:
                return visitor.visitChildren(self)




    def unaryExpression(self):

        localctx = CSSValueExpressionParser.UnaryExpressionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 18, self.RULE_unaryExpression)
        self._la = 0 # Token type
        try:
            self.state = 95
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [1, 2]:
                self.enterOuterAlt(localctx, 1)
                self.state = 92
                _la = self._input.LA(1)
                if not(_la==1 or _la==2):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 93
                self.unaryExpression()
                pass
            elif token in [15, 20, 21, 22, 23, 24]:
                self.enterOuterAlt(localctx, 2)
                self.state = 94
                self.atom()
                pass
            else:
                raise NoViableAltException(self)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class AtomContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def propertyReference(self):
            return self.getTypedRuleContext(CSSValueExpressionParser.PropertyReferenceContext,0)


        def literal(self):
            return self.getTypedRuleContext(CSSValueExpressionParser.LiteralContext,0)


        def LPAR(self):
            return self.getToken(CSSValueExpressionParser.LPAR, 0)

        def expression(self):
            return self.getTypedRuleContext(CSSValueExpressionParser.ExpressionContext,0)


        def RPAR(self):
            return self.getToken(CSSValueExpressionParser.RPAR, 0)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_atom

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterAtom" ):
                listener.enterAtom(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitAtom" ):
                listener.exitAtom(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitAtom" ):
                return visitor.visitAtom(self)
            else:
                return visitor.visitChildren(self)




    def atom(self):

        localctx = CSSValueExpressionParser.AtomContext(self, self._ctx, self.state)
        self.enterRule(localctx, 20, self.RULE_atom)
        try:
            self.state = 103
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,9,self._ctx)
            if la_ == 1:
                self.enterOuterAlt(localctx, 1)
                self.state = 97
                self.propertyReference()
                pass

            elif la_ == 2:
                self.enterOuterAlt(localctx, 2)
                self.state = 98
                self.literal()
                pass

            elif la_ == 3:
                self.enterOuterAlt(localctx, 3)
                self.state = 99
                self.match(CSSValueExpressionParser.LPAR)
                self.state = 100
                self.expression()
                self.state = 101
                self.match(CSSValueExpressionParser.RPAR)
                pass


        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class LiteralContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def DIMENSION(self):
            return self.getToken(CSSValueExpressionParser.DIMENSION, 0)

        def NUMBER(self):
            return self.getToken(CSSValueExpressionParser.NUMBER, 0)

        def STRING(self):
            return self.getToken(CSSValueExpressionParser.STRING, 0)

        def COLOR(self):
            return self.getToken(CSSValueExpressionParser.COLOR, 0)

        def IDENTIFIER(self):
            return self.getToken(CSSValueExpressionParser.IDENTIFIER, 0)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_literal

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterLiteral" ):
                listener.enterLiteral(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitLiteral" ):
                listener.exitLiteral(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitLiteral" ):
                return visitor.visitLiteral(self)
            else:
                return visitor.visitChildren(self)




    def literal(self):

        localctx = CSSValueExpressionParser.LiteralContext(self, self._ctx, self.state)
        self.enterRule(localctx, 22, self.RULE_literal)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 105
            _la = self._input.LA(1)
            if not((((_la) & ~0x3f) == 0 and ((1 << _la) & 32505856) != 0)):
                self._errHandler.recoverInline(self)
            else:
                self._errHandler.reportMatch(self)
                self.consume()
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class PropertyReferenceContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def IDENTIFIER(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.IDENTIFIER)
            else:
                return self.getToken(CSSValueExpressionParser.IDENTIFIER, i)

        def DOT(self, i:int=None):
            if i is None:
                return self.getTokens(CSSValueExpressionParser.DOT)
            else:
                return self.getToken(CSSValueExpressionParser.DOT, i)

        def getRuleIndex(self):
            return CSSValueExpressionParser.RULE_propertyReference

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterPropertyReference" ):
                listener.enterPropertyReference(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitPropertyReference" ):
                listener.exitPropertyReference(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitPropertyReference" ):
                return visitor.visitPropertyReference(self)
            else:
                return visitor.visitChildren(self)




    def propertyReference(self):

        localctx = CSSValueExpressionParser.PropertyReferenceContext(self, self._ctx, self.state)
        self.enterRule(localctx, 24, self.RULE_propertyReference)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 107
            self.match(CSSValueExpressionParser.IDENTIFIER)
            self.state = 112
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==17:
                self.state = 108
                self.match(CSSValueExpressionParser.DOT)
                self.state = 109
                self.match(CSSValueExpressionParser.IDENTIFIER)
                self.state = 114
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx

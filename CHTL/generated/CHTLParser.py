# Generated from grammars/CHTL.g4 by ANTLR 4.13.2
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
        4,1,22,139,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
        6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,1,0,1,0,
        5,0,29,8,0,10,0,12,0,32,9,0,1,1,1,1,1,1,1,1,3,1,38,8,1,1,1,1,1,1,
        1,1,1,3,1,44,8,1,3,1,46,8,1,1,2,1,2,1,2,1,2,5,2,52,8,2,10,2,12,2,
        55,9,2,1,2,1,2,1,3,1,3,1,3,1,3,5,3,63,8,3,10,3,12,3,66,9,3,1,3,1,
        3,1,4,1,4,1,4,1,4,5,4,74,8,4,10,4,12,4,77,9,4,1,4,1,4,1,5,1,5,1,
        5,1,5,1,5,1,5,1,5,5,5,88,8,5,10,5,12,5,91,9,5,1,5,1,5,1,6,1,6,1,
        6,1,6,3,6,99,8,6,1,6,1,6,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,8,1,8,
        1,9,1,9,1,9,1,9,1,9,1,9,1,10,1,10,1,11,1,11,1,11,4,11,124,8,11,11,
        11,12,11,125,3,11,128,8,11,1,12,1,12,1,12,1,12,1,12,3,12,135,8,12,
        1,12,1,12,1,12,0,0,13,0,2,4,6,8,10,12,14,16,18,20,22,24,0,3,1,0,
        15,16,3,0,8,10,13,13,15,19,2,0,7,8,18,18,145,0,30,1,0,0,0,2,45,1,
        0,0,0,4,47,1,0,0,0,6,58,1,0,0,0,8,69,1,0,0,0,10,80,1,0,0,0,12,94,
        1,0,0,0,14,102,1,0,0,0,16,107,1,0,0,0,18,112,1,0,0,0,20,118,1,0,
        0,0,22,127,1,0,0,0,24,129,1,0,0,0,26,29,3,2,1,0,27,29,3,10,5,0,28,
        26,1,0,0,0,28,27,1,0,0,0,29,32,1,0,0,0,30,28,1,0,0,0,30,31,1,0,0,
        0,31,1,1,0,0,0,32,30,1,0,0,0,33,37,5,1,0,0,34,38,3,4,2,0,35,38,3,
        6,3,0,36,38,3,8,4,0,37,34,1,0,0,0,37,35,1,0,0,0,37,36,1,0,0,0,38,
        46,1,0,0,0,39,43,5,2,0,0,40,44,3,4,2,0,41,44,3,6,3,0,42,44,3,8,4,
        0,43,40,1,0,0,0,43,41,1,0,0,0,43,42,1,0,0,0,44,46,1,0,0,0,45,33,
        1,0,0,0,45,39,1,0,0,0,46,3,1,0,0,0,47,48,5,3,0,0,48,49,5,18,0,0,
        49,53,5,13,0,0,50,52,3,20,10,0,51,50,1,0,0,0,52,55,1,0,0,0,53,51,
        1,0,0,0,53,54,1,0,0,0,54,56,1,0,0,0,55,53,1,0,0,0,56,57,5,14,0,0,
        57,5,1,0,0,0,58,59,5,4,0,0,59,60,5,18,0,0,60,64,5,13,0,0,61,63,3,
        10,5,0,62,61,1,0,0,0,63,66,1,0,0,0,64,62,1,0,0,0,64,65,1,0,0,0,65,
        67,1,0,0,0,66,64,1,0,0,0,67,68,5,14,0,0,68,7,1,0,0,0,69,70,5,5,0,
        0,70,71,5,18,0,0,71,75,5,13,0,0,72,74,3,14,7,0,73,72,1,0,0,0,74,
        77,1,0,0,0,75,73,1,0,0,0,75,76,1,0,0,0,76,78,1,0,0,0,77,75,1,0,0,
        0,78,79,5,14,0,0,79,9,1,0,0,0,80,81,5,18,0,0,81,89,5,13,0,0,82,88,
        3,14,7,0,83,88,3,10,5,0,84,88,3,16,8,0,85,88,3,18,9,0,86,88,3,12,
        6,0,87,82,1,0,0,0,87,83,1,0,0,0,87,84,1,0,0,0,87,85,1,0,0,0,87,86,
        1,0,0,0,88,91,1,0,0,0,89,87,1,0,0,0,89,90,1,0,0,0,90,92,1,0,0,0,
        91,89,1,0,0,0,92,93,5,14,0,0,93,11,1,0,0,0,94,95,5,4,0,0,95,98,5,
        18,0,0,96,97,5,13,0,0,97,99,5,14,0,0,98,96,1,0,0,0,98,99,1,0,0,0,
        99,100,1,0,0,0,100,101,5,17,0,0,101,13,1,0,0,0,102,103,5,18,0,0,
        103,104,7,0,0,0,104,105,3,22,11,0,105,106,5,17,0,0,106,15,1,0,0,
        0,107,108,5,8,0,0,108,109,5,13,0,0,109,110,3,22,11,0,110,111,5,14,
        0,0,111,17,1,0,0,0,112,113,5,6,0,0,113,114,5,11,0,0,114,115,5,19,
        0,0,115,116,5,12,0,0,116,117,5,17,0,0,117,19,1,0,0,0,118,119,7,1,
        0,0,119,21,1,0,0,0,120,128,5,19,0,0,121,128,3,24,12,0,122,124,7,
        2,0,0,123,122,1,0,0,0,124,125,1,0,0,0,125,123,1,0,0,0,125,126,1,
        0,0,0,126,128,1,0,0,0,127,120,1,0,0,0,127,121,1,0,0,0,127,123,1,
        0,0,0,128,23,1,0,0,0,129,130,5,18,0,0,130,131,5,11,0,0,131,134,5,
        18,0,0,132,133,5,16,0,0,133,135,3,22,11,0,134,132,1,0,0,0,134,135,
        1,0,0,0,135,136,1,0,0,0,136,137,5,12,0,0,137,25,1,0,0,0,14,28,30,
        37,43,45,53,64,75,87,89,98,125,127,134
    ]

class CHTLParser ( Parser ):

    grammarFileName = "CHTL.g4"

    atn = ATNDeserializer().deserialize(serializedATN())

    decisionsToDFA = [ DFA(ds, i) for i, ds in enumerate(atn.decisionToState) ]

    sharedContextCache = PredictionContextCache()

    literalNames = [ "<INVALID>", "'[Template]'", "'[Custom]'", "'@Style'",
                     "'@Element'", "'@Var'", "'__style_ref__'", "'style'",
                     "'text'", "<INVALID>", "'.'", "'('", "')'", "'{'",
                     "'}'", "':'", "'='", "';'" ]

    symbolicNames = [ "<INVALID>", "TEMPLATE", "CUSTOM", "AT_STYLE", "AT_ELEMENT",
                      "AT_VAR", "STYLE_REF", "STYLE", "TEXT", "NUMBER",
                      "DOT", "LPAR", "RPAR", "LBRACE", "RBRACE", "COLON",
                      "EQ", "SEMI", "IDENTIFIER", "STRING", "LINE_COMMENT",
                      "BLOCK_COMMENT", "WS" ]

    RULE_document = 0
    RULE_definition = 1
    RULE_styleTemplate = 2
    RULE_elementTemplate = 3
    RULE_varTemplate = 4
    RULE_element = 5
    RULE_elementUsage = 6
    RULE_attribute = 7
    RULE_textNode = 8
    RULE_stylePlaceholder = 9
    RULE_styleContent = 10
    RULE_value = 11
    RULE_varUsage = 12

    ruleNames =  [ "document", "definition", "styleTemplate", "elementTemplate",
                   "varTemplate", "element", "elementUsage", "attribute",
                   "textNode", "stylePlaceholder", "styleContent", "value",
                   "varUsage" ]

    EOF = Token.EOF
    TEMPLATE=1
    CUSTOM=2
    AT_STYLE=3
    AT_ELEMENT=4
    AT_VAR=5
    STYLE_REF=6
    STYLE=7
    TEXT=8
    NUMBER=9
    DOT=10
    LPAR=11
    RPAR=12
    LBRACE=13
    RBRACE=14
    COLON=15
    EQ=16
    SEMI=17
    IDENTIFIER=18
    STRING=19
    LINE_COMMENT=20
    BLOCK_COMMENT=21
    WS=22

    def __init__(self, input:TokenStream, output:TextIO = sys.stdout):
        super().__init__(input, output)
        self.checkVersion("4.13.2")
        self._interp = ParserATNSimulator(self, self.atn, self.decisionsToDFA, self.sharedContextCache)
        self._predicates = None




    class DocumentContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def definition(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.DefinitionContext)
            else:
                return self.getTypedRuleContext(CHTLParser.DefinitionContext,i)


        def element(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.ElementContext)
            else:
                return self.getTypedRuleContext(CHTLParser.ElementContext,i)


        def getRuleIndex(self):
            return CHTLParser.RULE_document

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterDocument" ):
                listener.enterDocument(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitDocument" ):
                listener.exitDocument(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitDocument" ):
                return visitor.visitDocument(self)
            else:
                return visitor.visitChildren(self)




    def document(self):

        localctx = CHTLParser.DocumentContext(self, self._ctx, self.state)
        self.enterRule(localctx, 0, self.RULE_document)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 30
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 262150) != 0):
                self.state = 28
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [1, 2]:
                    self.state = 26
                    self.definition()
                    pass
                elif token in [18]:
                    self.state = 27
                    self.element()
                    pass
                else:
                    raise NoViableAltException(self)

                self.state = 32
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class DefinitionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def TEMPLATE(self):
            return self.getToken(CHTLParser.TEMPLATE, 0)

        def styleTemplate(self):
            return self.getTypedRuleContext(CHTLParser.StyleTemplateContext,0)


        def elementTemplate(self):
            return self.getTypedRuleContext(CHTLParser.ElementTemplateContext,0)


        def varTemplate(self):
            return self.getTypedRuleContext(CHTLParser.VarTemplateContext,0)


        def CUSTOM(self):
            return self.getToken(CHTLParser.CUSTOM, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_definition

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterDefinition" ):
                listener.enterDefinition(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitDefinition" ):
                listener.exitDefinition(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitDefinition" ):
                return visitor.visitDefinition(self)
            else:
                return visitor.visitChildren(self)




    def definition(self):

        localctx = CHTLParser.DefinitionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 2, self.RULE_definition)
        try:
            self.state = 45
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [1]:
                self.enterOuterAlt(localctx, 1)
                self.state = 33
                self.match(CHTLParser.TEMPLATE)
                self.state = 37
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [3]:
                    self.state = 34
                    self.styleTemplate()
                    pass
                elif token in [4]:
                    self.state = 35
                    self.elementTemplate()
                    pass
                elif token in [5]:
                    self.state = 36
                    self.varTemplate()
                    pass
                else:
                    raise NoViableAltException(self)

                pass
            elif token in [2]:
                self.enterOuterAlt(localctx, 2)
                self.state = 39
                self.match(CHTLParser.CUSTOM)
                self.state = 43
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [3]:
                    self.state = 40
                    self.styleTemplate()
                    pass
                elif token in [4]:
                    self.state = 41
                    self.elementTemplate()
                    pass
                elif token in [5]:
                    self.state = 42
                    self.varTemplate()
                    pass
                else:
                    raise NoViableAltException(self)

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


    class StyleTemplateContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def AT_STYLE(self):
            return self.getToken(CHTLParser.AT_STYLE, 0)

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def LBRACE(self):
            return self.getToken(CHTLParser.LBRACE, 0)

        def RBRACE(self):
            return self.getToken(CHTLParser.RBRACE, 0)

        def styleContent(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.StyleContentContext)
            else:
                return self.getTypedRuleContext(CHTLParser.StyleContentContext,i)


        def getRuleIndex(self):
            return CHTLParser.RULE_styleTemplate

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterStyleTemplate" ):
                listener.enterStyleTemplate(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitStyleTemplate" ):
                listener.exitStyleTemplate(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitStyleTemplate" ):
                return visitor.visitStyleTemplate(self)
            else:
                return visitor.visitChildren(self)




    def styleTemplate(self):

        localctx = CHTLParser.StyleTemplateContext(self, self._ctx, self.state)
        self.enterRule(localctx, 4, self.RULE_styleTemplate)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 47
            self.match(CHTLParser.AT_STYLE)
            self.state = 48
            self.match(CHTLParser.IDENTIFIER)
            self.state = 49
            self.match(CHTLParser.LBRACE)
            self.state = 53
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 1025792) != 0):
                self.state = 50
                self.styleContent()
                self.state = 55
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 56
            self.match(CHTLParser.RBRACE)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class ElementTemplateContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def AT_ELEMENT(self):
            return self.getToken(CHTLParser.AT_ELEMENT, 0)

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def LBRACE(self):
            return self.getToken(CHTLParser.LBRACE, 0)

        def RBRACE(self):
            return self.getToken(CHTLParser.RBRACE, 0)

        def element(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.ElementContext)
            else:
                return self.getTypedRuleContext(CHTLParser.ElementContext,i)


        def getRuleIndex(self):
            return CHTLParser.RULE_elementTemplate

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterElementTemplate" ):
                listener.enterElementTemplate(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitElementTemplate" ):
                listener.exitElementTemplate(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitElementTemplate" ):
                return visitor.visitElementTemplate(self)
            else:
                return visitor.visitChildren(self)




    def elementTemplate(self):

        localctx = CHTLParser.ElementTemplateContext(self, self._ctx, self.state)
        self.enterRule(localctx, 6, self.RULE_elementTemplate)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 58
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 59
            self.match(CHTLParser.IDENTIFIER)
            self.state = 60
            self.match(CHTLParser.LBRACE)
            self.state = 64
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==18:
                self.state = 61
                self.element()
                self.state = 66
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 67
            self.match(CHTLParser.RBRACE)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class VarTemplateContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def AT_VAR(self):
            return self.getToken(CHTLParser.AT_VAR, 0)

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def LBRACE(self):
            return self.getToken(CHTLParser.LBRACE, 0)

        def RBRACE(self):
            return self.getToken(CHTLParser.RBRACE, 0)

        def attribute(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.AttributeContext)
            else:
                return self.getTypedRuleContext(CHTLParser.AttributeContext,i)


        def getRuleIndex(self):
            return CHTLParser.RULE_varTemplate

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterVarTemplate" ):
                listener.enterVarTemplate(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitVarTemplate" ):
                listener.exitVarTemplate(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitVarTemplate" ):
                return visitor.visitVarTemplate(self)
            else:
                return visitor.visitChildren(self)




    def varTemplate(self):

        localctx = CHTLParser.VarTemplateContext(self, self._ctx, self.state)
        self.enterRule(localctx, 8, self.RULE_varTemplate)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 69
            self.match(CHTLParser.AT_VAR)
            self.state = 70
            self.match(CHTLParser.IDENTIFIER)
            self.state = 71
            self.match(CHTLParser.LBRACE)
            self.state = 75
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==18:
                self.state = 72
                self.attribute()
                self.state = 77
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 78
            self.match(CHTLParser.RBRACE)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class ElementContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def LBRACE(self):
            return self.getToken(CHTLParser.LBRACE, 0)

        def RBRACE(self):
            return self.getToken(CHTLParser.RBRACE, 0)

        def attribute(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.AttributeContext)
            else:
                return self.getTypedRuleContext(CHTLParser.AttributeContext,i)


        def element(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.ElementContext)
            else:
                return self.getTypedRuleContext(CHTLParser.ElementContext,i)


        def textNode(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.TextNodeContext)
            else:
                return self.getTypedRuleContext(CHTLParser.TextNodeContext,i)


        def stylePlaceholder(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.StylePlaceholderContext)
            else:
                return self.getTypedRuleContext(CHTLParser.StylePlaceholderContext,i)


        def elementUsage(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.ElementUsageContext)
            else:
                return self.getTypedRuleContext(CHTLParser.ElementUsageContext,i)


        def getRuleIndex(self):
            return CHTLParser.RULE_element

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterElement" ):
                listener.enterElement(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitElement" ):
                listener.exitElement(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitElement" ):
                return visitor.visitElement(self)
            else:
                return visitor.visitChildren(self)




    def element(self):

        localctx = CHTLParser.ElementContext(self, self._ctx, self.state)
        self.enterRule(localctx, 10, self.RULE_element)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 80
            self.match(CHTLParser.IDENTIFIER)
            self.state = 81
            self.match(CHTLParser.LBRACE)
            self.state = 89
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 262480) != 0):
                self.state = 87
                self._errHandler.sync(self)
                la_ = self._interp.adaptivePredict(self._input,8,self._ctx)
                if la_ == 1:
                    self.state = 82
                    self.attribute()
                    pass

                elif la_ == 2:
                    self.state = 83
                    self.element()
                    pass

                elif la_ == 3:
                    self.state = 84
                    self.textNode()
                    pass

                elif la_ == 4:
                    self.state = 85
                    self.stylePlaceholder()
                    pass

                elif la_ == 5:
                    self.state = 86
                    self.elementUsage()
                    pass


                self.state = 91
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 92
            self.match(CHTLParser.RBRACE)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class ElementUsageContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def AT_ELEMENT(self):
            return self.getToken(CHTLParser.AT_ELEMENT, 0)

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def LBRACE(self):
            return self.getToken(CHTLParser.LBRACE, 0)

        def RBRACE(self):
            return self.getToken(CHTLParser.RBRACE, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_elementUsage

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterElementUsage" ):
                listener.enterElementUsage(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitElementUsage" ):
                listener.exitElementUsage(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitElementUsage" ):
                return visitor.visitElementUsage(self)
            else:
                return visitor.visitChildren(self)




    def elementUsage(self):

        localctx = CHTLParser.ElementUsageContext(self, self._ctx, self.state)
        self.enterRule(localctx, 12, self.RULE_elementUsage)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 94
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 95
            self.match(CHTLParser.IDENTIFIER)
            self.state = 98
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==13:
                self.state = 96
                self.match(CHTLParser.LBRACE)
                self.state = 97
                self.match(CHTLParser.RBRACE)


            self.state = 100
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class AttributeContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def value(self):
            return self.getTypedRuleContext(CHTLParser.ValueContext,0)


        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def COLON(self):
            return self.getToken(CHTLParser.COLON, 0)

        def EQ(self):
            return self.getToken(CHTLParser.EQ, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_attribute

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterAttribute" ):
                listener.enterAttribute(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitAttribute" ):
                listener.exitAttribute(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitAttribute" ):
                return visitor.visitAttribute(self)
            else:
                return visitor.visitChildren(self)




    def attribute(self):

        localctx = CHTLParser.AttributeContext(self, self._ctx, self.state)
        self.enterRule(localctx, 14, self.RULE_attribute)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 102
            self.match(CHTLParser.IDENTIFIER)
            self.state = 103
            _la = self._input.LA(1)
            if not(_la==15 or _la==16):
                self._errHandler.recoverInline(self)
            else:
                self._errHandler.reportMatch(self)
                self.consume()
            self.state = 104
            self.value()
            self.state = 105
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class TextNodeContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def TEXT(self):
            return self.getToken(CHTLParser.TEXT, 0)

        def LBRACE(self):
            return self.getToken(CHTLParser.LBRACE, 0)

        def value(self):
            return self.getTypedRuleContext(CHTLParser.ValueContext,0)


        def RBRACE(self):
            return self.getToken(CHTLParser.RBRACE, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_textNode

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterTextNode" ):
                listener.enterTextNode(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitTextNode" ):
                listener.exitTextNode(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitTextNode" ):
                return visitor.visitTextNode(self)
            else:
                return visitor.visitChildren(self)




    def textNode(self):

        localctx = CHTLParser.TextNodeContext(self, self._ctx, self.state)
        self.enterRule(localctx, 16, self.RULE_textNode)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 107
            self.match(CHTLParser.TEXT)
            self.state = 108
            self.match(CHTLParser.LBRACE)
            self.state = 109
            self.value()
            self.state = 110
            self.match(CHTLParser.RBRACE)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class StylePlaceholderContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def STYLE_REF(self):
            return self.getToken(CHTLParser.STYLE_REF, 0)

        def LPAR(self):
            return self.getToken(CHTLParser.LPAR, 0)

        def STRING(self):
            return self.getToken(CHTLParser.STRING, 0)

        def RPAR(self):
            return self.getToken(CHTLParser.RPAR, 0)

        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_stylePlaceholder

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterStylePlaceholder" ):
                listener.enterStylePlaceholder(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitStylePlaceholder" ):
                listener.exitStylePlaceholder(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitStylePlaceholder" ):
                return visitor.visitStylePlaceholder(self)
            else:
                return visitor.visitChildren(self)




    def stylePlaceholder(self):

        localctx = CHTLParser.StylePlaceholderContext(self, self._ctx, self.state)
        self.enterRule(localctx, 18, self.RULE_stylePlaceholder)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 112
            self.match(CHTLParser.STYLE_REF)
            self.state = 113
            self.match(CHTLParser.LPAR)
            self.state = 114
            self.match(CHTLParser.STRING)
            self.state = 115
            self.match(CHTLParser.RPAR)
            self.state = 116
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class StyleContentContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def STRING(self):
            return self.getToken(CHTLParser.STRING, 0)

        def COLON(self):
            return self.getToken(CHTLParser.COLON, 0)

        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def TEXT(self):
            return self.getToken(CHTLParser.TEXT, 0)

        def LBRACE(self):
            return self.getToken(CHTLParser.LBRACE, 0)

        def EQ(self):
            return self.getToken(CHTLParser.EQ, 0)

        def NUMBER(self):
            return self.getToken(CHTLParser.NUMBER, 0)

        def DOT(self):
            return self.getToken(CHTLParser.DOT, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_styleContent

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterStyleContent" ):
                listener.enterStyleContent(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitStyleContent" ):
                listener.exitStyleContent(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitStyleContent" ):
                return visitor.visitStyleContent(self)
            else:
                return visitor.visitChildren(self)




    def styleContent(self):

        localctx = CHTLParser.StyleContentContext(self, self._ctx, self.state)
        self.enterRule(localctx, 20, self.RULE_styleContent)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 118
            _la = self._input.LA(1)
            if not((((_la) & ~0x3f) == 0 and ((1 << _la) & 1025792) != 0)):
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


    class ValueContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def STRING(self):
            return self.getToken(CHTLParser.STRING, 0)

        def varUsage(self):
            return self.getTypedRuleContext(CHTLParser.VarUsageContext,0)


        def IDENTIFIER(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLParser.IDENTIFIER)
            else:
                return self.getToken(CHTLParser.IDENTIFIER, i)

        def TEXT(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLParser.TEXT)
            else:
                return self.getToken(CHTLParser.TEXT, i)

        def STYLE(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLParser.STYLE)
            else:
                return self.getToken(CHTLParser.STYLE, i)

        def getRuleIndex(self):
            return CHTLParser.RULE_value

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterValue" ):
                listener.enterValue(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitValue" ):
                listener.exitValue(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitValue" ):
                return visitor.visitValue(self)
            else:
                return visitor.visitChildren(self)




    def value(self):

        localctx = CHTLParser.ValueContext(self, self._ctx, self.state)
        self.enterRule(localctx, 22, self.RULE_value)
        self._la = 0 # Token type
        try:
            self.state = 127
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,12,self._ctx)
            if la_ == 1:
                self.enterOuterAlt(localctx, 1)
                self.state = 120
                self.match(CHTLParser.STRING)
                pass

            elif la_ == 2:
                self.enterOuterAlt(localctx, 2)
                self.state = 121
                self.varUsage()
                pass

            elif la_ == 3:
                self.enterOuterAlt(localctx, 3)
                self.state = 123
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                while True:
                    self.state = 122
                    _la = self._input.LA(1)
                    if not((((_la) & ~0x3f) == 0 and ((1 << _la) & 262528) != 0)):
                        self._errHandler.recoverInline(self)
                    else:
                        self._errHandler.reportMatch(self)
                        self.consume()
                    self.state = 125
                    self._errHandler.sync(self)
                    _la = self._input.LA(1)
                    if not ((((_la) & ~0x3f) == 0 and ((1 << _la) & 262528) != 0)):
                        break

                pass


        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class VarUsageContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def IDENTIFIER(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLParser.IDENTIFIER)
            else:
                return self.getToken(CHTLParser.IDENTIFIER, i)

        def LPAR(self):
            return self.getToken(CHTLParser.LPAR, 0)

        def RPAR(self):
            return self.getToken(CHTLParser.RPAR, 0)

        def EQ(self):
            return self.getToken(CHTLParser.EQ, 0)

        def value(self):
            return self.getTypedRuleContext(CHTLParser.ValueContext,0)


        def getRuleIndex(self):
            return CHTLParser.RULE_varUsage

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterVarUsage" ):
                listener.enterVarUsage(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitVarUsage" ):
                listener.exitVarUsage(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitVarUsage" ):
                return visitor.visitVarUsage(self)
            else:
                return visitor.visitChildren(self)




    def varUsage(self):

        localctx = CHTLParser.VarUsageContext(self, self._ctx, self.state)
        self.enterRule(localctx, 24, self.RULE_varUsage)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 129
            self.match(CHTLParser.IDENTIFIER)
            self.state = 130
            self.match(CHTLParser.LPAR)
            self.state = 131
            self.match(CHTLParser.IDENTIFIER)
            self.state = 134
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==16:
                self.state = 132
                self.match(CHTLParser.EQ)
                self.state = 133
                self.value()


            self.state = 136
            self.match(CHTLParser.RPAR)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx

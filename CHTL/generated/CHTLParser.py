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
        4,1,24,175,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
        6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,
        2,14,7,14,2,15,7,15,2,16,7,16,1,0,1,0,5,0,37,8,0,10,0,12,0,40,9,
        0,1,1,1,1,1,1,1,1,3,1,46,8,1,1,1,1,1,1,1,1,1,3,1,52,8,1,3,1,54,8,
        1,1,2,1,2,1,2,1,2,5,2,60,8,2,10,2,12,2,63,9,2,1,2,1,2,1,3,1,3,1,
        3,1,3,5,3,71,8,3,10,3,12,3,74,9,3,1,3,1,3,1,4,1,4,1,4,1,4,5,4,82,
        8,4,10,4,12,4,85,9,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,5,1,5,5,5,96,
        8,5,10,5,12,5,99,9,5,1,5,1,5,1,6,1,6,1,6,1,6,1,7,1,7,3,7,109,8,7,
        1,8,1,8,1,8,4,8,114,8,8,11,8,12,8,115,1,8,1,8,1,9,1,9,1,9,1,9,1,
        10,1,10,1,10,1,10,1,10,1,11,1,11,1,11,1,11,1,11,1,12,1,12,1,12,1,
        12,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,
        13,150,8,13,1,14,4,14,153,8,14,11,14,12,14,154,1,15,1,15,1,15,1,
        15,1,15,1,15,1,15,3,15,164,8,15,1,16,1,16,1,16,1,16,1,16,3,16,171,
        8,16,1,16,1,16,1,16,0,0,17,0,2,4,6,8,10,12,14,16,18,20,22,24,26,
        28,30,32,0,1,1,0,17,18,191,0,38,1,0,0,0,2,53,1,0,0,0,4,55,1,0,0,
        0,6,66,1,0,0,0,8,77,1,0,0,0,10,88,1,0,0,0,12,102,1,0,0,0,14,108,
        1,0,0,0,16,110,1,0,0,0,18,119,1,0,0,0,20,123,1,0,0,0,22,128,1,0,
        0,0,24,133,1,0,0,0,26,149,1,0,0,0,28,152,1,0,0,0,30,163,1,0,0,0,
        32,165,1,0,0,0,34,37,3,2,1,0,35,37,3,10,5,0,36,34,1,0,0,0,36,35,
        1,0,0,0,37,40,1,0,0,0,38,36,1,0,0,0,38,39,1,0,0,0,39,1,1,0,0,0,40,
        38,1,0,0,0,41,45,5,1,0,0,42,46,3,4,2,0,43,46,3,6,3,0,44,46,3,8,4,
        0,45,42,1,0,0,0,45,43,1,0,0,0,45,44,1,0,0,0,46,54,1,0,0,0,47,51,
        5,2,0,0,48,52,3,4,2,0,49,52,3,6,3,0,50,52,3,8,4,0,51,48,1,0,0,0,
        51,49,1,0,0,0,51,50,1,0,0,0,52,54,1,0,0,0,53,41,1,0,0,0,53,47,1,
        0,0,0,54,3,1,0,0,0,55,56,5,3,0,0,56,57,5,20,0,0,57,61,5,15,0,0,58,
        60,3,26,13,0,59,58,1,0,0,0,60,63,1,0,0,0,61,59,1,0,0,0,61,62,1,0,
        0,0,62,64,1,0,0,0,63,61,1,0,0,0,64,65,5,16,0,0,65,5,1,0,0,0,66,67,
        5,4,0,0,67,68,5,20,0,0,68,72,5,15,0,0,69,71,3,10,5,0,70,69,1,0,0,
        0,71,74,1,0,0,0,72,70,1,0,0,0,72,73,1,0,0,0,73,75,1,0,0,0,74,72,
        1,0,0,0,75,76,5,16,0,0,76,7,1,0,0,0,77,78,5,5,0,0,78,79,5,20,0,0,
        79,83,5,15,0,0,80,82,3,20,10,0,81,80,1,0,0,0,82,85,1,0,0,0,83,81,
        1,0,0,0,83,84,1,0,0,0,84,86,1,0,0,0,85,83,1,0,0,0,86,87,5,16,0,0,
        87,9,1,0,0,0,88,89,5,20,0,0,89,97,5,15,0,0,90,96,3,20,10,0,91,96,
        3,10,5,0,92,96,3,22,11,0,93,96,3,24,12,0,94,96,3,12,6,0,95,90,1,
        0,0,0,95,91,1,0,0,0,95,92,1,0,0,0,95,93,1,0,0,0,95,94,1,0,0,0,96,
        99,1,0,0,0,97,95,1,0,0,0,97,98,1,0,0,0,98,100,1,0,0,0,99,97,1,0,
        0,0,100,101,5,16,0,0,101,11,1,0,0,0,102,103,5,4,0,0,103,104,5,20,
        0,0,104,105,5,19,0,0,105,13,1,0,0,0,106,109,3,16,8,0,107,109,3,18,
        9,0,108,106,1,0,0,0,108,107,1,0,0,0,109,15,1,0,0,0,110,111,5,6,0,
        0,111,113,5,15,0,0,112,114,3,10,5,0,113,112,1,0,0,0,114,115,1,0,
        0,0,115,113,1,0,0,0,115,116,1,0,0,0,116,117,1,0,0,0,117,118,5,16,
        0,0,118,17,1,0,0,0,119,120,5,7,0,0,120,121,5,20,0,0,121,122,5,19,
        0,0,122,19,1,0,0,0,123,124,5,20,0,0,124,125,7,0,0,0,125,126,3,28,
        14,0,126,127,5,19,0,0,127,21,1,0,0,0,128,129,5,10,0,0,129,130,5,
        15,0,0,130,131,3,28,14,0,131,132,5,16,0,0,132,23,1,0,0,0,133,134,
        5,8,0,0,134,135,5,13,0,0,135,136,5,21,0,0,136,137,5,14,0,0,137,138,
        5,19,0,0,138,25,1,0,0,0,139,150,5,20,0,0,140,150,5,21,0,0,141,150,
        5,17,0,0,142,150,5,19,0,0,143,150,5,10,0,0,144,150,5,15,0,0,145,
        150,5,18,0,0,146,150,5,11,0,0,147,150,5,12,0,0,148,150,3,32,16,0,
        149,139,1,0,0,0,149,140,1,0,0,0,149,141,1,0,0,0,149,142,1,0,0,0,
        149,143,1,0,0,0,149,144,1,0,0,0,149,145,1,0,0,0,149,146,1,0,0,0,
        149,147,1,0,0,0,149,148,1,0,0,0,150,27,1,0,0,0,151,153,3,30,15,0,
        152,151,1,0,0,0,153,154,1,0,0,0,154,152,1,0,0,0,154,155,1,0,0,0,
        155,29,1,0,0,0,156,164,5,21,0,0,157,164,3,32,16,0,158,164,5,20,0,
        0,159,164,5,10,0,0,160,164,5,9,0,0,161,164,5,11,0,0,162,164,5,12,
        0,0,163,156,1,0,0,0,163,157,1,0,0,0,163,158,1,0,0,0,163,159,1,0,
        0,0,163,160,1,0,0,0,163,161,1,0,0,0,163,162,1,0,0,0,164,31,1,0,0,
        0,165,166,5,20,0,0,166,167,5,13,0,0,167,170,5,20,0,0,168,169,5,18,
        0,0,169,171,3,28,14,0,170,168,1,0,0,0,170,171,1,0,0,0,171,172,1,
        0,0,0,172,173,5,14,0,0,173,33,1,0,0,0,16,36,38,45,51,53,61,72,83,
        95,97,108,115,149,154,163,170
    ]

class CHTLParser ( Parser ):

    grammarFileName = "CHTL.g4"

    atn = ATNDeserializer().deserialize(serializedATN())

    decisionsToDFA = [ DFA(ds, i) for i, ds in enumerate(atn.decisionToState) ]

    sharedContextCache = PredictionContextCache()

    literalNames = [ "<INVALID>", "'[Template]'", "'[Custom]'", "'@Style'",
                     "'@Element'", "'@Var'", "'insert'", "'delete'", "'__style_ref__'",
                     "'style'", "'text'", "<INVALID>", "'.'", "'('", "')'",
                     "'{'", "'}'", "':'", "'='", "';'" ]

    symbolicNames = [ "<INVALID>", "TEMPLATE", "CUSTOM", "AT_STYLE", "AT_ELEMENT",
                      "AT_VAR", "INSERT", "DELETE", "STYLE_REF", "STYLE",
                      "TEXT", "NUMBER", "DOT", "LPAR", "RPAR", "LBRACE",
                      "RBRACE", "COLON", "EQ", "SEMI", "IDENTIFIER", "STRING",
                      "LINE_COMMENT", "BLOCK_COMMENT", "WS" ]

    RULE_document = 0
    RULE_definition = 1
    RULE_styleTemplate = 2
    RULE_elementTemplate = 3
    RULE_varTemplate = 4
    RULE_element = 5
    RULE_elementUsage = 6
    RULE_specializationBody = 7
    RULE_insertStatement = 8
    RULE_deleteStatement = 9
    RULE_attribute = 10
    RULE_textNode = 11
    RULE_stylePlaceholder = 12
    RULE_styleContent = 13
    RULE_value = 14
    RULE_valuePart = 15
    RULE_varUsage = 16

    ruleNames =  [ "document", "definition", "styleTemplate", "elementTemplate",
                   "varTemplate", "element", "elementUsage", "specializationBody",
                   "insertStatement", "deleteStatement", "attribute", "textNode",
                   "stylePlaceholder", "styleContent", "value", "valuePart",
                   "varUsage" ]

    EOF = Token.EOF
    TEMPLATE=1
    CUSTOM=2
    AT_STYLE=3
    AT_ELEMENT=4
    AT_VAR=5
    INSERT=6
    DELETE=7
    STYLE_REF=8
    STYLE=9
    TEXT=10
    NUMBER=11
    DOT=12
    LPAR=13
    RPAR=14
    LBRACE=15
    RBRACE=16
    COLON=17
    EQ=18
    SEMI=19
    IDENTIFIER=20
    STRING=21
    LINE_COMMENT=22
    BLOCK_COMMENT=23
    WS=24

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
            self.state = 38
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 1048582) != 0):
                self.state = 36
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [1, 2]:
                    self.state = 34
                    self.definition()
                    pass
                elif token in [20]:
                    self.state = 35
                    self.element()
                    pass
                else:
                    raise NoViableAltException(self)

                self.state = 40
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
            self.state = 53
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [1]:
                self.enterOuterAlt(localctx, 1)
                self.state = 41
                self.match(CHTLParser.TEMPLATE)
                self.state = 45
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [3]:
                    self.state = 42
                    self.styleTemplate()
                    pass
                elif token in [4]:
                    self.state = 43
                    self.elementTemplate()
                    pass
                elif token in [5]:
                    self.state = 44
                    self.varTemplate()
                    pass
                else:
                    raise NoViableAltException(self)

                pass
            elif token in [2]:
                self.enterOuterAlt(localctx, 2)
                self.state = 47
                self.match(CHTLParser.CUSTOM)
                self.state = 51
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [3]:
                    self.state = 48
                    self.styleTemplate()
                    pass
                elif token in [4]:
                    self.state = 49
                    self.elementTemplate()
                    pass
                elif token in [5]:
                    self.state = 50
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
            self.state = 55
            self.match(CHTLParser.AT_STYLE)
            self.state = 56
            self.match(CHTLParser.IDENTIFIER)
            self.state = 57
            self.match(CHTLParser.LBRACE)
            self.state = 61
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 4103168) != 0):
                self.state = 58
                self.styleContent()
                self.state = 63
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 64
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
            self.state = 66
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 67
            self.match(CHTLParser.IDENTIFIER)
            self.state = 68
            self.match(CHTLParser.LBRACE)
            self.state = 72
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==20:
                self.state = 69
                self.element()
                self.state = 74
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 75
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
            self.state = 77
            self.match(CHTLParser.AT_VAR)
            self.state = 78
            self.match(CHTLParser.IDENTIFIER)
            self.state = 79
            self.match(CHTLParser.LBRACE)
            self.state = 83
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==20:
                self.state = 80
                self.attribute()
                self.state = 85
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 86
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
            self.state = 88
            self.match(CHTLParser.IDENTIFIER)
            self.state = 89
            self.match(CHTLParser.LBRACE)
            self.state = 97
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 1049872) != 0):
                self.state = 95
                self._errHandler.sync(self)
                la_ = self._interp.adaptivePredict(self._input,8,self._ctx)
                if la_ == 1:
                    self.state = 90
                    self.attribute()
                    pass

                elif la_ == 2:
                    self.state = 91
                    self.element()
                    pass

                elif la_ == 3:
                    self.state = 92
                    self.textNode()
                    pass

                elif la_ == 4:
                    self.state = 93
                    self.stylePlaceholder()
                    pass

                elif la_ == 5:
                    self.state = 94
                    self.elementUsage()
                    pass


                self.state = 99
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 100
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
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 102
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 103
            self.match(CHTLParser.IDENTIFIER)
            self.state = 104
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class SpecializationBodyContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def insertStatement(self):
            return self.getTypedRuleContext(CHTLParser.InsertStatementContext,0)


        def deleteStatement(self):
            return self.getTypedRuleContext(CHTLParser.DeleteStatementContext,0)


        def getRuleIndex(self):
            return CHTLParser.RULE_specializationBody

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterSpecializationBody" ):
                listener.enterSpecializationBody(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitSpecializationBody" ):
                listener.exitSpecializationBody(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitSpecializationBody" ):
                return visitor.visitSpecializationBody(self)
            else:
                return visitor.visitChildren(self)




    def specializationBody(self):

        localctx = CHTLParser.SpecializationBodyContext(self, self._ctx, self.state)
        self.enterRule(localctx, 14, self.RULE_specializationBody)
        try:
            self.state = 108
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [6]:
                self.enterOuterAlt(localctx, 1)
                self.state = 106
                self.insertStatement()
                pass
            elif token in [7]:
                self.enterOuterAlt(localctx, 2)
                self.state = 107
                self.deleteStatement()
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


    class InsertStatementContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def INSERT(self):
            return self.getToken(CHTLParser.INSERT, 0)

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
            return CHTLParser.RULE_insertStatement

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterInsertStatement" ):
                listener.enterInsertStatement(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitInsertStatement" ):
                listener.exitInsertStatement(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitInsertStatement" ):
                return visitor.visitInsertStatement(self)
            else:
                return visitor.visitChildren(self)




    def insertStatement(self):

        localctx = CHTLParser.InsertStatementContext(self, self._ctx, self.state)
        self.enterRule(localctx, 16, self.RULE_insertStatement)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 110
            self.match(CHTLParser.INSERT)
            self.state = 111
            self.match(CHTLParser.LBRACE)
            self.state = 113
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while True:
                self.state = 112
                self.element()
                self.state = 115
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                if not (_la==20):
                    break

            self.state = 117
            self.match(CHTLParser.RBRACE)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class DeleteStatementContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def DELETE(self):
            return self.getToken(CHTLParser.DELETE, 0)

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_deleteStatement

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterDeleteStatement" ):
                listener.enterDeleteStatement(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitDeleteStatement" ):
                listener.exitDeleteStatement(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitDeleteStatement" ):
                return visitor.visitDeleteStatement(self)
            else:
                return visitor.visitChildren(self)




    def deleteStatement(self):

        localctx = CHTLParser.DeleteStatementContext(self, self._ctx, self.state)
        self.enterRule(localctx, 18, self.RULE_deleteStatement)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 119
            self.match(CHTLParser.DELETE)
            self.state = 120
            self.match(CHTLParser.IDENTIFIER)
            self.state = 121
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
        self.enterRule(localctx, 20, self.RULE_attribute)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 123
            self.match(CHTLParser.IDENTIFIER)
            self.state = 124
            _la = self._input.LA(1)
            if not(_la==17 or _la==18):
                self._errHandler.recoverInline(self)
            else:
                self._errHandler.reportMatch(self)
                self.consume()
            self.state = 125
            self.value()
            self.state = 126
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
        self.enterRule(localctx, 22, self.RULE_textNode)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 128
            self.match(CHTLParser.TEXT)
            self.state = 129
            self.match(CHTLParser.LBRACE)
            self.state = 130
            self.value()
            self.state = 131
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
        self.enterRule(localctx, 24, self.RULE_stylePlaceholder)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 133
            self.match(CHTLParser.STYLE_REF)
            self.state = 134
            self.match(CHTLParser.LPAR)
            self.state = 135
            self.match(CHTLParser.STRING)
            self.state = 136
            self.match(CHTLParser.RPAR)
            self.state = 137
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

        def varUsage(self):
            return self.getTypedRuleContext(CHTLParser.VarUsageContext,0)


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
        self.enterRule(localctx, 26, self.RULE_styleContent)
        try:
            self.state = 149
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,12,self._ctx)
            if la_ == 1:
                self.enterOuterAlt(localctx, 1)
                self.state = 139
                self.match(CHTLParser.IDENTIFIER)
                pass

            elif la_ == 2:
                self.enterOuterAlt(localctx, 2)
                self.state = 140
                self.match(CHTLParser.STRING)
                pass

            elif la_ == 3:
                self.enterOuterAlt(localctx, 3)
                self.state = 141
                self.match(CHTLParser.COLON)
                pass

            elif la_ == 4:
                self.enterOuterAlt(localctx, 4)
                self.state = 142
                self.match(CHTLParser.SEMI)
                pass

            elif la_ == 5:
                self.enterOuterAlt(localctx, 5)
                self.state = 143
                self.match(CHTLParser.TEXT)
                pass

            elif la_ == 6:
                self.enterOuterAlt(localctx, 6)
                self.state = 144
                self.match(CHTLParser.LBRACE)
                pass

            elif la_ == 7:
                self.enterOuterAlt(localctx, 7)
                self.state = 145
                self.match(CHTLParser.EQ)
                pass

            elif la_ == 8:
                self.enterOuterAlt(localctx, 8)
                self.state = 146
                self.match(CHTLParser.NUMBER)
                pass

            elif la_ == 9:
                self.enterOuterAlt(localctx, 9)
                self.state = 147
                self.match(CHTLParser.DOT)
                pass

            elif la_ == 10:
                self.enterOuterAlt(localctx, 10)
                self.state = 148
                self.varUsage()
                pass


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

        def valuePart(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.ValuePartContext)
            else:
                return self.getTypedRuleContext(CHTLParser.ValuePartContext,i)


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
        self.enterRule(localctx, 28, self.RULE_value)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 152
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while True:
                self.state = 151
                self.valuePart()
                self.state = 154
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                if not ((((_la) & ~0x3f) == 0 and ((1 << _la) & 3153408) != 0)):
                    break

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class ValuePartContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def STRING(self):
            return self.getToken(CHTLParser.STRING, 0)

        def varUsage(self):
            return self.getTypedRuleContext(CHTLParser.VarUsageContext,0)


        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def TEXT(self):
            return self.getToken(CHTLParser.TEXT, 0)

        def STYLE(self):
            return self.getToken(CHTLParser.STYLE, 0)

        def NUMBER(self):
            return self.getToken(CHTLParser.NUMBER, 0)

        def DOT(self):
            return self.getToken(CHTLParser.DOT, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_valuePart

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterValuePart" ):
                listener.enterValuePart(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitValuePart" ):
                listener.exitValuePart(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitValuePart" ):
                return visitor.visitValuePart(self)
            else:
                return visitor.visitChildren(self)




    def valuePart(self):

        localctx = CHTLParser.ValuePartContext(self, self._ctx, self.state)
        self.enterRule(localctx, 30, self.RULE_valuePart)
        try:
            self.state = 163
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,14,self._ctx)
            if la_ == 1:
                self.enterOuterAlt(localctx, 1)
                self.state = 156
                self.match(CHTLParser.STRING)
                pass

            elif la_ == 2:
                self.enterOuterAlt(localctx, 2)
                self.state = 157
                self.varUsage()
                pass

            elif la_ == 3:
                self.enterOuterAlt(localctx, 3)
                self.state = 158
                self.match(CHTLParser.IDENTIFIER)
                pass

            elif la_ == 4:
                self.enterOuterAlt(localctx, 4)
                self.state = 159
                self.match(CHTLParser.TEXT)
                pass

            elif la_ == 5:
                self.enterOuterAlt(localctx, 5)
                self.state = 160
                self.match(CHTLParser.STYLE)
                pass

            elif la_ == 6:
                self.enterOuterAlt(localctx, 6)
                self.state = 161
                self.match(CHTLParser.NUMBER)
                pass

            elif la_ == 7:
                self.enterOuterAlt(localctx, 7)
                self.state = 162
                self.match(CHTLParser.DOT)
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
        self.enterRule(localctx, 32, self.RULE_varUsage)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 165
            self.match(CHTLParser.IDENTIFIER)
            self.state = 166
            self.match(CHTLParser.LPAR)
            self.state = 167
            self.match(CHTLParser.IDENTIFIER)
            self.state = 170
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==18:
                self.state = 168
                self.match(CHTLParser.EQ)
                self.state = 169
                self.value()


            self.state = 172
            self.match(CHTLParser.RPAR)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx

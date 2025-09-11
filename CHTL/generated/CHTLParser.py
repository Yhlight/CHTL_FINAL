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
        4,1,32,241,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
        6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,
        2,14,7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,
        7,20,2,21,7,21,2,22,7,22,1,0,1,0,1,0,5,0,50,8,0,10,0,12,0,53,9,0,
        1,1,1,1,1,1,3,1,58,8,1,1,2,1,2,1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,1,
        4,1,4,1,4,3,4,73,8,4,1,4,1,4,5,4,77,8,4,10,4,12,4,80,9,4,1,4,1,4,
        1,5,1,5,1,5,1,5,3,5,88,8,5,1,5,1,5,1,5,1,5,3,5,94,8,5,3,5,96,8,5,
        1,6,1,6,1,6,1,6,5,6,102,8,6,10,6,12,6,105,9,6,1,6,1,6,1,7,1,7,1,
        7,1,7,5,7,113,8,7,10,7,12,7,116,9,7,1,7,1,7,1,8,1,8,1,8,1,8,5,8,
        124,8,8,10,8,12,8,127,9,8,1,8,1,8,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,
        9,5,9,139,8,9,10,9,12,9,142,9,9,1,9,1,9,1,10,1,10,1,10,1,10,5,10,
        150,8,10,10,10,12,10,153,9,10,1,10,1,10,3,10,157,8,10,1,11,1,11,
        3,11,161,8,11,1,12,1,12,1,12,4,12,166,8,12,11,12,12,12,167,1,12,
        1,12,1,13,1,13,1,13,1,13,1,14,1,14,1,14,5,14,179,8,14,10,14,12,14,
        182,9,14,1,15,1,15,1,15,1,15,1,15,1,16,1,16,1,16,1,16,1,16,1,17,
        1,17,1,17,1,17,1,17,1,17,1,18,1,18,1,18,1,18,1,18,1,18,1,19,1,19,
        1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,3,19,216,8,19,1,20,4,20,
        219,8,20,11,20,12,20,220,1,21,1,21,1,21,1,21,1,21,1,21,1,21,3,21,
        230,8,21,1,22,1,22,1,22,1,22,1,22,3,22,237,8,22,1,22,1,22,1,22,0,
        0,23,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,
        44,0,2,1,0,28,29,1,0,25,26,260,0,51,1,0,0,0,2,57,1,0,0,0,4,59,1,
        0,0,0,6,65,1,0,0,0,8,69,1,0,0,0,10,95,1,0,0,0,12,97,1,0,0,0,14,108,
        1,0,0,0,16,119,1,0,0,0,18,130,1,0,0,0,20,145,1,0,0,0,22,160,1,0,
        0,0,24,162,1,0,0,0,26,171,1,0,0,0,28,175,1,0,0,0,30,183,1,0,0,0,
        32,188,1,0,0,0,34,193,1,0,0,0,36,199,1,0,0,0,38,215,1,0,0,0,40,218,
        1,0,0,0,42,229,1,0,0,0,44,231,1,0,0,0,46,50,3,2,1,0,47,50,3,10,5,
        0,48,50,3,18,9,0,49,46,1,0,0,0,49,47,1,0,0,0,49,48,1,0,0,0,50,53,
        1,0,0,0,51,49,1,0,0,0,51,52,1,0,0,0,52,1,1,0,0,0,53,51,1,0,0,0,54,
        58,3,4,2,0,55,58,3,6,3,0,56,58,3,8,4,0,57,54,1,0,0,0,57,55,1,0,0,
        0,57,56,1,0,0,0,58,3,1,0,0,0,59,60,5,1,0,0,60,61,5,6,0,0,61,62,5,
        13,0,0,62,63,7,0,0,0,63,64,5,27,0,0,64,5,1,0,0,0,65,66,5,2,0,0,66,
        67,5,28,0,0,67,68,5,27,0,0,68,7,1,0,0,0,69,72,5,3,0,0,70,71,5,7,
        0,0,71,73,5,28,0,0,72,70,1,0,0,0,72,73,1,0,0,0,73,74,1,0,0,0,74,
        78,5,23,0,0,75,77,3,30,15,0,76,75,1,0,0,0,77,80,1,0,0,0,78,76,1,
        0,0,0,78,79,1,0,0,0,79,81,1,0,0,0,80,78,1,0,0,0,81,82,5,24,0,0,82,
        9,1,0,0,0,83,87,5,4,0,0,84,88,3,12,6,0,85,88,3,14,7,0,86,88,3,16,
        8,0,87,84,1,0,0,0,87,85,1,0,0,0,87,86,1,0,0,0,88,96,1,0,0,0,89,93,
        5,5,0,0,90,94,3,12,6,0,91,94,3,14,7,0,92,94,3,16,8,0,93,90,1,0,0,
        0,93,91,1,0,0,0,93,92,1,0,0,0,94,96,1,0,0,0,95,83,1,0,0,0,95,89,
        1,0,0,0,96,11,1,0,0,0,97,98,5,8,0,0,98,99,5,28,0,0,99,103,5,23,0,
        0,100,102,3,38,19,0,101,100,1,0,0,0,102,105,1,0,0,0,103,101,1,0,
        0,0,103,104,1,0,0,0,104,106,1,0,0,0,105,103,1,0,0,0,106,107,5,24,
        0,0,107,13,1,0,0,0,108,109,5,9,0,0,109,110,5,28,0,0,110,114,5,23,
        0,0,111,113,3,18,9,0,112,111,1,0,0,0,113,116,1,0,0,0,114,112,1,0,
        0,0,114,115,1,0,0,0,115,117,1,0,0,0,116,114,1,0,0,0,117,118,5,24,
        0,0,118,15,1,0,0,0,119,120,5,10,0,0,120,121,5,28,0,0,121,125,5,23,
        0,0,122,124,3,30,15,0,123,122,1,0,0,0,124,127,1,0,0,0,125,123,1,
        0,0,0,125,126,1,0,0,0,126,128,1,0,0,0,127,125,1,0,0,0,128,129,5,
        24,0,0,129,17,1,0,0,0,130,131,5,28,0,0,131,140,5,23,0,0,132,139,
        3,30,15,0,133,139,3,18,9,0,134,139,3,32,16,0,135,139,3,34,17,0,136,
        139,3,36,18,0,137,139,3,20,10,0,138,132,1,0,0,0,138,133,1,0,0,0,
        138,134,1,0,0,0,138,135,1,0,0,0,138,136,1,0,0,0,138,137,1,0,0,0,
        139,142,1,0,0,0,140,138,1,0,0,0,140,141,1,0,0,0,141,143,1,0,0,0,
        142,140,1,0,0,0,143,144,5,24,0,0,144,19,1,0,0,0,145,146,5,9,0,0,
        146,156,5,28,0,0,147,151,5,23,0,0,148,150,3,22,11,0,149,148,1,0,
        0,0,150,153,1,0,0,0,151,149,1,0,0,0,151,152,1,0,0,0,152,154,1,0,
        0,0,153,151,1,0,0,0,154,157,5,24,0,0,155,157,5,27,0,0,156,147,1,
        0,0,0,156,155,1,0,0,0,157,21,1,0,0,0,158,161,3,24,12,0,159,161,3,
        26,13,0,160,158,1,0,0,0,160,159,1,0,0,0,161,23,1,0,0,0,162,163,5,
        11,0,0,163,165,5,23,0,0,164,166,3,18,9,0,165,164,1,0,0,0,166,167,
        1,0,0,0,167,165,1,0,0,0,167,168,1,0,0,0,168,169,1,0,0,0,169,170,
        5,24,0,0,170,25,1,0,0,0,171,172,5,12,0,0,172,173,5,28,0,0,173,174,
        5,27,0,0,174,27,1,0,0,0,175,180,5,28,0,0,176,177,5,20,0,0,177,179,
        5,28,0,0,178,176,1,0,0,0,179,182,1,0,0,0,180,178,1,0,0,0,180,181,
        1,0,0,0,181,29,1,0,0,0,182,180,1,0,0,0,183,184,5,28,0,0,184,185,
        7,1,0,0,185,186,3,40,20,0,186,187,5,27,0,0,187,31,1,0,0,0,188,189,
        5,18,0,0,189,190,5,23,0,0,190,191,3,40,20,0,191,192,5,24,0,0,192,
        33,1,0,0,0,193,194,5,15,0,0,194,195,5,21,0,0,195,196,5,29,0,0,196,
        197,5,22,0,0,197,198,5,27,0,0,198,35,1,0,0,0,199,200,5,16,0,0,200,
        201,5,21,0,0,201,202,5,29,0,0,202,203,5,22,0,0,203,204,5,27,0,0,
        204,37,1,0,0,0,205,216,5,28,0,0,206,216,5,29,0,0,207,216,5,25,0,
        0,208,216,5,27,0,0,209,216,5,18,0,0,210,216,5,23,0,0,211,216,5,26,
        0,0,212,216,5,19,0,0,213,216,5,20,0,0,214,216,3,44,22,0,215,205,
        1,0,0,0,215,206,1,0,0,0,215,207,1,0,0,0,215,208,1,0,0,0,215,209,
        1,0,0,0,215,210,1,0,0,0,215,211,1,0,0,0,215,212,1,0,0,0,215,213,
        1,0,0,0,215,214,1,0,0,0,216,39,1,0,0,0,217,219,3,42,21,0,218,217,
        1,0,0,0,219,220,1,0,0,0,220,218,1,0,0,0,220,221,1,0,0,0,221,41,1,
        0,0,0,222,230,5,29,0,0,223,230,3,44,22,0,224,230,5,28,0,0,225,230,
        5,18,0,0,226,230,5,17,0,0,227,230,5,19,0,0,228,230,5,20,0,0,229,
        222,1,0,0,0,229,223,1,0,0,0,229,224,1,0,0,0,229,225,1,0,0,0,229,
        226,1,0,0,0,229,227,1,0,0,0,229,228,1,0,0,0,230,43,1,0,0,0,231,232,
        5,28,0,0,232,233,5,21,0,0,233,236,5,28,0,0,234,235,5,26,0,0,235,
        237,3,40,20,0,236,234,1,0,0,0,236,237,1,0,0,0,237,238,1,0,0,0,238,
        239,5,22,0,0,239,45,1,0,0,0,22,49,51,57,72,78,87,93,95,103,114,125,
        138,140,151,156,160,167,180,215,220,229,236
    ]

class CHTLParser ( Parser ):

    grammarFileName = "CHTL.g4"

    atn = ATNDeserializer().deserialize(serializedATN())

    decisionsToDFA = [ DFA(ds, i) for i, ds in enumerate(atn.decisionToState) ]

    sharedContextCache = PredictionContextCache()

    literalNames = [ "<INVALID>", "'[Import]'", "'[Namespace]'", "'[Configuration]'",
                     "'[Template]'", "'[Custom]'", "'@Chtl'", "'@Config'",
                     "'@Style'", "'@Element'", "'@Var'", "'insert'", "'delete'",
                     "'from'", "'as'", "'__style_ref__'", "'__script_ref__'",
                     "'style'", "'text'", "<INVALID>", "'.'", "'('", "')'",
                     "'{'", "'}'", "':'", "'='", "';'" ]

    symbolicNames = [ "<INVALID>", "IMPORT", "NAMESPACE", "CONFIGURATION",
                      "TEMPLATE", "CUSTOM", "AT_CHTL", "AT_CONFIG", "AT_STYLE",
                      "AT_ELEMENT", "AT_VAR", "INSERT", "DELETE", "FROM",
                      "AS", "STYLE_REF", "SCRIPT_REF", "STYLE", "TEXT",
                      "NUMBER", "DOT", "LPAR", "RPAR", "LBRACE", "RBRACE",
                      "COLON", "EQ", "SEMI", "IDENTIFIER", "STRING", "LINE_COMMENT",
                      "BLOCK_COMMENT", "WS" ]

    RULE_document = 0
    RULE_directive = 1
    RULE_importStatement = 2
    RULE_namespaceStatement = 3
    RULE_configurationStatement = 4
    RULE_definition = 5
    RULE_styleTemplate = 6
    RULE_elementTemplate = 7
    RULE_varTemplate = 8
    RULE_element = 9
    RULE_elementUsage = 10
    RULE_specializationBody = 11
    RULE_insertStatement = 12
    RULE_deleteStatement = 13
    RULE_namespacePath = 14
    RULE_attribute = 15
    RULE_textNode = 16
    RULE_stylePlaceholder = 17
    RULE_scriptPlaceholder = 18
    RULE_styleContent = 19
    RULE_value = 20
    RULE_valuePart = 21
    RULE_varUsage = 22

    ruleNames =  [ "document", "directive", "importStatement", "namespaceStatement",
                   "configurationStatement", "definition", "styleTemplate",
                   "elementTemplate", "varTemplate", "element", "elementUsage",
                   "specializationBody", "insertStatement", "deleteStatement",
                   "namespacePath", "attribute", "textNode", "stylePlaceholder",
                   "scriptPlaceholder", "styleContent", "value", "valuePart",
                   "varUsage" ]

    EOF = Token.EOF
    IMPORT=1
    NAMESPACE=2
    CONFIGURATION=3
    TEMPLATE=4
    CUSTOM=5
    AT_CHTL=6
    AT_CONFIG=7
    AT_STYLE=8
    AT_ELEMENT=9
    AT_VAR=10
    INSERT=11
    DELETE=12
    FROM=13
    AS=14
    STYLE_REF=15
    SCRIPT_REF=16
    STYLE=17
    TEXT=18
    NUMBER=19
    DOT=20
    LPAR=21
    RPAR=22
    LBRACE=23
    RBRACE=24
    COLON=25
    EQ=26
    SEMI=27
    IDENTIFIER=28
    STRING=29
    LINE_COMMENT=30
    BLOCK_COMMENT=31
    WS=32

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

        def directive(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.DirectiveContext)
            else:
                return self.getTypedRuleContext(CHTLParser.DirectiveContext,i)


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
            self.state = 51
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 268435518) != 0):
                self.state = 49
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [1, 2, 3]:
                    self.state = 46
                    self.directive()
                    pass
                elif token in [4, 5]:
                    self.state = 47
                    self.definition()
                    pass
                elif token in [28]:
                    self.state = 48
                    self.element()
                    pass
                else:
                    raise NoViableAltException(self)

                self.state = 53
                self._errHandler.sync(self)
                _la = self._input.LA(1)

        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class DirectiveContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def importStatement(self):
            return self.getTypedRuleContext(CHTLParser.ImportStatementContext,0)


        def namespaceStatement(self):
            return self.getTypedRuleContext(CHTLParser.NamespaceStatementContext,0)


        def configurationStatement(self):
            return self.getTypedRuleContext(CHTLParser.ConfigurationStatementContext,0)


        def getRuleIndex(self):
            return CHTLParser.RULE_directive

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterDirective" ):
                listener.enterDirective(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitDirective" ):
                listener.exitDirective(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitDirective" ):
                return visitor.visitDirective(self)
            else:
                return visitor.visitChildren(self)




    def directive(self):

        localctx = CHTLParser.DirectiveContext(self, self._ctx, self.state)
        self.enterRule(localctx, 2, self.RULE_directive)
        try:
            self.state = 57
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [1]:
                self.enterOuterAlt(localctx, 1)
                self.state = 54
                self.importStatement()
                pass
            elif token in [2]:
                self.enterOuterAlt(localctx, 2)
                self.state = 55
                self.namespaceStatement()
                pass
            elif token in [3]:
                self.enterOuterAlt(localctx, 3)
                self.state = 56
                self.configurationStatement()
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


    class ImportStatementContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def IMPORT(self):
            return self.getToken(CHTLParser.IMPORT, 0)

        def AT_CHTL(self):
            return self.getToken(CHTLParser.AT_CHTL, 0)

        def FROM(self):
            return self.getToken(CHTLParser.FROM, 0)

        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def STRING(self):
            return self.getToken(CHTLParser.STRING, 0)

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_importStatement

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterImportStatement" ):
                listener.enterImportStatement(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitImportStatement" ):
                listener.exitImportStatement(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitImportStatement" ):
                return visitor.visitImportStatement(self)
            else:
                return visitor.visitChildren(self)




    def importStatement(self):

        localctx = CHTLParser.ImportStatementContext(self, self._ctx, self.state)
        self.enterRule(localctx, 4, self.RULE_importStatement)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 59
            self.match(CHTLParser.IMPORT)
            self.state = 60
            self.match(CHTLParser.AT_CHTL)
            self.state = 61
            self.match(CHTLParser.FROM)
            self.state = 62
            _la = self._input.LA(1)
            if not(_la==28 or _la==29):
                self._errHandler.recoverInline(self)
            else:
                self._errHandler.reportMatch(self)
                self.consume()
            self.state = 63
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class NamespaceStatementContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def NAMESPACE(self):
            return self.getToken(CHTLParser.NAMESPACE, 0)

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_namespaceStatement

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterNamespaceStatement" ):
                listener.enterNamespaceStatement(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitNamespaceStatement" ):
                listener.exitNamespaceStatement(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitNamespaceStatement" ):
                return visitor.visitNamespaceStatement(self)
            else:
                return visitor.visitChildren(self)




    def namespaceStatement(self):

        localctx = CHTLParser.NamespaceStatementContext(self, self._ctx, self.state)
        self.enterRule(localctx, 6, self.RULE_namespaceStatement)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 65
            self.match(CHTLParser.NAMESPACE)
            self.state = 66
            self.match(CHTLParser.IDENTIFIER)
            self.state = 67
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class ConfigurationStatementContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def CONFIGURATION(self):
            return self.getToken(CHTLParser.CONFIGURATION, 0)

        def LBRACE(self):
            return self.getToken(CHTLParser.LBRACE, 0)

        def RBRACE(self):
            return self.getToken(CHTLParser.RBRACE, 0)

        def AT_CONFIG(self):
            return self.getToken(CHTLParser.AT_CONFIG, 0)

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def attribute(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.AttributeContext)
            else:
                return self.getTypedRuleContext(CHTLParser.AttributeContext,i)


        def getRuleIndex(self):
            return CHTLParser.RULE_configurationStatement

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterConfigurationStatement" ):
                listener.enterConfigurationStatement(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitConfigurationStatement" ):
                listener.exitConfigurationStatement(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitConfigurationStatement" ):
                return visitor.visitConfigurationStatement(self)
            else:
                return visitor.visitChildren(self)




    def configurationStatement(self):

        localctx = CHTLParser.ConfigurationStatementContext(self, self._ctx, self.state)
        self.enterRule(localctx, 8, self.RULE_configurationStatement)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 69
            self.match(CHTLParser.CONFIGURATION)
            self.state = 72
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==7:
                self.state = 70
                self.match(CHTLParser.AT_CONFIG)
                self.state = 71
                self.match(CHTLParser.IDENTIFIER)


            self.state = 74
            self.match(CHTLParser.LBRACE)
            self.state = 78
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==28:
                self.state = 75
                self.attribute()
                self.state = 80
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 81
            self.match(CHTLParser.RBRACE)
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
        self.enterRule(localctx, 10, self.RULE_definition)
        try:
            self.state = 95
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [4]:
                self.enterOuterAlt(localctx, 1)
                self.state = 83
                self.match(CHTLParser.TEMPLATE)
                self.state = 87
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [8]:
                    self.state = 84
                    self.styleTemplate()
                    pass
                elif token in [9]:
                    self.state = 85
                    self.elementTemplate()
                    pass
                elif token in [10]:
                    self.state = 86
                    self.varTemplate()
                    pass
                else:
                    raise NoViableAltException(self)

                pass
            elif token in [5]:
                self.enterOuterAlt(localctx, 2)
                self.state = 89
                self.match(CHTLParser.CUSTOM)
                self.state = 93
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [8]:
                    self.state = 90
                    self.styleTemplate()
                    pass
                elif token in [9]:
                    self.state = 91
                    self.elementTemplate()
                    pass
                elif token in [10]:
                    self.state = 92
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
        self.enterRule(localctx, 12, self.RULE_styleTemplate)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 97
            self.match(CHTLParser.AT_STYLE)
            self.state = 98
            self.match(CHTLParser.IDENTIFIER)
            self.state = 99
            self.match(CHTLParser.LBRACE)
            self.state = 103
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 1050411008) != 0):
                self.state = 100
                self.styleContent()
                self.state = 105
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 106
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
        self.enterRule(localctx, 14, self.RULE_elementTemplate)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 108
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 109
            self.match(CHTLParser.IDENTIFIER)
            self.state = 110
            self.match(CHTLParser.LBRACE)
            self.state = 114
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==28:
                self.state = 111
                self.element()
                self.state = 116
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 117
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
        self.enterRule(localctx, 16, self.RULE_varTemplate)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 119
            self.match(CHTLParser.AT_VAR)
            self.state = 120
            self.match(CHTLParser.IDENTIFIER)
            self.state = 121
            self.match(CHTLParser.LBRACE)
            self.state = 125
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==28:
                self.state = 122
                self.attribute()
                self.state = 127
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 128
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


        def scriptPlaceholder(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.ScriptPlaceholderContext)
            else:
                return self.getTypedRuleContext(CHTLParser.ScriptPlaceholderContext,i)


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
        self.enterRule(localctx, 18, self.RULE_element)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 130
            self.match(CHTLParser.IDENTIFIER)
            self.state = 131
            self.match(CHTLParser.LBRACE)
            self.state = 140
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 268796416) != 0):
                self.state = 138
                self._errHandler.sync(self)
                la_ = self._interp.adaptivePredict(self._input,11,self._ctx)
                if la_ == 1:
                    self.state = 132
                    self.attribute()
                    pass

                elif la_ == 2:
                    self.state = 133
                    self.element()
                    pass

                elif la_ == 3:
                    self.state = 134
                    self.textNode()
                    pass

                elif la_ == 4:
                    self.state = 135
                    self.stylePlaceholder()
                    pass

                elif la_ == 5:
                    self.state = 136
                    self.scriptPlaceholder()
                    pass

                elif la_ == 6:
                    self.state = 137
                    self.elementUsage()
                    pass


                self.state = 142
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 143
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

        def specializationBody(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.SpecializationBodyContext)
            else:
                return self.getTypedRuleContext(CHTLParser.SpecializationBodyContext,i)


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
        self.enterRule(localctx, 20, self.RULE_elementUsage)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 145
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 146
            self.match(CHTLParser.IDENTIFIER)
            self.state = 156
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [23]:
                self.state = 147
                self.match(CHTLParser.LBRACE)
                self.state = 151
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                while _la==11 or _la==12:
                    self.state = 148
                    self.specializationBody()
                    self.state = 153
                    self._errHandler.sync(self)
                    _la = self._input.LA(1)

                self.state = 154
                self.match(CHTLParser.RBRACE)
                pass
            elif token in [27]:
                self.state = 155
                self.match(CHTLParser.SEMI)
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
        self.enterRule(localctx, 22, self.RULE_specializationBody)
        try:
            self.state = 160
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [11]:
                self.enterOuterAlt(localctx, 1)
                self.state = 158
                self.insertStatement()
                pass
            elif token in [12]:
                self.enterOuterAlt(localctx, 2)
                self.state = 159
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
        self.enterRule(localctx, 24, self.RULE_insertStatement)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 162
            self.match(CHTLParser.INSERT)
            self.state = 163
            self.match(CHTLParser.LBRACE)
            self.state = 165
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while True:
                self.state = 164
                self.element()
                self.state = 167
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                if not (_la==28):
                    break

            self.state = 169
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
        self.enterRule(localctx, 26, self.RULE_deleteStatement)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 171
            self.match(CHTLParser.DELETE)
            self.state = 172
            self.match(CHTLParser.IDENTIFIER)
            self.state = 173
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class NamespacePathContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def IDENTIFIER(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLParser.IDENTIFIER)
            else:
                return self.getToken(CHTLParser.IDENTIFIER, i)

        def DOT(self, i:int=None):
            if i is None:
                return self.getTokens(CHTLParser.DOT)
            else:
                return self.getToken(CHTLParser.DOT, i)

        def getRuleIndex(self):
            return CHTLParser.RULE_namespacePath

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterNamespacePath" ):
                listener.enterNamespacePath(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitNamespacePath" ):
                listener.exitNamespacePath(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitNamespacePath" ):
                return visitor.visitNamespacePath(self)
            else:
                return visitor.visitChildren(self)




    def namespacePath(self):

        localctx = CHTLParser.NamespacePathContext(self, self._ctx, self.state)
        self.enterRule(localctx, 28, self.RULE_namespacePath)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 175
            self.match(CHTLParser.IDENTIFIER)
            self.state = 180
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==20:
                self.state = 176
                self.match(CHTLParser.DOT)
                self.state = 177
                self.match(CHTLParser.IDENTIFIER)
                self.state = 182
                self._errHandler.sync(self)
                _la = self._input.LA(1)

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
        self.enterRule(localctx, 30, self.RULE_attribute)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 183
            self.match(CHTLParser.IDENTIFIER)
            self.state = 184
            _la = self._input.LA(1)
            if not(_la==25 or _la==26):
                self._errHandler.recoverInline(self)
            else:
                self._errHandler.reportMatch(self)
                self.consume()
            self.state = 185
            self.value()
            self.state = 186
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
        self.enterRule(localctx, 32, self.RULE_textNode)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 188
            self.match(CHTLParser.TEXT)
            self.state = 189
            self.match(CHTLParser.LBRACE)
            self.state = 190
            self.value()
            self.state = 191
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
        self.enterRule(localctx, 34, self.RULE_stylePlaceholder)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 193
            self.match(CHTLParser.STYLE_REF)
            self.state = 194
            self.match(CHTLParser.LPAR)
            self.state = 195
            self.match(CHTLParser.STRING)
            self.state = 196
            self.match(CHTLParser.RPAR)
            self.state = 197
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class ScriptPlaceholderContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def SCRIPT_REF(self):
            return self.getToken(CHTLParser.SCRIPT_REF, 0)

        def LPAR(self):
            return self.getToken(CHTLParser.LPAR, 0)

        def STRING(self):
            return self.getToken(CHTLParser.STRING, 0)

        def RPAR(self):
            return self.getToken(CHTLParser.RPAR, 0)

        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_scriptPlaceholder

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterScriptPlaceholder" ):
                listener.enterScriptPlaceholder(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitScriptPlaceholder" ):
                listener.exitScriptPlaceholder(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitScriptPlaceholder" ):
                return visitor.visitScriptPlaceholder(self)
            else:
                return visitor.visitChildren(self)




    def scriptPlaceholder(self):

        localctx = CHTLParser.ScriptPlaceholderContext(self, self._ctx, self.state)
        self.enterRule(localctx, 36, self.RULE_scriptPlaceholder)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 199
            self.match(CHTLParser.SCRIPT_REF)
            self.state = 200
            self.match(CHTLParser.LPAR)
            self.state = 201
            self.match(CHTLParser.STRING)
            self.state = 202
            self.match(CHTLParser.RPAR)
            self.state = 203
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
        self.enterRule(localctx, 38, self.RULE_styleContent)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 215
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,18,self._ctx)
            if la_ == 1:
                self.state = 205
                self.match(CHTLParser.IDENTIFIER)
                pass

            elif la_ == 2:
                self.state = 206
                self.match(CHTLParser.STRING)
                pass

            elif la_ == 3:
                self.state = 207
                self.match(CHTLParser.COLON)
                pass

            elif la_ == 4:
                self.state = 208
                self.match(CHTLParser.SEMI)
                pass

            elif la_ == 5:
                self.state = 209
                self.match(CHTLParser.TEXT)
                pass

            elif la_ == 6:
                self.state = 210
                self.match(CHTLParser.LBRACE)
                pass

            elif la_ == 7:
                self.state = 211
                self.match(CHTLParser.EQ)
                pass

            elif la_ == 8:
                self.state = 212
                self.match(CHTLParser.NUMBER)
                pass

            elif la_ == 9:
                self.state = 213
                self.match(CHTLParser.DOT)
                pass

            elif la_ == 10:
                self.state = 214
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
        self.enterRule(localctx, 40, self.RULE_value)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 218
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while True:
                self.state = 217
                self.valuePart()
                self.state = 220
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                if not ((((_la) & ~0x3f) == 0 and ((1 << _la) & 807272448) != 0)):
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
        self.enterRule(localctx, 42, self.RULE_valuePart)
        try:
            self.state = 229
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,20,self._ctx)
            if la_ == 1:
                self.enterOuterAlt(localctx, 1)
                self.state = 222
                self.match(CHTLParser.STRING)
                pass

            elif la_ == 2:
                self.enterOuterAlt(localctx, 2)
                self.state = 223
                self.varUsage()
                pass

            elif la_ == 3:
                self.enterOuterAlt(localctx, 3)
                self.state = 224
                self.match(CHTLParser.IDENTIFIER)
                pass

            elif la_ == 4:
                self.enterOuterAlt(localctx, 4)
                self.state = 225
                self.match(CHTLParser.TEXT)
                pass

            elif la_ == 5:
                self.enterOuterAlt(localctx, 5)
                self.state = 226
                self.match(CHTLParser.STYLE)
                pass

            elif la_ == 6:
                self.enterOuterAlt(localctx, 6)
                self.state = 227
                self.match(CHTLParser.NUMBER)
                pass

            elif la_ == 7:
                self.enterOuterAlt(localctx, 7)
                self.state = 228
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
        self.enterRule(localctx, 44, self.RULE_varUsage)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 231
            self.match(CHTLParser.IDENTIFIER)
            self.state = 232
            self.match(CHTLParser.LPAR)
            self.state = 233
            self.match(CHTLParser.IDENTIFIER)
            self.state = 236
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==26:
                self.state = 234
                self.match(CHTLParser.EQ)
                self.state = 235
                self.value()


            self.state = 238
            self.match(CHTLParser.RPAR)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx

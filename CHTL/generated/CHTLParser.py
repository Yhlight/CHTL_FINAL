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
        4,1,40,262,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
        6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,
        2,14,7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,
        7,20,2,21,7,21,2,22,7,22,2,23,7,23,2,24,7,24,1,0,1,0,1,0,5,0,54,
        8,0,10,0,12,0,57,9,0,1,1,1,1,1,1,3,1,62,8,1,1,2,1,2,1,2,1,2,1,2,
        1,2,1,3,1,3,1,3,1,3,1,4,1,4,1,4,3,4,77,8,4,1,4,1,4,5,4,81,8,4,10,
        4,12,4,84,9,4,1,4,1,4,1,5,1,5,1,5,1,5,3,5,92,8,5,1,5,1,5,1,5,1,5,
        3,5,98,8,5,3,5,100,8,5,1,6,1,6,1,6,1,6,5,6,106,8,6,10,6,12,6,109,
        9,6,1,6,1,6,1,7,1,7,1,7,1,7,5,7,117,8,7,10,7,12,7,120,9,7,1,7,1,
        7,1,8,1,8,1,8,1,8,5,8,128,8,8,10,8,12,8,131,9,8,1,8,1,8,1,9,1,9,
        1,9,1,9,1,9,1,9,1,9,1,9,5,9,143,8,9,10,9,12,9,146,9,9,1,9,1,9,1,
        10,1,10,1,10,1,10,5,10,154,8,10,10,10,12,10,157,9,10,1,10,1,10,3,
        10,161,8,10,1,11,1,11,3,11,165,8,11,1,12,1,12,3,12,169,8,12,1,12,
        1,12,4,12,173,8,12,11,12,12,12,174,1,12,1,12,1,13,1,13,1,13,1,13,
        1,14,1,14,1,14,1,14,3,14,187,8,14,1,15,1,15,1,15,1,15,3,15,193,8,
        15,1,16,1,16,1,16,5,16,198,8,16,10,16,12,16,201,9,16,1,17,1,17,1,
        17,3,17,206,8,17,1,17,1,17,1,18,1,18,1,18,1,18,1,18,1,19,1,19,1,
        19,1,19,1,19,1,19,1,20,1,20,1,20,1,20,1,20,1,20,1,21,1,21,1,21,1,
        21,1,21,1,21,1,21,1,21,1,21,1,21,3,21,237,8,21,1,22,4,22,240,8,22,
        11,22,12,22,241,1,23,1,23,1,23,1,23,1,23,1,23,1,23,3,23,251,8,23,
        1,24,1,24,1,24,1,24,1,24,3,24,258,8,24,1,24,1,24,1,24,0,0,25,0,2,
        4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,
        0,4,1,0,36,37,1,0,13,15,1,0,17,18,1,0,33,34,283,0,55,1,0,0,0,2,61,
        1,0,0,0,4,63,1,0,0,0,6,69,1,0,0,0,8,73,1,0,0,0,10,99,1,0,0,0,12,
        101,1,0,0,0,14,112,1,0,0,0,16,123,1,0,0,0,18,134,1,0,0,0,20,149,
        1,0,0,0,22,164,1,0,0,0,24,166,1,0,0,0,26,178,1,0,0,0,28,186,1,0,
        0,0,30,188,1,0,0,0,32,194,1,0,0,0,34,202,1,0,0,0,36,209,1,0,0,0,
        38,214,1,0,0,0,40,220,1,0,0,0,42,236,1,0,0,0,44,239,1,0,0,0,46,250,
        1,0,0,0,48,252,1,0,0,0,50,54,3,2,1,0,51,54,3,10,5,0,52,54,3,18,9,
        0,53,50,1,0,0,0,53,51,1,0,0,0,53,52,1,0,0,0,54,57,1,0,0,0,55,53,
        1,0,0,0,55,56,1,0,0,0,56,1,1,0,0,0,57,55,1,0,0,0,58,62,3,4,2,0,59,
        62,3,6,3,0,60,62,3,8,4,0,61,58,1,0,0,0,61,59,1,0,0,0,61,60,1,0,0,
        0,62,3,1,0,0,0,63,64,5,1,0,0,64,65,5,6,0,0,65,66,5,19,0,0,66,67,
        7,0,0,0,67,68,5,35,0,0,68,5,1,0,0,0,69,70,5,2,0,0,70,71,5,36,0,0,
        71,72,5,35,0,0,72,7,1,0,0,0,73,76,5,3,0,0,74,75,5,7,0,0,75,77,5,
        36,0,0,76,74,1,0,0,0,76,77,1,0,0,0,77,78,1,0,0,0,78,82,5,29,0,0,
        79,81,3,34,17,0,80,79,1,0,0,0,81,84,1,0,0,0,82,80,1,0,0,0,82,83,
        1,0,0,0,83,85,1,0,0,0,84,82,1,0,0,0,85,86,5,30,0,0,86,9,1,0,0,0,
        87,91,5,4,0,0,88,92,3,12,6,0,89,92,3,14,7,0,90,92,3,16,8,0,91,88,
        1,0,0,0,91,89,1,0,0,0,91,90,1,0,0,0,92,100,1,0,0,0,93,97,5,5,0,0,
        94,98,3,12,6,0,95,98,3,14,7,0,96,98,3,16,8,0,97,94,1,0,0,0,97,95,
        1,0,0,0,97,96,1,0,0,0,98,100,1,0,0,0,99,87,1,0,0,0,99,93,1,0,0,0,
        100,11,1,0,0,0,101,102,5,8,0,0,102,103,5,36,0,0,103,107,5,29,0,0,
        104,106,3,34,17,0,105,104,1,0,0,0,106,109,1,0,0,0,107,105,1,0,0,
        0,107,108,1,0,0,0,108,110,1,0,0,0,109,107,1,0,0,0,110,111,5,30,0,
        0,111,13,1,0,0,0,112,113,5,9,0,0,113,114,5,36,0,0,114,118,5,29,0,
        0,115,117,3,18,9,0,116,115,1,0,0,0,117,120,1,0,0,0,118,116,1,0,0,
        0,118,119,1,0,0,0,119,121,1,0,0,0,120,118,1,0,0,0,121,122,5,30,0,
        0,122,15,1,0,0,0,123,124,5,10,0,0,124,125,5,36,0,0,125,129,5,29,
        0,0,126,128,3,34,17,0,127,126,1,0,0,0,128,131,1,0,0,0,129,127,1,
        0,0,0,129,130,1,0,0,0,130,132,1,0,0,0,131,129,1,0,0,0,132,133,5,
        30,0,0,133,17,1,0,0,0,134,135,5,36,0,0,135,144,5,29,0,0,136,143,
        3,34,17,0,137,143,3,18,9,0,138,143,3,36,18,0,139,143,3,38,19,0,140,
        143,3,40,20,0,141,143,3,20,10,0,142,136,1,0,0,0,142,137,1,0,0,0,
        142,138,1,0,0,0,142,139,1,0,0,0,142,140,1,0,0,0,142,141,1,0,0,0,
        143,146,1,0,0,0,144,142,1,0,0,0,144,145,1,0,0,0,145,147,1,0,0,0,
        146,144,1,0,0,0,147,148,5,30,0,0,148,19,1,0,0,0,149,150,5,9,0,0,
        150,160,5,36,0,0,151,155,5,29,0,0,152,154,3,22,11,0,153,152,1,0,
        0,0,154,157,1,0,0,0,155,153,1,0,0,0,155,156,1,0,0,0,156,158,1,0,
        0,0,157,155,1,0,0,0,158,161,5,30,0,0,159,161,5,35,0,0,160,151,1,
        0,0,0,160,159,1,0,0,0,161,21,1,0,0,0,162,165,3,24,12,0,163,165,3,
        26,13,0,164,162,1,0,0,0,164,163,1,0,0,0,165,23,1,0,0,0,166,168,5,
        11,0,0,167,169,3,28,14,0,168,167,1,0,0,0,168,169,1,0,0,0,169,170,
        1,0,0,0,170,172,5,29,0,0,171,173,3,18,9,0,172,171,1,0,0,0,173,174,
        1,0,0,0,174,172,1,0,0,0,174,175,1,0,0,0,175,176,1,0,0,0,176,177,
        5,30,0,0,177,25,1,0,0,0,178,179,5,12,0,0,179,180,3,30,15,0,180,181,
        5,35,0,0,181,27,1,0,0,0,182,183,7,1,0,0,183,187,3,30,15,0,184,185,
        5,16,0,0,185,187,7,2,0,0,186,182,1,0,0,0,186,184,1,0,0,0,187,29,
        1,0,0,0,188,192,5,36,0,0,189,190,5,31,0,0,190,191,5,25,0,0,191,193,
        5,32,0,0,192,189,1,0,0,0,192,193,1,0,0,0,193,31,1,0,0,0,194,199,
        5,36,0,0,195,196,5,26,0,0,196,198,5,36,0,0,197,195,1,0,0,0,198,201,
        1,0,0,0,199,197,1,0,0,0,199,200,1,0,0,0,200,33,1,0,0,0,201,199,1,
        0,0,0,202,205,5,36,0,0,203,204,7,3,0,0,204,206,3,44,22,0,205,203,
        1,0,0,0,205,206,1,0,0,0,206,207,1,0,0,0,207,208,5,35,0,0,208,35,
        1,0,0,0,209,210,5,24,0,0,210,211,5,29,0,0,211,212,3,44,22,0,212,
        213,5,30,0,0,213,37,1,0,0,0,214,215,5,21,0,0,215,216,5,27,0,0,216,
        217,5,37,0,0,217,218,5,28,0,0,218,219,5,35,0,0,219,39,1,0,0,0,220,
        221,5,22,0,0,221,222,5,27,0,0,222,223,5,37,0,0,223,224,5,28,0,0,
        224,225,5,35,0,0,225,41,1,0,0,0,226,237,5,36,0,0,227,237,5,37,0,
        0,228,237,5,33,0,0,229,237,5,35,0,0,230,237,5,24,0,0,231,237,5,29,
        0,0,232,237,5,34,0,0,233,237,5,25,0,0,234,237,5,26,0,0,235,237,3,
        48,24,0,236,226,1,0,0,0,236,227,1,0,0,0,236,228,1,0,0,0,236,229,
        1,0,0,0,236,230,1,0,0,0,236,231,1,0,0,0,236,232,1,0,0,0,236,233,
        1,0,0,0,236,234,1,0,0,0,236,235,1,0,0,0,237,43,1,0,0,0,238,240,3,
        46,23,0,239,238,1,0,0,0,240,241,1,0,0,0,241,239,1,0,0,0,241,242,
        1,0,0,0,242,45,1,0,0,0,243,251,5,37,0,0,244,251,3,48,24,0,245,251,
        5,36,0,0,246,251,5,24,0,0,247,251,5,23,0,0,248,251,5,25,0,0,249,
        251,5,26,0,0,250,243,1,0,0,0,250,244,1,0,0,0,250,245,1,0,0,0,250,
        246,1,0,0,0,250,247,1,0,0,0,250,248,1,0,0,0,250,249,1,0,0,0,251,
        47,1,0,0,0,252,253,5,36,0,0,253,254,5,27,0,0,254,257,5,36,0,0,255,
        256,5,34,0,0,256,258,3,44,22,0,257,255,1,0,0,0,257,258,1,0,0,0,258,
        259,1,0,0,0,259,260,5,28,0,0,260,49,1,0,0,0,26,53,55,61,76,82,91,
        97,99,107,118,129,142,144,155,160,164,168,174,186,192,199,205,236,
        241,250,257
    ]

class CHTLParser ( Parser ):

    grammarFileName = "CHTL.g4"

    atn = ATNDeserializer().deserialize(serializedATN())

    decisionsToDFA = [ DFA(ds, i) for i, ds in enumerate(atn.decisionToState) ]

    sharedContextCache = PredictionContextCache()

    literalNames = [ "<INVALID>", "'[Import]'", "'[Namespace]'", "'[Configuration]'",
                     "'[Template]'", "'[Custom]'", "'@Chtl'", "'@Config'",
                     "'@Style'", "'@Element'", "'@Var'", "'insert'", "'delete'",
                     "'after'", "'before'", "'replace'", "'at'", "'top'",
                     "'bottom'", "'from'", "'as'", "'__style_ref__'", "'__script_ref__'",
                     "'style'", "'text'", "<INVALID>", "'.'", "'('", "')'",
                     "'{'", "'}'", "'['", "']'", "':'", "'='", "';'" ]

    symbolicNames = [ "<INVALID>", "IMPORT", "NAMESPACE", "CONFIGURATION",
                      "TEMPLATE", "CUSTOM", "AT_CHTL", "AT_CONFIG", "AT_STYLE",
                      "AT_ELEMENT", "AT_VAR", "INSERT", "DELETE", "AFTER",
                      "BEFORE", "REPLACE", "AT", "TOP", "BOTTOM", "FROM",
                      "AS", "STYLE_REF", "SCRIPT_REF", "STYLE", "TEXT",
                      "NUMBER", "DOT", "LPAR", "RPAR", "LBRACE", "RBRACE",
                      "LBRACK", "RBRACK", "COLON", "EQ", "SEMI", "IDENTIFIER",
                      "STRING", "LINE_COMMENT", "BLOCK_COMMENT", "WS" ]

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
    RULE_insertPosition = 14
    RULE_selector = 15
    RULE_namespacePath = 16
    RULE_attribute = 17
    RULE_textNode = 18
    RULE_stylePlaceholder = 19
    RULE_scriptPlaceholder = 20
    RULE_styleContent = 21
    RULE_value = 22
    RULE_valuePart = 23
    RULE_varUsage = 24

    ruleNames =  [ "document", "directive", "importStatement", "namespaceStatement",
                   "configurationStatement", "definition", "styleTemplate",
                   "elementTemplate", "varTemplate", "element", "elementUsage",
                   "specializationBody", "insertStatement", "deleteStatement",
                   "insertPosition", "selector", "namespacePath", "attribute",
                   "textNode", "stylePlaceholder", "scriptPlaceholder",
                   "styleContent", "value", "valuePart", "varUsage" ]

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
    AFTER=13
    BEFORE=14
    REPLACE=15
    AT=16
    TOP=17
    BOTTOM=18
    FROM=19
    AS=20
    STYLE_REF=21
    SCRIPT_REF=22
    STYLE=23
    TEXT=24
    NUMBER=25
    DOT=26
    LPAR=27
    RPAR=28
    LBRACE=29
    RBRACE=30
    LBRACK=31
    RBRACK=32
    COLON=33
    EQ=34
    SEMI=35
    IDENTIFIER=36
    STRING=37
    LINE_COMMENT=38
    BLOCK_COMMENT=39
    WS=40

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
            self.state = 55
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 68719476798) != 0):
                self.state = 53
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [1, 2, 3]:
                    self.state = 50
                    self.directive()
                    pass
                elif token in [4, 5]:
                    self.state = 51
                    self.definition()
                    pass
                elif token in [36]:
                    self.state = 52
                    self.element()
                    pass
                else:
                    raise NoViableAltException(self)

                self.state = 57
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
            self.state = 61
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [1]:
                self.enterOuterAlt(localctx, 1)
                self.state = 58
                self.importStatement()
                pass
            elif token in [2]:
                self.enterOuterAlt(localctx, 2)
                self.state = 59
                self.namespaceStatement()
                pass
            elif token in [3]:
                self.enterOuterAlt(localctx, 3)
                self.state = 60
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
            self.state = 63
            self.match(CHTLParser.IMPORT)
            self.state = 64
            self.match(CHTLParser.AT_CHTL)
            self.state = 65
            self.match(CHTLParser.FROM)
            self.state = 66
            _la = self._input.LA(1)
            if not(_la==36 or _la==37):
                self._errHandler.recoverInline(self)
            else:
                self._errHandler.reportMatch(self)
                self.consume()
            self.state = 67
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
            self.state = 69
            self.match(CHTLParser.NAMESPACE)
            self.state = 70
            self.match(CHTLParser.IDENTIFIER)
            self.state = 71
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
            self.state = 73
            self.match(CHTLParser.CONFIGURATION)
            self.state = 76
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==7:
                self.state = 74
                self.match(CHTLParser.AT_CONFIG)
                self.state = 75
                self.match(CHTLParser.IDENTIFIER)


            self.state = 78
            self.match(CHTLParser.LBRACE)
            self.state = 82
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==36:
                self.state = 79
                self.attribute()
                self.state = 84
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 85
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
            self.state = 99
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [4]:
                self.enterOuterAlt(localctx, 1)
                self.state = 87
                self.match(CHTLParser.TEMPLATE)
                self.state = 91
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [8]:
                    self.state = 88
                    self.styleTemplate()
                    pass
                elif token in [9]:
                    self.state = 89
                    self.elementTemplate()
                    pass
                elif token in [10]:
                    self.state = 90
                    self.varTemplate()
                    pass
                else:
                    raise NoViableAltException(self)

                pass
            elif token in [5]:
                self.enterOuterAlt(localctx, 2)
                self.state = 93
                self.match(CHTLParser.CUSTOM)
                self.state = 97
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [8]:
                    self.state = 94
                    self.styleTemplate()
                    pass
                elif token in [9]:
                    self.state = 95
                    self.elementTemplate()
                    pass
                elif token in [10]:
                    self.state = 96
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

        def attribute(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.AttributeContext)
            else:
                return self.getTypedRuleContext(CHTLParser.AttributeContext,i)


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
            self.state = 101
            self.match(CHTLParser.AT_STYLE)
            self.state = 102
            self.match(CHTLParser.IDENTIFIER)
            self.state = 103
            self.match(CHTLParser.LBRACE)
            self.state = 107
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==36:
                self.state = 104
                self.attribute()
                self.state = 109
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 110
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
            self.state = 112
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 113
            self.match(CHTLParser.IDENTIFIER)
            self.state = 114
            self.match(CHTLParser.LBRACE)
            self.state = 118
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==36:
                self.state = 115
                self.element()
                self.state = 120
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 121
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
            self.state = 123
            self.match(CHTLParser.AT_VAR)
            self.state = 124
            self.match(CHTLParser.IDENTIFIER)
            self.state = 125
            self.match(CHTLParser.LBRACE)
            self.state = 129
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==36:
                self.state = 126
                self.attribute()
                self.state = 131
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 132
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
            self.state = 134
            self.match(CHTLParser.IDENTIFIER)
            self.state = 135
            self.match(CHTLParser.LBRACE)
            self.state = 144
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 68742545920) != 0):
                self.state = 142
                self._errHandler.sync(self)
                la_ = self._interp.adaptivePredict(self._input,11,self._ctx)
                if la_ == 1:
                    self.state = 136
                    self.attribute()
                    pass

                elif la_ == 2:
                    self.state = 137
                    self.element()
                    pass

                elif la_ == 3:
                    self.state = 138
                    self.textNode()
                    pass

                elif la_ == 4:
                    self.state = 139
                    self.stylePlaceholder()
                    pass

                elif la_ == 5:
                    self.state = 140
                    self.scriptPlaceholder()
                    pass

                elif la_ == 6:
                    self.state = 141
                    self.elementUsage()
                    pass


                self.state = 146
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 147
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
            self.state = 149
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 150
            self.match(CHTLParser.IDENTIFIER)
            self.state = 160
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [29]:
                self.state = 151
                self.match(CHTLParser.LBRACE)
                self.state = 155
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                while _la==11 or _la==12:
                    self.state = 152
                    self.specializationBody()
                    self.state = 157
                    self._errHandler.sync(self)
                    _la = self._input.LA(1)

                self.state = 158
                self.match(CHTLParser.RBRACE)
                pass
            elif token in [35]:
                self.state = 159
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
            self.state = 164
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [11]:
                self.enterOuterAlt(localctx, 1)
                self.state = 162
                self.insertStatement()
                pass
            elif token in [12]:
                self.enterOuterAlt(localctx, 2)
                self.state = 163
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

        def insertPosition(self):
            return self.getTypedRuleContext(CHTLParser.InsertPositionContext,0)


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
            self.state = 166
            self.match(CHTLParser.INSERT)
            self.state = 168
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if (((_la) & ~0x3f) == 0 and ((1 << _la) & 122880) != 0):
                self.state = 167
                self.insertPosition()


            self.state = 170
            self.match(CHTLParser.LBRACE)
            self.state = 172
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while True:
                self.state = 171
                self.element()
                self.state = 174
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                if not (_la==36):
                    break

            self.state = 176
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

        def selector(self):
            return self.getTypedRuleContext(CHTLParser.SelectorContext,0)


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
            self.state = 178
            self.match(CHTLParser.DELETE)
            self.state = 179
            self.selector()
            self.state = 180
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class InsertPositionContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def selector(self):
            return self.getTypedRuleContext(CHTLParser.SelectorContext,0)


        def AFTER(self):
            return self.getToken(CHTLParser.AFTER, 0)

        def BEFORE(self):
            return self.getToken(CHTLParser.BEFORE, 0)

        def REPLACE(self):
            return self.getToken(CHTLParser.REPLACE, 0)

        def AT(self):
            return self.getToken(CHTLParser.AT, 0)

        def TOP(self):
            return self.getToken(CHTLParser.TOP, 0)

        def BOTTOM(self):
            return self.getToken(CHTLParser.BOTTOM, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_insertPosition

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterInsertPosition" ):
                listener.enterInsertPosition(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitInsertPosition" ):
                listener.exitInsertPosition(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitInsertPosition" ):
                return visitor.visitInsertPosition(self)
            else:
                return visitor.visitChildren(self)




    def insertPosition(self):

        localctx = CHTLParser.InsertPositionContext(self, self._ctx, self.state)
        self.enterRule(localctx, 28, self.RULE_insertPosition)
        self._la = 0 # Token type
        try:
            self.state = 186
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [13, 14, 15]:
                self.enterOuterAlt(localctx, 1)
                self.state = 182
                _la = self._input.LA(1)
                if not((((_la) & ~0x3f) == 0 and ((1 << _la) & 57344) != 0)):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 183
                self.selector()
                pass
            elif token in [16]:
                self.enterOuterAlt(localctx, 2)
                self.state = 184
                self.match(CHTLParser.AT)
                self.state = 185
                _la = self._input.LA(1)
                if not(_la==17 or _la==18):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
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


    class SelectorContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def LBRACK(self):
            return self.getToken(CHTLParser.LBRACK, 0)

        def NUMBER(self):
            return self.getToken(CHTLParser.NUMBER, 0)

        def RBRACK(self):
            return self.getToken(CHTLParser.RBRACK, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_selector

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

        localctx = CHTLParser.SelectorContext(self, self._ctx, self.state)
        self.enterRule(localctx, 30, self.RULE_selector)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 188
            self.match(CHTLParser.IDENTIFIER)
            self.state = 192
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==31:
                self.state = 189
                self.match(CHTLParser.LBRACK)
                self.state = 190
                self.match(CHTLParser.NUMBER)
                self.state = 191
                self.match(CHTLParser.RBRACK)


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
        self.enterRule(localctx, 32, self.RULE_namespacePath)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 194
            self.match(CHTLParser.IDENTIFIER)
            self.state = 199
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==26:
                self.state = 195
                self.match(CHTLParser.DOT)
                self.state = 196
                self.match(CHTLParser.IDENTIFIER)
                self.state = 201
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

        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def value(self):
            return self.getTypedRuleContext(CHTLParser.ValueContext,0)


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
        self.enterRule(localctx, 34, self.RULE_attribute)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 202
            self.match(CHTLParser.IDENTIFIER)
            self.state = 205
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==33 or _la==34:
                self.state = 203
                _la = self._input.LA(1)
                if not(_la==33 or _la==34):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 204
                self.value()


            self.state = 207
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
        self.enterRule(localctx, 36, self.RULE_textNode)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 209
            self.match(CHTLParser.TEXT)
            self.state = 210
            self.match(CHTLParser.LBRACE)
            self.state = 211
            self.value()
            self.state = 212
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
        self.enterRule(localctx, 38, self.RULE_stylePlaceholder)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 214
            self.match(CHTLParser.STYLE_REF)
            self.state = 215
            self.match(CHTLParser.LPAR)
            self.state = 216
            self.match(CHTLParser.STRING)
            self.state = 217
            self.match(CHTLParser.RPAR)
            self.state = 218
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
        self.enterRule(localctx, 40, self.RULE_scriptPlaceholder)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 220
            self.match(CHTLParser.SCRIPT_REF)
            self.state = 221
            self.match(CHTLParser.LPAR)
            self.state = 222
            self.match(CHTLParser.STRING)
            self.state = 223
            self.match(CHTLParser.RPAR)
            self.state = 224
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
        self.enterRule(localctx, 42, self.RULE_styleContent)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 236
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,22,self._ctx)
            if la_ == 1:
                self.state = 226
                self.match(CHTLParser.IDENTIFIER)
                pass

            elif la_ == 2:
                self.state = 227
                self.match(CHTLParser.STRING)
                pass

            elif la_ == 3:
                self.state = 228
                self.match(CHTLParser.COLON)
                pass

            elif la_ == 4:
                self.state = 229
                self.match(CHTLParser.SEMI)
                pass

            elif la_ == 5:
                self.state = 230
                self.match(CHTLParser.TEXT)
                pass

            elif la_ == 6:
                self.state = 231
                self.match(CHTLParser.LBRACE)
                pass

            elif la_ == 7:
                self.state = 232
                self.match(CHTLParser.EQ)
                pass

            elif la_ == 8:
                self.state = 233
                self.match(CHTLParser.NUMBER)
                pass

            elif la_ == 9:
                self.state = 234
                self.match(CHTLParser.DOT)
                pass

            elif la_ == 10:
                self.state = 235
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
        self.enterRule(localctx, 44, self.RULE_value)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 239
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while True:
                self.state = 238
                self.valuePart()
                self.state = 241
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                if not ((((_la) & ~0x3f) == 0 and ((1 << _la) & 206284259328) != 0)):
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
        self.enterRule(localctx, 46, self.RULE_valuePart)
        try:
            self.state = 250
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,24,self._ctx)
            if la_ == 1:
                self.enterOuterAlt(localctx, 1)
                self.state = 243
                self.match(CHTLParser.STRING)
                pass

            elif la_ == 2:
                self.enterOuterAlt(localctx, 2)
                self.state = 244
                self.varUsage()
                pass

            elif la_ == 3:
                self.enterOuterAlt(localctx, 3)
                self.state = 245
                self.match(CHTLParser.IDENTIFIER)
                pass

            elif la_ == 4:
                self.enterOuterAlt(localctx, 4)
                self.state = 246
                self.match(CHTLParser.TEXT)
                pass

            elif la_ == 5:
                self.enterOuterAlt(localctx, 5)
                self.state = 247
                self.match(CHTLParser.STYLE)
                pass

            elif la_ == 6:
                self.enterOuterAlt(localctx, 6)
                self.state = 248
                self.match(CHTLParser.NUMBER)
                pass

            elif la_ == 7:
                self.enterOuterAlt(localctx, 7)
                self.state = 249
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
        self.enterRule(localctx, 48, self.RULE_varUsage)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 252
            self.match(CHTLParser.IDENTIFIER)
            self.state = 253
            self.match(CHTLParser.LPAR)
            self.state = 254
            self.match(CHTLParser.IDENTIFIER)
            self.state = 257
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==34:
                self.state = 255
                self.match(CHTLParser.EQ)
                self.state = 256
                self.value()


            self.state = 259
            self.match(CHTLParser.RPAR)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx

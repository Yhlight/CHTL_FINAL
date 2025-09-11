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
        4,1,43,288,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
        6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,
        2,14,7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,
        7,20,2,21,7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,
        2,27,7,27,1,0,1,0,1,0,1,0,5,0,61,8,0,10,0,12,0,64,9,0,1,1,1,1,1,
        1,3,1,69,8,1,1,2,1,2,1,2,1,2,1,2,1,2,3,2,77,8,2,1,2,1,2,1,3,1,3,
        1,4,1,4,1,5,1,5,1,5,1,5,1,6,1,6,1,6,3,6,92,8,6,1,6,1,6,5,6,96,8,
        6,10,6,12,6,99,9,6,1,6,1,6,1,7,1,7,1,7,1,7,3,7,107,8,7,1,7,1,7,1,
        7,1,7,3,7,113,8,7,3,7,115,8,7,1,8,1,8,1,8,1,8,5,8,121,8,8,10,8,12,
        8,124,9,8,1,8,1,8,1,9,1,9,1,9,1,9,5,9,132,8,9,10,9,12,9,135,9,9,
        1,9,1,9,1,10,1,10,1,10,1,10,5,10,143,8,10,10,10,12,10,146,9,10,1,
        10,1,10,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,5,11,159,8,
        11,10,11,12,11,162,9,11,1,11,1,11,1,12,1,12,1,12,1,12,3,12,170,8,
        12,1,12,1,12,5,12,174,8,12,10,12,12,12,177,9,12,1,12,1,12,3,12,181,
        8,12,1,13,1,13,3,13,185,8,13,1,14,1,14,3,14,189,8,14,1,14,1,14,4,
        14,193,8,14,11,14,12,14,194,1,14,1,14,1,15,1,15,1,15,1,15,1,16,1,
        16,1,16,1,16,3,16,207,8,16,1,17,1,17,1,17,1,17,3,17,213,8,17,1,18,
        1,18,1,18,5,18,218,8,18,10,18,12,18,221,9,18,1,19,1,19,1,19,3,19,
        226,8,19,1,19,1,19,1,20,1,20,1,20,1,20,1,20,1,21,1,21,1,21,1,21,
        1,21,1,21,1,22,1,22,1,22,1,22,1,22,1,22,1,23,1,23,1,23,1,23,1,23,
        1,23,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,3,24,263,
        8,24,1,25,4,25,266,8,25,11,25,12,25,267,1,26,1,26,1,26,1,26,1,26,
        1,26,1,26,3,26,277,8,26,1,27,1,27,1,27,1,27,1,27,3,27,284,8,27,1,
        27,1,27,1,27,0,0,28,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,
        34,36,38,40,42,44,46,48,50,52,54,0,5,2,0,6,6,8,10,1,0,39,40,1,0,
        15,17,1,0,19,20,1,0,36,37,310,0,62,1,0,0,0,2,68,1,0,0,0,4,70,1,0,
        0,0,6,80,1,0,0,0,8,82,1,0,0,0,10,84,1,0,0,0,12,88,1,0,0,0,14,114,
        1,0,0,0,16,116,1,0,0,0,18,127,1,0,0,0,20,138,1,0,0,0,22,149,1,0,
        0,0,24,165,1,0,0,0,26,184,1,0,0,0,28,186,1,0,0,0,30,198,1,0,0,0,
        32,206,1,0,0,0,34,208,1,0,0,0,36,214,1,0,0,0,38,222,1,0,0,0,40,229,
        1,0,0,0,42,234,1,0,0,0,44,240,1,0,0,0,46,246,1,0,0,0,48,262,1,0,
        0,0,50,265,1,0,0,0,52,276,1,0,0,0,54,278,1,0,0,0,56,61,3,2,1,0,57,
        61,3,14,7,0,58,61,3,22,11,0,59,61,3,46,23,0,60,56,1,0,0,0,60,57,
        1,0,0,0,60,58,1,0,0,0,60,59,1,0,0,0,61,64,1,0,0,0,62,60,1,0,0,0,
        62,63,1,0,0,0,63,1,1,0,0,0,64,62,1,0,0,0,65,69,3,4,2,0,66,69,3,10,
        5,0,67,69,3,12,6,0,68,65,1,0,0,0,68,66,1,0,0,0,68,67,1,0,0,0,69,
        3,1,0,0,0,70,71,5,1,0,0,71,72,3,6,3,0,72,73,5,21,0,0,73,76,3,8,4,
        0,74,75,5,22,0,0,75,77,5,39,0,0,76,74,1,0,0,0,76,77,1,0,0,0,77,78,
        1,0,0,0,78,79,5,38,0,0,79,5,1,0,0,0,80,81,7,0,0,0,81,7,1,0,0,0,82,
        83,7,1,0,0,83,9,1,0,0,0,84,85,5,2,0,0,85,86,5,39,0,0,86,87,5,38,
        0,0,87,11,1,0,0,0,88,91,5,3,0,0,89,90,5,7,0,0,90,92,5,39,0,0,91,
        89,1,0,0,0,91,92,1,0,0,0,92,93,1,0,0,0,93,97,5,32,0,0,94,96,3,38,
        19,0,95,94,1,0,0,0,96,99,1,0,0,0,97,95,1,0,0,0,97,98,1,0,0,0,98,
        100,1,0,0,0,99,97,1,0,0,0,100,101,5,33,0,0,101,13,1,0,0,0,102,106,
        5,4,0,0,103,107,3,16,8,0,104,107,3,18,9,0,105,107,3,20,10,0,106,
        103,1,0,0,0,106,104,1,0,0,0,106,105,1,0,0,0,107,115,1,0,0,0,108,
        112,5,5,0,0,109,113,3,16,8,0,110,113,3,18,9,0,111,113,3,20,10,0,
        112,109,1,0,0,0,112,110,1,0,0,0,112,111,1,0,0,0,113,115,1,0,0,0,
        114,102,1,0,0,0,114,108,1,0,0,0,115,15,1,0,0,0,116,117,5,9,0,0,117,
        118,5,39,0,0,118,122,5,32,0,0,119,121,3,38,19,0,120,119,1,0,0,0,
        121,124,1,0,0,0,122,120,1,0,0,0,122,123,1,0,0,0,123,125,1,0,0,0,
        124,122,1,0,0,0,125,126,5,33,0,0,126,17,1,0,0,0,127,128,5,11,0,0,
        128,129,5,39,0,0,129,133,5,32,0,0,130,132,3,22,11,0,131,130,1,0,
        0,0,132,135,1,0,0,0,133,131,1,0,0,0,133,134,1,0,0,0,134,136,1,0,
        0,0,135,133,1,0,0,0,136,137,5,33,0,0,137,19,1,0,0,0,138,139,5,12,
        0,0,139,140,5,39,0,0,140,144,5,32,0,0,141,143,3,38,19,0,142,141,
        1,0,0,0,143,146,1,0,0,0,144,142,1,0,0,0,144,145,1,0,0,0,145,147,
        1,0,0,0,146,144,1,0,0,0,147,148,5,33,0,0,148,21,1,0,0,0,149,150,
        5,39,0,0,150,160,5,32,0,0,151,159,3,38,19,0,152,159,3,22,11,0,153,
        159,3,40,20,0,154,159,3,42,21,0,155,159,3,44,22,0,156,159,3,24,12,
        0,157,159,3,46,23,0,158,151,1,0,0,0,158,152,1,0,0,0,158,153,1,0,
        0,0,158,154,1,0,0,0,158,155,1,0,0,0,158,156,1,0,0,0,158,157,1,0,
        0,0,159,162,1,0,0,0,160,158,1,0,0,0,160,161,1,0,0,0,161,163,1,0,
        0,0,162,160,1,0,0,0,163,164,5,33,0,0,164,23,1,0,0,0,165,166,5,11,
        0,0,166,169,5,39,0,0,167,168,5,21,0,0,168,170,3,36,18,0,169,167,
        1,0,0,0,169,170,1,0,0,0,170,180,1,0,0,0,171,175,5,32,0,0,172,174,
        3,26,13,0,173,172,1,0,0,0,174,177,1,0,0,0,175,173,1,0,0,0,175,176,
        1,0,0,0,176,178,1,0,0,0,177,175,1,0,0,0,178,181,5,33,0,0,179,181,
        5,38,0,0,180,171,1,0,0,0,180,179,1,0,0,0,181,25,1,0,0,0,182,185,
        3,28,14,0,183,185,3,30,15,0,184,182,1,0,0,0,184,183,1,0,0,0,185,
        27,1,0,0,0,186,188,5,13,0,0,187,189,3,32,16,0,188,187,1,0,0,0,188,
        189,1,0,0,0,189,190,1,0,0,0,190,192,5,32,0,0,191,193,3,22,11,0,192,
        191,1,0,0,0,193,194,1,0,0,0,194,192,1,0,0,0,194,195,1,0,0,0,195,
        196,1,0,0,0,196,197,5,33,0,0,197,29,1,0,0,0,198,199,5,14,0,0,199,
        200,3,34,17,0,200,201,5,38,0,0,201,31,1,0,0,0,202,203,7,2,0,0,203,
        207,3,34,17,0,204,205,5,18,0,0,205,207,7,3,0,0,206,202,1,0,0,0,206,
        204,1,0,0,0,207,33,1,0,0,0,208,212,5,39,0,0,209,210,5,34,0,0,210,
        211,5,28,0,0,211,213,5,35,0,0,212,209,1,0,0,0,212,213,1,0,0,0,213,
        35,1,0,0,0,214,219,5,39,0,0,215,216,5,29,0,0,216,218,5,39,0,0,217,
        215,1,0,0,0,218,221,1,0,0,0,219,217,1,0,0,0,219,220,1,0,0,0,220,
        37,1,0,0,0,221,219,1,0,0,0,222,225,5,39,0,0,223,224,7,4,0,0,224,
        226,3,50,25,0,225,223,1,0,0,0,225,226,1,0,0,0,226,227,1,0,0,0,227,
        228,5,38,0,0,228,39,1,0,0,0,229,230,5,27,0,0,230,231,5,32,0,0,231,
        232,3,50,25,0,232,233,5,33,0,0,233,41,1,0,0,0,234,235,5,23,0,0,235,
        236,5,30,0,0,236,237,5,40,0,0,237,238,5,31,0,0,238,239,5,38,0,0,
        239,43,1,0,0,0,240,241,5,24,0,0,241,242,5,30,0,0,242,243,5,40,0,
        0,243,244,5,31,0,0,244,245,5,38,0,0,245,45,1,0,0,0,246,247,5,25,
        0,0,247,248,5,30,0,0,248,249,5,40,0,0,249,250,5,31,0,0,250,251,5,
        38,0,0,251,47,1,0,0,0,252,263,5,39,0,0,253,263,5,40,0,0,254,263,
        5,36,0,0,255,263,5,38,0,0,256,263,5,27,0,0,257,263,5,32,0,0,258,
        263,5,37,0,0,259,263,5,28,0,0,260,263,5,29,0,0,261,263,3,54,27,0,
        262,252,1,0,0,0,262,253,1,0,0,0,262,254,1,0,0,0,262,255,1,0,0,0,
        262,256,1,0,0,0,262,257,1,0,0,0,262,258,1,0,0,0,262,259,1,0,0,0,
        262,260,1,0,0,0,262,261,1,0,0,0,263,49,1,0,0,0,264,266,3,52,26,0,
        265,264,1,0,0,0,266,267,1,0,0,0,267,265,1,0,0,0,267,268,1,0,0,0,
        268,51,1,0,0,0,269,277,5,40,0,0,270,277,3,54,27,0,271,277,5,39,0,
        0,272,277,5,27,0,0,273,277,5,26,0,0,274,277,5,28,0,0,275,277,5,29,
        0,0,276,269,1,0,0,0,276,270,1,0,0,0,276,271,1,0,0,0,276,272,1,0,
        0,0,276,273,1,0,0,0,276,274,1,0,0,0,276,275,1,0,0,0,277,53,1,0,0,
        0,278,279,5,39,0,0,279,280,5,30,0,0,280,283,5,39,0,0,281,282,5,37,
        0,0,282,284,3,50,25,0,283,281,1,0,0,0,283,284,1,0,0,0,284,285,1,
        0,0,0,285,286,5,31,0,0,286,55,1,0,0,0,28,60,62,68,76,91,97,106,112,
        114,122,133,144,158,160,169,175,180,184,188,194,206,212,219,225,
        262,267,276,283
    ]

class CHTLParser ( Parser ):

    grammarFileName = "CHTL.g4"

    atn = ATNDeserializer().deserialize(serializedATN())

    decisionsToDFA = [ DFA(ds, i) for i, ds in enumerate(atn.decisionToState) ]

    sharedContextCache = PredictionContextCache()

    literalNames = [ "<INVALID>", "'[Import]'", "'[Namespace]'", "'[Configuration]'",
                     "'[Template]'", "'[Custom]'", "'@Chtl'", "'@Config'",
                     "'@Html'", "'@Style'", "'@JavaScript'", "'@Element'",
                     "'@Var'", "'insert'", "'delete'", "'after'", "'before'",
                     "'replace'", "'at'", "'top'", "'bottom'", "'from'",
                     "'as'", "'__style_ref__'", "'__script_ref__'", "'__origin_ref__'",
                     "'style'", "'text'", "<INVALID>", "'.'", "'('", "')'",
                     "'{'", "'}'", "'['", "']'", "':'", "'='", "';'" ]

    symbolicNames = [ "<INVALID>", "IMPORT", "NAMESPACE", "CONFIGURATION",
                      "TEMPLATE", "CUSTOM", "AT_CHTL", "AT_CONFIG", "AT_HTML",
                      "AT_STYLE", "AT_JAVASCRIPT", "AT_ELEMENT", "AT_VAR",
                      "INSERT", "DELETE", "AFTER", "BEFORE", "REPLACE",
                      "AT", "TOP", "BOTTOM", "FROM", "AS", "STYLE_REF",
                      "SCRIPT_REF", "ORIGIN_REF", "STYLE", "TEXT", "NUMBER",
                      "DOT", "LPAR", "RPAR", "LBRACE", "RBRACE", "LBRACK",
                      "RBRACK", "COLON", "EQ", "SEMI", "IDENTIFIER", "STRING",
                      "LINE_COMMENT", "BLOCK_COMMENT", "WS" ]

    RULE_document = 0
    RULE_directive = 1
    RULE_importStatement = 2
    RULE_importType = 3
    RULE_path = 4
    RULE_namespaceStatement = 5
    RULE_configurationStatement = 6
    RULE_definition = 7
    RULE_styleTemplate = 8
    RULE_elementTemplate = 9
    RULE_varTemplate = 10
    RULE_element = 11
    RULE_elementUsage = 12
    RULE_specializationBody = 13
    RULE_insertStatement = 14
    RULE_deleteStatement = 15
    RULE_insertPosition = 16
    RULE_selector = 17
    RULE_namespacePath = 18
    RULE_attribute = 19
    RULE_textNode = 20
    RULE_stylePlaceholder = 21
    RULE_scriptPlaceholder = 22
    RULE_originPlaceholder = 23
    RULE_styleContent = 24
    RULE_value = 25
    RULE_valuePart = 26
    RULE_varUsage = 27

    ruleNames =  [ "document", "directive", "importStatement", "importType",
                   "path", "namespaceStatement", "configurationStatement",
                   "definition", "styleTemplate", "elementTemplate", "varTemplate",
                   "element", "elementUsage", "specializationBody", "insertStatement",
                   "deleteStatement", "insertPosition", "selector", "namespacePath",
                   "attribute", "textNode", "stylePlaceholder", "scriptPlaceholder",
                   "originPlaceholder", "styleContent", "value", "valuePart",
                   "varUsage" ]

    EOF = Token.EOF
    IMPORT=1
    NAMESPACE=2
    CONFIGURATION=3
    TEMPLATE=4
    CUSTOM=5
    AT_CHTL=6
    AT_CONFIG=7
    AT_HTML=8
    AT_STYLE=9
    AT_JAVASCRIPT=10
    AT_ELEMENT=11
    AT_VAR=12
    INSERT=13
    DELETE=14
    AFTER=15
    BEFORE=16
    REPLACE=17
    AT=18
    TOP=19
    BOTTOM=20
    FROM=21
    AS=22
    STYLE_REF=23
    SCRIPT_REF=24
    ORIGIN_REF=25
    STYLE=26
    TEXT=27
    NUMBER=28
    DOT=29
    LPAR=30
    RPAR=31
    LBRACE=32
    RBRACE=33
    LBRACK=34
    RBRACK=35
    COLON=36
    EQ=37
    SEMI=38
    IDENTIFIER=39
    STRING=40
    LINE_COMMENT=41
    BLOCK_COMMENT=42
    WS=43

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


        def originPlaceholder(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.OriginPlaceholderContext)
            else:
                return self.getTypedRuleContext(CHTLParser.OriginPlaceholderContext,i)


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
            self.state = 62
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 549789368382) != 0):
                self.state = 60
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [1, 2, 3]:
                    self.state = 56
                    self.directive()
                    pass
                elif token in [4, 5]:
                    self.state = 57
                    self.definition()
                    pass
                elif token in [39]:
                    self.state = 58
                    self.element()
                    pass
                elif token in [25]:
                    self.state = 59
                    self.originPlaceholder()
                    pass
                else:
                    raise NoViableAltException(self)

                self.state = 64
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
            self.state = 68
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [1]:
                self.enterOuterAlt(localctx, 1)
                self.state = 65
                self.importStatement()
                pass
            elif token in [2]:
                self.enterOuterAlt(localctx, 2)
                self.state = 66
                self.namespaceStatement()
                pass
            elif token in [3]:
                self.enterOuterAlt(localctx, 3)
                self.state = 67
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

        def importType(self):
            return self.getTypedRuleContext(CHTLParser.ImportTypeContext,0)


        def FROM(self):
            return self.getToken(CHTLParser.FROM, 0)

        def path(self):
            return self.getTypedRuleContext(CHTLParser.PathContext,0)


        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def AS(self):
            return self.getToken(CHTLParser.AS, 0)

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
            self.state = 70
            self.match(CHTLParser.IMPORT)
            self.state = 71
            self.importType()
            self.state = 72
            self.match(CHTLParser.FROM)
            self.state = 73
            self.path()
            self.state = 76
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==22:
                self.state = 74
                self.match(CHTLParser.AS)
                self.state = 75
                self.match(CHTLParser.IDENTIFIER)


            self.state = 78
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class ImportTypeContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def AT_HTML(self):
            return self.getToken(CHTLParser.AT_HTML, 0)

        def AT_STYLE(self):
            return self.getToken(CHTLParser.AT_STYLE, 0)

        def AT_JAVASCRIPT(self):
            return self.getToken(CHTLParser.AT_JAVASCRIPT, 0)

        def AT_CHTL(self):
            return self.getToken(CHTLParser.AT_CHTL, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_importType

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterImportType" ):
                listener.enterImportType(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitImportType" ):
                listener.exitImportType(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitImportType" ):
                return visitor.visitImportType(self)
            else:
                return visitor.visitChildren(self)




    def importType(self):

        localctx = CHTLParser.ImportTypeContext(self, self._ctx, self.state)
        self.enterRule(localctx, 6, self.RULE_importType)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 80
            _la = self._input.LA(1)
            if not((((_la) & ~0x3f) == 0 and ((1 << _la) & 1856) != 0)):
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


    class PathContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def STRING(self):
            return self.getToken(CHTLParser.STRING, 0)

        def IDENTIFIER(self):
            return self.getToken(CHTLParser.IDENTIFIER, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_path

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterPath" ):
                listener.enterPath(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitPath" ):
                listener.exitPath(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitPath" ):
                return visitor.visitPath(self)
            else:
                return visitor.visitChildren(self)




    def path(self):

        localctx = CHTLParser.PathContext(self, self._ctx, self.state)
        self.enterRule(localctx, 8, self.RULE_path)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 82
            _la = self._input.LA(1)
            if not(_la==39 or _la==40):
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
        self.enterRule(localctx, 10, self.RULE_namespaceStatement)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 84
            self.match(CHTLParser.NAMESPACE)
            self.state = 85
            self.match(CHTLParser.IDENTIFIER)
            self.state = 86
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
        self.enterRule(localctx, 12, self.RULE_configurationStatement)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 88
            self.match(CHTLParser.CONFIGURATION)
            self.state = 91
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==7:
                self.state = 89
                self.match(CHTLParser.AT_CONFIG)
                self.state = 90
                self.match(CHTLParser.IDENTIFIER)


            self.state = 93
            self.match(CHTLParser.LBRACE)
            self.state = 97
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==39:
                self.state = 94
                self.attribute()
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
        self.enterRule(localctx, 14, self.RULE_definition)
        try:
            self.state = 114
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [4]:
                self.enterOuterAlt(localctx, 1)
                self.state = 102
                self.match(CHTLParser.TEMPLATE)
                self.state = 106
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [9]:
                    self.state = 103
                    self.styleTemplate()
                    pass
                elif token in [11]:
                    self.state = 104
                    self.elementTemplate()
                    pass
                elif token in [12]:
                    self.state = 105
                    self.varTemplate()
                    pass
                else:
                    raise NoViableAltException(self)

                pass
            elif token in [5]:
                self.enterOuterAlt(localctx, 2)
                self.state = 108
                self.match(CHTLParser.CUSTOM)
                self.state = 112
                self._errHandler.sync(self)
                token = self._input.LA(1)
                if token in [9]:
                    self.state = 109
                    self.styleTemplate()
                    pass
                elif token in [11]:
                    self.state = 110
                    self.elementTemplate()
                    pass
                elif token in [12]:
                    self.state = 111
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
        self.enterRule(localctx, 16, self.RULE_styleTemplate)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 116
            self.match(CHTLParser.AT_STYLE)
            self.state = 117
            self.match(CHTLParser.IDENTIFIER)
            self.state = 118
            self.match(CHTLParser.LBRACE)
            self.state = 122
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==39:
                self.state = 119
                self.attribute()
                self.state = 124
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 125
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
        self.enterRule(localctx, 18, self.RULE_elementTemplate)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 127
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 128
            self.match(CHTLParser.IDENTIFIER)
            self.state = 129
            self.match(CHTLParser.LBRACE)
            self.state = 133
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==39:
                self.state = 130
                self.element()
                self.state = 135
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 136
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
        self.enterRule(localctx, 20, self.RULE_varTemplate)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 138
            self.match(CHTLParser.AT_VAR)
            self.state = 139
            self.match(CHTLParser.IDENTIFIER)
            self.state = 140
            self.match(CHTLParser.LBRACE)
            self.state = 144
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==39:
                self.state = 141
                self.attribute()
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


        def originPlaceholder(self, i:int=None):
            if i is None:
                return self.getTypedRuleContexts(CHTLParser.OriginPlaceholderContext)
            else:
                return self.getTypedRuleContext(CHTLParser.OriginPlaceholderContext,i)


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
        self.enterRule(localctx, 22, self.RULE_element)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 149
            self.match(CHTLParser.IDENTIFIER)
            self.state = 150
            self.match(CHTLParser.LBRACE)
            self.state = 160
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while (((_la) & ~0x3f) == 0 and ((1 << _la) & 549948753920) != 0):
                self.state = 158
                self._errHandler.sync(self)
                la_ = self._interp.adaptivePredict(self._input,12,self._ctx)
                if la_ == 1:
                    self.state = 151
                    self.attribute()
                    pass

                elif la_ == 2:
                    self.state = 152
                    self.element()
                    pass

                elif la_ == 3:
                    self.state = 153
                    self.textNode()
                    pass

                elif la_ == 4:
                    self.state = 154
                    self.stylePlaceholder()
                    pass

                elif la_ == 5:
                    self.state = 155
                    self.scriptPlaceholder()
                    pass

                elif la_ == 6:
                    self.state = 156
                    self.elementUsage()
                    pass

                elif la_ == 7:
                    self.state = 157
                    self.originPlaceholder()
                    pass


                self.state = 162
                self._errHandler.sync(self)
                _la = self._input.LA(1)

            self.state = 163
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

        def FROM(self):
            return self.getToken(CHTLParser.FROM, 0)

        def namespacePath(self):
            return self.getTypedRuleContext(CHTLParser.NamespacePathContext,0)


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
        self.enterRule(localctx, 24, self.RULE_elementUsage)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 165
            self.match(CHTLParser.AT_ELEMENT)
            self.state = 166
            self.match(CHTLParser.IDENTIFIER)
            self.state = 169
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==21:
                self.state = 167
                self.match(CHTLParser.FROM)
                self.state = 168
                self.namespacePath()


            self.state = 180
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [32]:
                self.state = 171
                self.match(CHTLParser.LBRACE)
                self.state = 175
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                while _la==13 or _la==14:
                    self.state = 172
                    self.specializationBody()
                    self.state = 177
                    self._errHandler.sync(self)
                    _la = self._input.LA(1)

                self.state = 178
                self.match(CHTLParser.RBRACE)
                pass
            elif token in [38]:
                self.state = 179
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
        self.enterRule(localctx, 26, self.RULE_specializationBody)
        try:
            self.state = 184
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [13]:
                self.enterOuterAlt(localctx, 1)
                self.state = 182
                self.insertStatement()
                pass
            elif token in [14]:
                self.enterOuterAlt(localctx, 2)
                self.state = 183
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
        self.enterRule(localctx, 28, self.RULE_insertStatement)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 186
            self.match(CHTLParser.INSERT)
            self.state = 188
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if (((_la) & ~0x3f) == 0 and ((1 << _la) & 491520) != 0):
                self.state = 187
                self.insertPosition()


            self.state = 190
            self.match(CHTLParser.LBRACE)
            self.state = 192
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while True:
                self.state = 191
                self.element()
                self.state = 194
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                if not (_la==39):
                    break

            self.state = 196
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
        self.enterRule(localctx, 30, self.RULE_deleteStatement)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 198
            self.match(CHTLParser.DELETE)
            self.state = 199
            self.selector()
            self.state = 200
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
        self.enterRule(localctx, 32, self.RULE_insertPosition)
        self._la = 0 # Token type
        try:
            self.state = 206
            self._errHandler.sync(self)
            token = self._input.LA(1)
            if token in [15, 16, 17]:
                self.enterOuterAlt(localctx, 1)
                self.state = 202
                _la = self._input.LA(1)
                if not((((_la) & ~0x3f) == 0 and ((1 << _la) & 229376) != 0)):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 203
                self.selector()
                pass
            elif token in [18]:
                self.enterOuterAlt(localctx, 2)
                self.state = 204
                self.match(CHTLParser.AT)
                self.state = 205
                _la = self._input.LA(1)
                if not(_la==19 or _la==20):
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
        self.enterRule(localctx, 34, self.RULE_selector)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 208
            self.match(CHTLParser.IDENTIFIER)
            self.state = 212
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==34:
                self.state = 209
                self.match(CHTLParser.LBRACK)
                self.state = 210
                self.match(CHTLParser.NUMBER)
                self.state = 211
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
        self.enterRule(localctx, 36, self.RULE_namespacePath)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 214
            self.match(CHTLParser.IDENTIFIER)
            self.state = 219
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while _la==29:
                self.state = 215
                self.match(CHTLParser.DOT)
                self.state = 216
                self.match(CHTLParser.IDENTIFIER)
                self.state = 221
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
        self.enterRule(localctx, 38, self.RULE_attribute)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 222
            self.match(CHTLParser.IDENTIFIER)
            self.state = 225
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==36 or _la==37:
                self.state = 223
                _la = self._input.LA(1)
                if not(_la==36 or _la==37):
                    self._errHandler.recoverInline(self)
                else:
                    self._errHandler.reportMatch(self)
                    self.consume()
                self.state = 224
                self.value()


            self.state = 227
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
        self.enterRule(localctx, 40, self.RULE_textNode)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 229
            self.match(CHTLParser.TEXT)
            self.state = 230
            self.match(CHTLParser.LBRACE)
            self.state = 231
            self.value()
            self.state = 232
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
        self.enterRule(localctx, 42, self.RULE_stylePlaceholder)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 234
            self.match(CHTLParser.STYLE_REF)
            self.state = 235
            self.match(CHTLParser.LPAR)
            self.state = 236
            self.match(CHTLParser.STRING)
            self.state = 237
            self.match(CHTLParser.RPAR)
            self.state = 238
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
        self.enterRule(localctx, 44, self.RULE_scriptPlaceholder)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 240
            self.match(CHTLParser.SCRIPT_REF)
            self.state = 241
            self.match(CHTLParser.LPAR)
            self.state = 242
            self.match(CHTLParser.STRING)
            self.state = 243
            self.match(CHTLParser.RPAR)
            self.state = 244
            self.match(CHTLParser.SEMI)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx


    class OriginPlaceholderContext(ParserRuleContext):
        __slots__ = 'parser'

        def __init__(self, parser, parent:ParserRuleContext=None, invokingState:int=-1):
            super().__init__(parent, invokingState)
            self.parser = parser

        def ORIGIN_REF(self):
            return self.getToken(CHTLParser.ORIGIN_REF, 0)

        def LPAR(self):
            return self.getToken(CHTLParser.LPAR, 0)

        def STRING(self):
            return self.getToken(CHTLParser.STRING, 0)

        def RPAR(self):
            return self.getToken(CHTLParser.RPAR, 0)

        def SEMI(self):
            return self.getToken(CHTLParser.SEMI, 0)

        def getRuleIndex(self):
            return CHTLParser.RULE_originPlaceholder

        def enterRule(self, listener:ParseTreeListener):
            if hasattr( listener, "enterOriginPlaceholder" ):
                listener.enterOriginPlaceholder(self)

        def exitRule(self, listener:ParseTreeListener):
            if hasattr( listener, "exitOriginPlaceholder" ):
                listener.exitOriginPlaceholder(self)

        def accept(self, visitor:ParseTreeVisitor):
            if hasattr( visitor, "visitOriginPlaceholder" ):
                return visitor.visitOriginPlaceholder(self)
            else:
                return visitor.visitChildren(self)




    def originPlaceholder(self):

        localctx = CHTLParser.OriginPlaceholderContext(self, self._ctx, self.state)
        self.enterRule(localctx, 46, self.RULE_originPlaceholder)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 246
            self.match(CHTLParser.ORIGIN_REF)
            self.state = 247
            self.match(CHTLParser.LPAR)
            self.state = 248
            self.match(CHTLParser.STRING)
            self.state = 249
            self.match(CHTLParser.RPAR)
            self.state = 250
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
        self.enterRule(localctx, 48, self.RULE_styleContent)
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 262
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,24,self._ctx)
            if la_ == 1:
                self.state = 252
                self.match(CHTLParser.IDENTIFIER)
                pass

            elif la_ == 2:
                self.state = 253
                self.match(CHTLParser.STRING)
                pass

            elif la_ == 3:
                self.state = 254
                self.match(CHTLParser.COLON)
                pass

            elif la_ == 4:
                self.state = 255
                self.match(CHTLParser.SEMI)
                pass

            elif la_ == 5:
                self.state = 256
                self.match(CHTLParser.TEXT)
                pass

            elif la_ == 6:
                self.state = 257
                self.match(CHTLParser.LBRACE)
                pass

            elif la_ == 7:
                self.state = 258
                self.match(CHTLParser.EQ)
                pass

            elif la_ == 8:
                self.state = 259
                self.match(CHTLParser.NUMBER)
                pass

            elif la_ == 9:
                self.state = 260
                self.match(CHTLParser.DOT)
                pass

            elif la_ == 10:
                self.state = 261
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
        self.enterRule(localctx, 50, self.RULE_value)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 265
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            while True:
                self.state = 264
                self.valuePart()
                self.state = 267
                self._errHandler.sync(self)
                _la = self._input.LA(1)
                if not ((((_la) & ~0x3f) == 0 and ((1 << _la) & 1650274074624) != 0)):
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
        self.enterRule(localctx, 52, self.RULE_valuePart)
        try:
            self.state = 276
            self._errHandler.sync(self)
            la_ = self._interp.adaptivePredict(self._input,26,self._ctx)
            if la_ == 1:
                self.enterOuterAlt(localctx, 1)
                self.state = 269
                self.match(CHTLParser.STRING)
                pass

            elif la_ == 2:
                self.enterOuterAlt(localctx, 2)
                self.state = 270
                self.varUsage()
                pass

            elif la_ == 3:
                self.enterOuterAlt(localctx, 3)
                self.state = 271
                self.match(CHTLParser.IDENTIFIER)
                pass

            elif la_ == 4:
                self.enterOuterAlt(localctx, 4)
                self.state = 272
                self.match(CHTLParser.TEXT)
                pass

            elif la_ == 5:
                self.enterOuterAlt(localctx, 5)
                self.state = 273
                self.match(CHTLParser.STYLE)
                pass

            elif la_ == 6:
                self.enterOuterAlt(localctx, 6)
                self.state = 274
                self.match(CHTLParser.NUMBER)
                pass

            elif la_ == 7:
                self.enterOuterAlt(localctx, 7)
                self.state = 275
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
        self.enterRule(localctx, 54, self.RULE_varUsage)
        self._la = 0 # Token type
        try:
            self.enterOuterAlt(localctx, 1)
            self.state = 278
            self.match(CHTLParser.IDENTIFIER)
            self.state = 279
            self.match(CHTLParser.LPAR)
            self.state = 280
            self.match(CHTLParser.IDENTIFIER)
            self.state = 283
            self._errHandler.sync(self)
            _la = self._input.LA(1)
            if _la==37:
                self.state = 281
                self.match(CHTLParser.EQ)
                self.state = 282
                self.value()


            self.state = 285
            self.match(CHTLParser.RPAR)
        except RecognitionException as re:
            localctx.exception = re
            self._errHandler.reportError(self, re)
            self._errHandler.recover(self, re)
        finally:
            self.exitRule()
        return localctx

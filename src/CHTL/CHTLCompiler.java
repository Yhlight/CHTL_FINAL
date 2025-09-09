package CHTL;

import CHTL.Lexer.CHTLLexer;
import CHTL.Lexer.Token;
import CHTL.Node.BaseNode;
import CHTL.Parser.CHTLParser;
import Scanner.SourceFragment;

import java.util.List;

import CHTL.Manage.DefinitionManager;

// Main class for the CHTL compilation pipeline.
public class CHTLCompiler {

    private final DefinitionManager definitionManager;

    public CHTLCompiler(DefinitionManager definitionManager) {
        this.definitionManager = definitionManager;
    }

    public List<BaseNode> compile(SourceFragment fragment) {
        if (fragment.type() != Scanner.FragmentType.CHTL) {
            throw new IllegalArgumentException("CHTLCompiler can only handle CHTL fragments.");
        }

        // 1. Lexing
        CHTLLexer lexer = new CHTLLexer(fragment.content());
        List<Token> tokens = lexer.tokenize();

        // 2. Parsing
        CHTLParser parser = new CHTLParser(tokens, this.definitionManager);
        List<BaseNode> ast = parser.parse();

        // 3. Processing (to be done by the dispatcher after all fragments are parsed)
        return ast;
    }
}

package CHTL;

import CHTL.Lexer.CHTLLexer;
import CHTL.Lexer.Token;
import CHTL.Node.BaseNode;
import CHTL.Parser.CHTLParser;
import Scanner.SourceFragment;

import java.util.List;

// Main class for the CHTL compilation pipeline.
public class CHTLCompiler {
    public List<BaseNode> compile(SourceFragment fragment) {
        if (fragment.type() != Scanner.FragmentType.CHTL) {
            throw new IllegalArgumentException("CHTLCompiler can only handle CHTL fragments.");
        }

        // 1. Lexing
        CHTLLexer lexer = new CHTLLexer(fragment.content());
        List<Token> tokens = lexer.tokenize();

        // 2. Parsing
        CHTLParser parser = new CHTLParser(tokens);
        List<BaseNode> ast = parser.parse();

        // 3. Processing (to be done later by the main dispatcher)
        // ASTProcessor processor = new ASTProcessor(ast, new DefinitionManager());
        // processor.process();

        return ast;
    }
}

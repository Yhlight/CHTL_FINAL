package CompilerDispatcher;

import CHTL.CHTLLexer.CHTLLexer;
import CHTL.CHTLLexer.Token;
import CHTL.CHTLLoader.CHTLLoader;
import CHTL.CHTLManage.DefinitionManager;
import CHTL.CHTLParser.CHTLParser;
import CHTL.CHTLNode.BaseNode;
import CHTL.CHTLProcessor.ProcessedAST;
import CHTL.CHTLProcessor.ASTProcessor;
import CHTL.CHTLGenerator.CHTLGenerator;

import java.util.List;

public class CompilerDispatcher {
    public static void main(String[] args) {
        // A CHTL source code example for testing the import system.
        String source = "[Import] @Chtl from \"other.chtl\"\n" +
                "\n" +
                "body {\n" +
                "    @Element ImportedBox from other;\n" +
                "}";

        System.out.println("--- CHTL Source ---");
        System.out.println(source);

        try {
            // 0. Setup
            DefinitionManager definitionManager = new DefinitionManager();
            CHTLLoader loader = new CHTLLoader();

            // 1. Lexer
            CHTLLexer lexer = new CHTLLexer(source);
            List<Token> tokens = lexer.tokenize();

            // 2. Parser
            // The initial parser is for the main file, in the default namespace.
            CHTLParser parser = new CHTLParser(tokens, definitionManager, DefinitionManager.DEFAULT_NAMESPACE, loader);
            List<BaseNode> ast = parser.parse();

            // 3. AST Processor
            ASTProcessor astProcessor = new ASTProcessor(definitionManager);
            ProcessedAST processedAST = astProcessor.process(ast);

            // 4. Generator
            CHTLGenerator generator = new CHTLGenerator();
            String bodyContent = generator.generate(processedAST.ast());
            String styleBlock = generator.generateStyleBlock(processedAST.globalStyles());

            // 5. Final Assembly
            String finalHtml;
            if (!styleBlock.isEmpty() && bodyContent.contains("</head>")) {
                finalHtml = bodyContent.replace("</head>", styleBlock + "</head>");
            } else {
                finalHtml = bodyContent;
            }

            System.out.println("\n--- Generated HTML ---");
            System.out.println(finalHtml);

        } catch (Exception e) {
            System.err.println("\n--- Compilation Failed ---");
            e.printStackTrace();
        }
    }
}

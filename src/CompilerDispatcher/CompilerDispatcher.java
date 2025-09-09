package CompilerDispatcher;

import CHTL.CHTLLexer.CHTLLexer;
import CHTL.CHTLLexer.Token;
import CHTL.CHTLLexer.TokenType;
import CHTL.CHTLLoader.CHTLLoader;
import CHTL.CHTLManage.ConfigurationManager;
import CHTL.CHTLManage.DefinitionManager;
import CHTL.CHTLParser.CHTLParser;
import CHTL.CHTLNode.BaseNode;
import CHTL.CHTLNode.ConfigurationNode;
import CHTL.CHTLProcessor.ProcessedAST;
import CHTL.CHTLProcessor.ASTProcessor;
import CHTL.CHTLGenerator.CHTLGenerator;

import java.util.List;
import java.util.Map;

public class CompilerDispatcher {
    public static void main(String[] args) {
        // Final test case for dynamic keywords.
        String source = "[Configuration] {\n" +
                "    [Name] {\n" +
                "        KEYWORD_STYLE = css;\n" +
                "    }\n" +
                "}\n" +
                "div {\n" +
                "    css { \n" +
                "        color: green;\n" +
                "    }\n" +
                "}";

        System.out.println("--- CHTL Source ---");
        System.out.println(source);

        try {
            // 0. Setup
            DefinitionManager definitionManager = new DefinitionManager();
            ConfigurationManager configManager = new ConfigurationManager();
            CHTLLoader loader = new CHTLLoader();

            // Pass 1
            CHTLLexer preLexer = new CHTLLexer(source);
            List<Token> preTokens = preLexer.tokenize();
            CHTLParser preParser = new CHTLParser(source, preTokens, definitionManager, DefinitionManager.DEFAULT_NAMESPACE, loader);
            List<ConfigurationNode> configNodes = preParser.preScanForConfiguration();

            for (ConfigurationNode configNode : configNodes) {
                configManager.processConfigNode(configNode);
            }

            // Pass 2
            Map<String, TokenType> activeKeywords = configManager.getActiveKeywords(CHTLLexer.getDefaultKeywords());
            CHTLLexer lexer = new CHTLLexer(source, activeKeywords);
            List<Token> tokens = lexer.tokenize();

            CHTLParser parser = new CHTLParser(source, tokens, definitionManager, DefinitionManager.DEFAULT_NAMESPACE, loader);
            List<BaseNode> ast = parser.parse();

            ASTProcessor astProcessor = new ASTProcessor(definitionManager);
            ProcessedAST processedAST = astProcessor.process(ast);

            CHTLGenerator generator = new CHTLGenerator();
            String bodyContent = generator.generate(processedAST.ast());
            String styleBlock = generator.generateStyleBlock(processedAST.globalStyles());

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

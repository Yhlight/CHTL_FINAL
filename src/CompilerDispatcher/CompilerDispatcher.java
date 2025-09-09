package CompilerDispatcher;

import CHTL.CHTLCompiler;
import CHTL.Node.BaseNode;
import CHTL_JS.Generator.CHTLJSGenerator;
import CHTL_JS.Lexer.CHTLJSLexer;
import CHTL_JS.Node.CHTLJSBaseNode;
import CHTL_JS.Parser.CHTLJSParser;
import CSS.CssCompiler;
import JS.JsCompiler;
import Scanner.CHTLUnifiedScanner;
import Scanner.SourceFragment;
import java.util.ArrayList;
import java.util.List;
import CodeMerger;

public class CompilerDispatcher {
    public static void main(String[] args) {
        String source = "style { body { font-family: sans-serif; } }\n" +
                        "\n" +
                        "div {\n" +
                        "    id: main-box;\n" +
                        "    style {\n" +
                        "        width: 100px * 2;\n" +
                        "        height: width;\n" +
                        "        border: 1px solid black;\n" +
                        "    }\n" +
                        "    text: \"Hello, World!\";\n" +
                        "    script {\n" +
                        "        {{#main-box}} -> listen { \n" +
                        "             click: () => { this.style.backgroundColor = 'lightblue'; }\n" +
                        "        };\n" +
                        "    }\n" +
                        "}\n" +
                        "\n" +
                        "script { console.log('Global script loaded.'); }";

        System.out.println("--- Source Code ---\n" + source + "\n");

        try {
            // 1. Scanning
            CHTLUnifiedScanner scanner = new CHTLUnifiedScanner(source);
            List<SourceFragment> fragments = scanner.scan();

            // 2. Compilation
            List<BaseNode> chtlAst = new ArrayList<>();
            List<String> cssOutputs = new ArrayList<>();
            List<String> jsOutputs = new ArrayList<>();

            DefinitionManager definitionManager = new DefinitionManager();
            CHTLCompiler chtlCompiler = new CHTLCompiler(definitionManager);
            CHTLJSGenerator chtljsGenerator = new CHTLJSGenerator();
            CssCompiler cssCompiler = new CssCompiler();
            JsCompiler jsCompiler = new JsCompiler();

            for (SourceFragment fragment : fragments) {
                switch (fragment.type()) {
                    case CHTL:
                        chtlAst.addAll(chtlCompiler.compile(fragment));
                        break;
                    case GLOBAL_CSS:
                        cssOutputs.add(cssCompiler.compile(fragment));
                        break;
                    case GLOBAL_JS:
                        jsOutputs.add(jsCompiler.compile(fragment));
                        break;
                    case CHTL_JS:
                        CHTLJSLexer chtljsLexer = new CHTLJSLexer(fragment.content());
                        CHTLJSParser chtljsParser = new CHTLJSParser(chtljsLexer.tokenize());
                        List<CHTLJSBaseNode> chtljsAst = chtljsParser.parse();
                        jsOutputs.add(chtljsGenerator.generate(chtljsAst));
                        break;
                }
            }

            // 3. Post-Processing CHTL AST
            // This is the critical step that was missing.
            // It resolves templates and evaluates all style expressions.
            CHTL.Processor.ASTProcessor processor = new CHTL.Processor.ASTProcessor(chtlAst, definitionManager);
            List<BaseNode> processedAst = processor.process();

            // 4. Merging
            CodeMerger merger = new CodeMerger();
            String finalHtml = merger.merge(processedAst, cssOutputs, jsOutputs);

            System.out.println("--- Final HTML Output ---");
            System.out.println(finalHtml);

        } catch (Exception e) {
            System.err.println("\n--- COMPILATION FAILED ---");
            e.printStackTrace();
        }
    }
}

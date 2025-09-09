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
        String source = "style { body { background-color: #eee; } }\n" +
                        "div {\n" +
                        "    id: main-box;\n" +
                        "    text: \"Click the button!\";\n" +
                        "    script {\n" +
                        "        {{#main-box}} -> listen { click: () => { alert('Hello from CHTL JS!'); } };\n" +
                        "    }\n" +
                        "}\n" +
                        "script { console.log('Global JS executed.'); }";

        System.out.println("--- Source Code ---\n" + source + "\n");

        try {
            // 1. Scanning
            CHTLUnifiedScanner scanner = new CHTLUnifiedScanner(source);
            List<SourceFragment> fragments = scanner.scan();

            // 2. Compilation
            List<BaseNode> chtlAst = new ArrayList<>();
            List<String> cssOutputs = new ArrayList<>();
            List<String> jsOutputs = new ArrayList<>();

            CHTLCompiler chtlCompiler = new CHTLCompiler();
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

            // 3. Merging
            CodeMerger merger = new CodeMerger();
            String finalHtml = merger.merge(chtlAst, cssOutputs, jsOutputs);

            System.out.println("--- Final HTML Output ---");
            System.out.println(finalHtml);

        } catch (Exception e) {
            System.err.println("\n--- COMPILATION FAILED ---");
            e.printStackTrace();
        }
    }
}

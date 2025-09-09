package com.chtholly;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.generator.CHTLGenerator;
import com.chtholly.chthl.generator.CompilationResult;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.parser.CHTLParser;
import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class CompilerEndToEndTest {

    @Test
    void testFullCompilation() {
        String chtlSource = """
        html {
            head {
                title { text { "My CHTL Page" } }
            }
            body {
                div {
                    id: main;
                    class: "container";
                    h1 { text { "Welcome!" } }
                    p { text { "This is a paragraph." } }
                    br {}
                }
            }
        }
        """;

        String expectedHtml = "<html><head><title>My CHTL Page</title></head>" +
                              "<body><div class=\"container\" id=\"main\">" +
                              "<h1>Welcome!</h1><p>This is a paragraph.</p>" +
                              "<br></div></body></html>";

        CHTLGenerator generator = new CHTLGenerator();
        CompilationResult result = generator.generate(new CHTLParser(new CHTLLexer(chtlSource).scanTokens()).parse());

        assertEquals(expectedHtml, result.getHtml());
        assertTrue(result.getCss().isEmpty());
    }

    @Test
    void testFullCompilationWithStyles() {
        String chtlSource = """
        div {
            id: main;
            style {
                color: red;
                .box {
                    font-weight: 700;
                }
            }
            h1 { text { "Styled" } }
        }
        """;

        // Attributes are sorted: class, then id, then style
        String expectedHtml = "<div class=\"box\" id=\"main\" style=\"color:red;\"><h1>Styled</h1></div>";
        String expectedCss = ".box {\n    font-weight: 700;\n}\n";

        CHTLGenerator generator = new CHTLGenerator();
        CompilationResult result = generator.generate(new CHTLParser(new CHTLLexer(chtlSource).scanTokens()).parse());

        assertEquals(expectedHtml, result.getHtml());
        assertEquals(expectedCss.trim(), result.getCss().trim());
    }

    @Test
    void testAutoAttributeAndAmpersandResolution() {
        String chtlSource = """
        div {
            // This div has no class attribute initially.
            // The .box selector should add it automatically.
            style {
                .box {
                    color: blue;
                }
                &:hover {
                    color: red;
                }
            }
        }
        """;

        // The class="box" should be added automatically.
        String expectedHtml = "<div class=\"box\"></div>";
        // The & should be resolved to .box
        String expectedCss = ".box {\n    color: blue;\n}\n.box:hover {\n    color: red;\n}\n";

        CHTLGenerator generator = new CHTLGenerator();
        CompilationResult result = generator.generate(new CHTLParser(new CHTLLexer(chtlSource).scanTokens()).parse());

        assertEquals(expectedHtml, result.getHtml());
        assertEquals(expectedCss.trim(), result.getCss().trim());
    }
}

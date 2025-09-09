package com.chtholly;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.generator.CHTLGenerator;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.parser.CHTLParser;
import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;

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

        // Note: A real HTML generator would add newlines and indentation for readability.
        // For testing, a compact string is easier to assert against.
        String expectedHtml = "<html><head><title>My CHTL Page</title></head>" +
                              "<body><div id=\"main\" class=\"container\">" +
                              "<h1>Welcome!</h1><p>This is a paragraph.</p>" +
                              "<br></div></body></html>";

        // 1. Lexer
        CHTLLexer lexer = new CHTLLexer(chtlSource);
        List<Token> tokens = lexer.scanTokens();

        // 2. Parser
        CHTLParser parser = new CHTLParser(tokens);
        List<Node> ast = parser.parse();

        // 3. Generator
        CHTLGenerator generator = new CHTLGenerator();
        String actualHtml = generator.generate(ast);

        // 4. Assert
        assertEquals(expectedHtml, actualHtml);
    }
}

package com.chtholly;

import com.chtholly.chthl.generator.CHTLGenerator;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.node.DocumentNode;
import com.chtholly.chthl.parser.CHTLParser;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class CHTLGeneratorTest {

    @Test
    void testHtmlGeneration() {
        String input = """
                html {
                    body {
                        h1 {
                            text { "This is a heading" }
                        }
                        div {
                            text { "Hello, World!" }
                        }
                    }
                }
                """;

        String expectedHtml = "<html><body><h1>This is a heading</h1><div>Hello, World!</div></body></html>";

        CHTLLexer lexer = new CHTLLexer(input);
        CHTLParser parser = new CHTLParser(lexer);
        DocumentNode document = parser.parseDocument();

        // Ensure the parser worked correctly before testing the generator
        assertTrue(parser.getErrors().isEmpty(), "Parser should not have errors for valid input.");

        CHTLGenerator generator = new CHTLGenerator();
        String actualHtml = generator.generate(document);

        assertEquals(expectedHtml, actualHtml, "The generated HTML does not match the expected output.");
    }

    @Test
    void testEmptyGeneration() {
        String input = "";
        String expectedHtml = "";

        CHTLLexer lexer = new CHTLLexer(input);
        CHTLParser parser = new CHTLParser(lexer);
        DocumentNode document = parser.parseDocument();
        CHTLGenerator generator = new CHTLGenerator();
        String actualHtml = generator.generate(document);

        assertEquals(expectedHtml, actualHtml);
    }
}

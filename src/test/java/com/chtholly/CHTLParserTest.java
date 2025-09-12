package com.chtholly;

import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.node.DocumentNode;
import com.chtholly.chthl.node.ElementNode;
import com.chtholly.chthl.node.Node;
import com.chtholly.chthl.node.TextNode;
import com.chtholly.chthl.parser.CHTLParser;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

public class CHTLParserTest {

    @Test
    void testSimpleParsing() {
        String input = """
                html {
                    body {
                        div {
                            text { "Hello Parser" }
                        }
                    }
                }
                """;

        CHTLLexer lexer = new CHTLLexer(input);
        CHTLParser parser = new CHTLParser(lexer);
        DocumentNode document = parser.parseDocument();

        // Check for parsing errors
        assertTrue(parser.getErrors().isEmpty(), "Parser should not have errors.");
        assertNotNull(document, "parseDocument() should not return null.");

        // Document -> html
        assertEquals(1, document.getChildren().size());
        Node htmlNode = document.getChildren().get(0);
        assertInstanceOf(ElementNode.class, htmlNode, "Expected html node to be an ElementNode.");
        assertEquals("html", ((ElementNode) htmlNode).getTagName());

        // html -> body
        ElementNode htmlElement = (ElementNode) htmlNode;
        assertEquals(1, htmlElement.getChildren().size());
        Node bodyNode = htmlElement.getChildren().get(0);
        assertInstanceOf(ElementNode.class, bodyNode, "Expected body node to be an ElementNode.");
        assertEquals("body", ((ElementNode) bodyNode).getTagName());

        // body -> div
        ElementNode bodyElement = (ElementNode) bodyNode;
        assertEquals(1, bodyElement.getChildren().size());
        Node divNode = bodyElement.getChildren().get(0);
        assertInstanceOf(ElementNode.class, divNode, "Expected div node to be an ElementNode.");
        assertEquals("div", ((ElementNode) divNode).getTagName());

        // div -> text
        ElementNode divElement = (ElementNode) divNode;
        assertEquals(1, divElement.getChildren().size());
        Node textNode = divElement.getChildren().get(0);
        assertInstanceOf(TextNode.class, textNode, "Expected text node to be a TextNode.");
        assertEquals("Hello Parser", ((TextNode) textNode).getValue());
    }
}

package com.chtholly;

import com.chtholly.chthl.ast.ElementNode;
import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.ast.StyleBlockNode;
import com.chtholly.chthl.ast.TextNode;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.parser.CHTLParser;
import org.junit.jupiter.api.Test;

import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

import static org.junit.jupiter.api.Assertions.*;

class CHTLParserTest {

    @Test
    void testSimpleElementParsing() {
        String input = "html { body {} }";
        List<Token> tokens = new CHTLLexer(input).scanTokens();
        CHTLParser parser = new CHTLParser(tokens);
        List<Node> ast = parser.parse().stream().filter(Objects::nonNull).collect(Collectors.toList());

        assertNotNull(ast);
        assertEquals(1, ast.size());

        Node root = ast.get(0);
        assertTrue(root instanceof ElementNode);
        ElementNode htmlNode = (ElementNode) root;
        assertEquals("html", htmlNode.tagName);
        assertEquals(1, htmlNode.children.stream().filter(Objects::nonNull).collect(Collectors.toList()).size());

        Node body = htmlNode.children.get(0);
        assertTrue(body instanceof ElementNode);
        ElementNode bodyNode = (ElementNode) body;
        assertEquals("body", bodyNode.tagName);
        assertTrue(bodyNode.children.isEmpty());
    }

    @Test
    void testTextNodeParsing() {
        String input = "text { \"hello world\" }";
        List<Token> tokens = new CHTLLexer(input).scanTokens();
        CHTLParser parser = new CHTLParser(tokens);
        List<Node> ast = parser.parse().stream().filter(Objects::nonNull).collect(Collectors.toList());

        assertNotNull(ast);
        assertEquals(1, ast.size());

        Node root = ast.get(0);
        assertTrue(root instanceof TextNode);
        TextNode textNode = (TextNode) root;
        assertEquals("hello world", textNode.text);
    }

    @Test
    void testMixedContentParsing() {
        String input = "div { text { \"I am a div\" } }";
        List<Token> tokens = new CHTLLexer(input).scanTokens();
        CHTLParser parser = new CHTLParser(tokens);
        List<Node> ast = parser.parse().stream().filter(Objects::nonNull).collect(Collectors.toList());

        assertNotNull(ast);
        assertEquals(1, ast.size());

        Node root = ast.get(0);
        assertTrue(root instanceof ElementNode);
        ElementNode divNode = (ElementNode) root;
        assertEquals("div", divNode.tagName);
        assertEquals(1, divNode.children.stream().filter(Objects::nonNull).collect(Collectors.toList()).size());

        Node text = divNode.children.get(0);
        assertTrue(text instanceof TextNode);
        TextNode textNode = (TextNode) text;
        assertEquals("I am a div", textNode.text);
    }

    @Test
    void testElementWithAttributes() {
        String input = "div { id: box; class: 'container'; }";
        List<Token> tokens = new CHTLLexer(input).scanTokens();
        CHTLParser parser = new CHTLParser(tokens);
        List<Node> ast = parser.parse();

        assertEquals(1, ast.size());
        assertTrue(ast.get(0) instanceof ElementNode);

        ElementNode node = (ElementNode) ast.get(0);
        assertEquals("div", node.tagName);
        assertEquals(2, node.attributes.size());
        assertEquals("box", node.attributes.get("id"));
        assertEquals("container", node.attributes.get("class"));
        assertTrue(node.children.isEmpty());
    }

    @Test
    void testStyleBlockParsing() {
        String input = "div { style { color: red; font-size: 16px; } }";
        List<Token> tokens = new CHTLLexer(input).scanTokens();
        CHTLParser parser = new CHTLParser(tokens);
        List<Node> ast = parser.parse();

        assertEquals(1, ast.size());
        assertTrue(ast.get(0) instanceof ElementNode);

        ElementNode node = (ElementNode) ast.get(0);
        assertEquals("div", node.tagName);
        assertEquals(1, node.children.size());

        Node child = node.children.get(0);
        assertTrue(child instanceof StyleBlockNode);

        StyleBlockNode styleNode = (StyleBlockNode) child;
        assertEquals(2, styleNode.properties.size());
        assertEquals("red", styleNode.properties.get("color"));
        assertEquals("16px", styleNode.properties.get("font-size"));
    }
}

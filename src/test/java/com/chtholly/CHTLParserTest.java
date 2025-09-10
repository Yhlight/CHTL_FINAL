package com.chtholly;

import com.chtholly.chthl.ast.*;
import com.chtholly.chthl.ast.expr.CallExpr;
import com.chtholly.chthl.ast.expr.ReferenceExpr;
import com.chtholly.chthl.ast.expr.VariableExpr;
import com.chtholly.chthl.ast.custom.CustomizationBlockNode;
import com.chtholly.chthl.ast.custom.DeleteNode;
import com.chtholly.chthl.ast.custom.InsertNode;
import com.chtholly.chthl.ast.template.StyleTemplateNode;
import com.chtholly.chthl.ast.template.TemplateNode;
import com.chtholly.chthl.ast.template.TemplateUsageNode;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;
import com.chtholly.chthl.parser.CHTLParser;
import org.junit.jupiter.api.Test;

import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.stream.Collectors;

import static org.junit.jupiter.api.Assertions.*;

class CHTLParserTest {

    @Test
    void testSimpleElementParsing() {
        String input = "html { body {} }";
        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        List<Node> ast = parser.getAst().stream().filter(Objects::nonNull).collect(Collectors.toList());
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
        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        List<Node> ast = parser.getAst().stream().filter(Objects::nonNull).collect(Collectors.toList());
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
        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        List<Node> ast = parser.getAst().stream().filter(Objects::nonNull).collect(Collectors.toList());
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
        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        List<Node> ast = parser.getAst();
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
        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        List<Node> ast = parser.getAst();
        assertEquals(1, ast.size());
        assertTrue(ast.get(0) instanceof ElementNode);
        ElementNode node = (ElementNode) ast.get(0);
        assertEquals("div", node.tagName);
        assertEquals(1, node.children.size());
        Node child = node.children.get(0);
        assertTrue(child instanceof StyleBlockNode);
        StyleBlockNode styleNode = (StyleBlockNode) child;
        assertEquals(2, styleNode.directPropertiesAndUsages.size());
        StylePropertyNode prop1 = (StylePropertyNode) styleNode.directPropertiesAndUsages.get(0);
        assertEquals("color", prop1.key);
        assertTrue(prop1.value instanceof VariableExpr);
        assertEquals("red", ((VariableExpr) prop1.value).name.getLexeme());
        StylePropertyNode prop2 = (StylePropertyNode) styleNode.directPropertiesAndUsages.get(1);
        assertEquals("font-size", prop2.key);
        assertTrue(prop2.value instanceof VariableExpr);
        assertEquals("16px", ((VariableExpr) prop2.value).name.getLexeme());
    }

    @Test
    void testSelectorBlockParsing() {
        String input = "div { style { color: white; .box { background-color: black; } font-size: 16px; } }";
        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        List<Node> ast = parser.getAst();
        assertEquals(1, ast.size());
        ElementNode div = (ElementNode) ast.get(0);
        assertEquals(1, div.children.size());
        StyleBlockNode styleNode = (StyleBlockNode) div.children.get(0);
        assertEquals(2, styleNode.directPropertiesAndUsages.size());
        StylePropertyNode prop1 = (StylePropertyNode) styleNode.directPropertiesAndUsages.get(0);
        assertEquals("color", prop1.key);
        assertTrue(prop1.value instanceof VariableExpr);
        assertEquals("white", ((VariableExpr) prop1.value).name.getLexeme());
        StylePropertyNode prop2 = (StylePropertyNode) styleNode.directPropertiesAndUsages.get(1);
        assertEquals("font-size", prop2.key);
        assertTrue(prop2.value instanceof VariableExpr);
        assertEquals("16px", ((VariableExpr) prop2.value).name.getLexeme());
        assertEquals(1, styleNode.selectorBlocks.size());
        SelectorBlockNode selectorNode = styleNode.selectorBlocks.get(0);
        assertEquals(".box", selectorNode.selector);
        assertEquals(1, selectorNode.body.size());
        assertTrue(selectorNode.body.get(0) instanceof StylePropertyNode);
        StylePropertyNode prop = (StylePropertyNode) selectorNode.body.get(0);
        assertEquals("background-color", prop.key);
        assertTrue(prop.value instanceof VariableExpr);
        assertEquals("black", ((VariableExpr) prop.value).name.getLexeme());
    }

    @Test
    void testReferenceExpressionParsing() {
        String input = "div { style { width: .box.width; } }";
        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        List<Node> ast = parser.getAst();
        ElementNode div = (ElementNode) ast.get(0);
        StyleBlockNode styleNode = (StyleBlockNode) div.children.get(0);
        StylePropertyNode propNode = (StylePropertyNode) styleNode.directPropertiesAndUsages.get(0);
        assertEquals("width", propNode.key);
        assertTrue(propNode.value instanceof ReferenceExpr);
        ReferenceExpr refExpr = (ReferenceExpr) propNode.value;
        assertEquals("width", refExpr.property.getLexeme());
        assertTrue(refExpr.object instanceof VariableExpr);
        VariableExpr selectorExpr = (VariableExpr) refExpr.object;
        assertEquals(".box", selectorExpr.name.getLexeme());
    }

    @Test
    void testTemplateDefinitionParsing() {
        String input = "[Template] @Style DefaultText { color: black; line-height: 1.6; }";
        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        parser.getAst(); // This runs the parsing
        Map<String, TemplateNode> table = parser.getTemplateTable();
        assertEquals(1, table.size());
        assertTrue(table.containsKey("DefaultText"));
        TemplateNode node = table.get("DefaultText");
        assertTrue(node instanceof StyleTemplateNode);
        StyleTemplateNode styleTemplate = (StyleTemplateNode) node;
        assertEquals(2, styleTemplate.body.size());
        assertTrue(styleTemplate.body.get(0) instanceof StylePropertyNode);
        StylePropertyNode prop = (StylePropertyNode) styleTemplate.body.get(0);
        assertEquals("color", prop.key);
    }

    @Test
    void testTemplateUsageParsing() {
        String input = """
        body {
            @Element Box;
            style {
                @Style DefaultText;
                color: ThemeColor(primary);
            }
        }
        """;
        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        List<Node> ast = parser.getAst();

        // Check @Element usage
        ElementNode body = (ElementNode) ast.get(0);
        assertTrue(body.children.get(0) instanceof TemplateUsageNode);
        TemplateUsageNode elementUsage = (TemplateUsageNode) body.children.get(0);
        assertEquals("@Element", elementUsage.type.getLexeme());
        assertEquals("Box", elementUsage.name.getLexeme());

        // Check @Style usage
        StyleBlockNode styleNode = (StyleBlockNode) body.children.get(1);
        assertTrue(styleNode.directPropertiesAndUsages.get(0) instanceof TemplateUsageNode);
        TemplateUsageNode styleUsage = (TemplateUsageNode) styleNode.directPropertiesAndUsages.get(0);
        assertEquals("@Style", styleUsage.type.getLexeme());
        assertEquals("DefaultText", styleUsage.name.getLexeme());

        // Check @Var usage (CallExpr)
        StylePropertyNode propNode = (StylePropertyNode) styleNode.directPropertiesAndUsages.get(1);
        assertTrue(propNode.value instanceof CallExpr);
        CallExpr callExpr = (CallExpr) propNode.value;
        assertTrue(callExpr.callee instanceof VariableExpr);
        assertEquals("ThemeColor", ((VariableExpr) callExpr.callee).name.getLexeme());
        assertEquals(1, callExpr.arguments.size());
        assertTrue(callExpr.arguments.get(0) instanceof VariableExpr);
        assertEquals("primary", ((VariableExpr) callExpr.arguments.get(0)).name.getLexeme());
    }

    @Test
    void testCustomizationBlockParsing() {
        String input = """
        body {
            @Element Box {
                delete span;
                insert after div[0] {
                    p { text { "Inserted" } }
                }
            }
        }
        """;

        CHTLParser parser = new CHTLParser(new CHTLLexer(input).scanTokens());
        List<Node> ast = parser.getAst();

        ElementNode body = (ElementNode) ast.get(0);
        TemplateUsageNode usage = (TemplateUsageNode) body.children.get(0);
        assertNotNull(usage.customization);

        CustomizationBlockNode custom = usage.customization;
        assertEquals(2, custom.modifications.size());

        // Check delete node
        assertTrue(custom.modifications.get(0) instanceof DeleteNode);
        DeleteNode deleteNode = (DeleteNode) custom.modifications.get(0);
        assertEquals(1, deleteNode.targets.size());
        assertEquals("span", deleteNode.targets.get(0).get(0).getLexeme());

        // Check insert node
        assertTrue(custom.modifications.get(1) instanceof InsertNode);
        InsertNode insertNode = (InsertNode) custom.modifications.get(1);
        assertEquals("after", insertNode.position.getLexeme());
        assertEquals("div[0]", insertNode.target.stream().map(Token::getLexeme).collect(Collectors.joining("")));
        assertEquals(1, insertNode.body.size());
        assertTrue(insertNode.body.get(0) instanceof ElementNode);
    }
}

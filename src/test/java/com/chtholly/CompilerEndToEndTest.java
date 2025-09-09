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

    private CompilationResult compile(String source) {
        CHTLParser parser = new CHTLParser(new CHTLLexer(source).scanTokens());
        List<Node> ast = parser.getAst();
        CHTLGenerator generator = new CHTLGenerator(ast, parser.getTemplateTable());
        return generator.generate();
    }

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

        CompilationResult result = compile(chtlSource);
        assertEquals(expectedHtml, result.getHtml());
        assertTrue(result.getCss().isEmpty());
    }

    @Test
    void testVariableTemplateExpansion() {
        String chtlSource = """
        [Template] @Var Theme {
            primaryColor: #007bff;
            secondaryColor: #6c757d;
        }
        div {
            style {
                background-color: Theme(primaryColor);
                color: Theme(secondaryColor);
            }
        }
        """;
        String expectedHtml = "<div style=\"background-color:#007bff;color:#6c757d;\"></div>";
        CompilationResult result = compile(chtlSource);
        assertEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testConditionalExpressionEvaluation() {
        String chtlSource = """
        div {
            id: box;
            style {
                width: 100px;
                background-color: width > 50px ? 'green' : 'red';
            }
        }
        """;
        String expectedHtml = "<div id=\"box\" style=\"width:100px;background-color:green;\"></div>";

        CompilationResult result = compile(chtlSource);
        assertEquals(expectedHtml, result.getHtml());
        assertTrue(result.getCss().isEmpty());
    }

    @Test
    void testPropertyReferenceEvaluation() {
        String chtlSource = """
        div { id: box1; style { width: 100px; } }
        div { id: box2; style { width: #box1.width + 50px; } }
        """;
        String expectedHtml = "<div id=\"box1\" style=\"width:100px;\"></div>" +
                              "<div id=\"box2\" style=\"width:150px;\"></div>";

        CompilationResult result = compile(chtlSource);
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
                .box { font-weight: 700; }
            }
            h1 { text { "Styled" } }
        }
        """;
        String expectedHtml = "<div class=\"box\" id=\"main\" style=\"color:red;\"><h1>Styled</h1></div>";
        String expectedCss = ".box {\n    font-weight: 700;\n}\n";

        CompilationResult result = compile(chtlSource);
        assertEquals(expectedHtml, result.getHtml());
        assertEquals(expectedCss.trim(), result.getCss().trim());
    }

    @Test
    void testAutoAttributeAndAmpersandResolution() {
        String chtlSource = """
        div {
            style {
                .box { color: blue; }
                &:hover { color: red; }
            }
        }
        """;
        String expectedHtml = "<div class=\"box\"></div>";
        String expectedCss = ".box {\n    color: blue;\n}\n.box:hover {\n    color: red;\n}\n";

        CompilationResult result = compile(chtlSource);
        assertEquals(expectedHtml, result.getHtml());
        assertEquals(expectedCss.trim(), result.getCss().trim());
    }

    @Test
    void testStyleWithBinaryExpression() {
        String chtlSource = "div { style { width: 100px + 50px; } }";
        String expectedHtml = "<div style=\"width:150px;\"></div>";

        CompilationResult result = compile(chtlSource);
        assertEquals(expectedHtml, result.getHtml());
        assertTrue(result.getCss().isEmpty());
    }

    @Test
    void testElementTemplateExpansion() {
        String chtlSource = """
        [Template] @Element Box {
            h1 { text { "Title" } }
            p { text { "Content" } }
        }
        body {
            @Element Box;
        }
        """;
        String expectedHtml = "<body><h1>Title</h1><p>Content</p></body>";
        CompilationResult result = compile(chtlSource);
        assertEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testStyleTemplateExpansion() {
        String chtlSource = """
        [Template] @Style DefaultText {
            font-family: sans-serif;
            font-size: 16px;
        }
        div {
            style {
                @Style DefaultText;
                color: blue;
            }
        }
        """;
        String expectedHtml = "<div style=\"font-family:sans-serif;font-size:16px;color:blue;\"></div>";
        CompilationResult result = compile(chtlSource);
        assertEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testStyleTemplateInheritance() {
        String chtlSource = """
        [Template] @Style Base {
            color: red;
            font-size: 16px;
        }
        [Template] @Style Derived {
            @Style Base;
            font-size: 20px;
            font-weight: 700;
        }
        div {
            style {
                @Style Derived;
            }
        }
        """;
        String expectedHtml = "<div style=\"color:red;font-size:16px;font-size:20px;font-weight:700;\"></div>";
        CompilationResult result = compile(chtlSource);
        assertEquals(expectedHtml, result.getHtml());
    }
}

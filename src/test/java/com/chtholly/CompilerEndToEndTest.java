package com.chtholly;

import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.generator.CHTLGenerator;
import com.chtholly.chthl.generator.CompilationResult;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.parser.CHTLParser;
import org.junit.jupiter.api.Test;

import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import com.chtholly.chthl.CHTLConfig;

class CompilerEndToEndTest {

    private CompilationResult compile(String source) {
        CHTLParser parser = new CHTLParser(new CHTLLexer(source).scanTokens(), null);
        List<Node> ast = parser.getAst();

        CHTLConfig config = new CHTLConfig();
        config.load(parser.getConfiguration());

        CHTLGenerator generator = new CHTLGenerator(ast, parser.getTemplateTable(), parser.getOriginTable(), config);
        return generator.generate();
    }

    private String normalizeStyle(String html) {
        Pattern stylePattern = Pattern.compile("style=\"([^\"]*)\"");
        Matcher matcher = stylePattern.matcher(html);
        StringBuffer sb = new StringBuffer();
        while (matcher.find()) {
            String styleContent = matcher.group(1);
            String normalized = styleContent.replaceAll("\\s*;\\s*", ";").replaceAll("\\s*:\\s*", ":").replaceAll("\\s+", " ");
            if (normalized.endsWith(";")) {
                normalized = normalized.substring(0, normalized.length() - 1);
            }
            matcher.appendReplacement(sb, "style=\"" + normalized + "\"");
        }
        matcher.appendTail(sb);
        return sb.toString();
    }

    private void assertHtmlEquals(String expected, String actual) {
        assertEquals(normalizeStyle(expected), normalizeStyle(actual));
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
        assertHtmlEquals(expectedHtml, result.getHtml());
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
        String expectedHtml = "<div style=\"background-color:#007bff;color:#6c757d\"></div>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
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
        String expectedHtml = "<div id=\"box\" style=\"width:100px;background-color:green\"></div>";

        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
        assertTrue(result.getCss().isEmpty());
    }

    @Test
    void testPropertyReferenceEvaluation() {
        String chtlSource = """
        div { id: box1; style { width: 100px; } }
        div { id: box2; style { width: #box1.width + 50px; } }
        """;
        String expectedHtml = "<div id=\"box1\" style=\"width:100px\"></div>" +
                              "<div id=\"box2\" style=\"width:150px\"></div>";

        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
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
        String expectedHtml = "<div class=\"box\" id=\"main\" style=\"color:red\"><h1>Styled</h1></div>";
        String expectedCss = ".box {\n    font-weight: 700;\n}\n";

        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
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
        assertHtmlEquals(expectedHtml, result.getHtml());
        assertEquals(expectedCss.trim(), result.getCss().trim());
    }

    @Test
    void testStyleWithBinaryExpression() {
        String chtlSource = "div { style { width: 100px + 50px; } }";
        String expectedHtml = "<div style=\"width:150px\"></div>";

        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
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
        assertHtmlEquals(expectedHtml, result.getHtml());
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
        String expectedHtml = "<div style=\"font-family:sans-serif;font-size:16px;color:blue\"></div>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
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
        String expectedHtml = "<div style=\"color:red;font-size:16px;font-size:20px;font-weight:700\"></div>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testDeleteSpecialization() {
        String chtlSource = """
        [Template] @Element MyComponent {
            h1 { text { "Title" } }
            span { text { "I should be deleted." } }
            div {
                style {
                    color: red;
                    font-size: 16px; // This should be deleted
                }
            }
        }

        body {
            @Element MyComponent {
                delete span;
                delete font-size;
            }
        }
        """;

        String expectedHtml = "<body><h1>Title</h1><div style=\"color:red\"></div></body>";

        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testInsertSpecialization() {
        String chtlSource = """
        [Template] @Element MyComponent {
            div { id: first; }
            div { id: second; }
        }

        body {
            @Element MyComponent {
                insert after div[0] {
                    p { text { "Inserted" } }
                }
            }
        }
        """;
        String expectedHtml = "<body><div id=\"first\"></div><p>Inserted</p><div id=\"second\"></div></body>";

        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testIndexedDeleteSpecialization() {
        String chtlSource = """
        [Template] @Element MyComponent {
            p { text { "p1" } }
            p { text { "p2 should be deleted" } }
            p { text { "p3" } }
        }
        body {
            @Element MyComponent {
                delete p[1];
            }
        }
        """;
        String expectedHtml = "<body><p>p1</p><p>p3</p></body>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testInheritanceDeleteSpecialization() {
        String chtlSource = """
        [Template] @Style Base { color: red; }
        [Template] @Style Derived {
            @Style Base;
            font-weight: 700;
        }
        div {
            style {
                @Style Derived {
                    delete @Style Base;
                }
            }
        }
        """;
        String expectedHtml = "<div style=\"font-weight:700\"></div>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testInsertIntoElementSpecialization() {
        String chtlSource = """
        [Template] @Element Container {
            div { id: box; }
        }

        body {
            @Element Container {
                insert into div[0] {
                    p { text { "Nested" } }
                }
            }
        }
        """;
        String expectedHtml = "<body><div id=\"box\"><p>Nested</p></div></body>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testInsertIntoStyleSpecialization() {
        String chtlSource = """
        [Template] @Element MyComponent {
            div {
                style {
                    color: red;
                }
            }
        }

        body {
            @Element MyComponent {
                insert into style {
                    font-size: 22px;
                }
            }
        }
        """;
        String expectedHtml = "<body><div style=\"color:red;font-size:22px\"></div></body>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testReplaceElementSpecialization() {
        String chtlSource = """
        [Template] @Element MyComponent {
            h1 { text { "Title" } }
            p { id: old; text { "Old paragraph" } }
        }

        body {
            @Element MyComponent {
                insert replace p[0] {
                    div { id: new; text { "New paragraph" } }
                }
            }
        }
        """;
        String expectedHtml = "<body><h1>Title</h1><div id=\"new\">New paragraph</div></body>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testSetVariableSpecialization() {
        String chtlSource = """
        [Template] @Var Theme {
            width: 100px;
            height: 200px;
            padding: 5px;
        }

        [Template] @Element Box {
            div {
                style {
                    width: &width;
                    height: &height;
                    padding: &padding;
                }
            }
        }

        // Default usage
        @Element Box;

        // Specialized usage
        @Element Box {
            set width = 500px;
            set height = &width / 2;
        }
        """;
        String expectedHtml = "<div style=\"width:100px;height:200px;padding:5px\"></div>" +
                              "<div style=\"width:500px;height:250px;padding:5px\"></div>";

        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testChainedConditionalExpression() {
        String chtlSource = """
        div {
            style {
                width: 150px;
                height: 80px;
                // The first true condition should be chosen
                color: height < 50px ? "blue", width > 100px ? "green" : "red";
            }
        }
        """;
        String expectedHtml = "<div style=\"width:150px;height:80px;color:green\"></div>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testOriginBlock() {
        String chtlSource = """
        [Origin] @Html MyRawHtml {
            "<div>This is raw</div>"
        }

        body {
            p { text { "Before" } }
            [Origin] @Html MyRawHtml;
            p { text { "After" } }
        }
        """;
        String expectedHtml = "<body><p>Before</p><div>This is raw</div><p>After</p></body>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }

    @Test
    void testConfigurationIndexInitialCount() {
        String chtlSource = """
        [Configuration] {
            INDEX_INITIAL_COUNT = 1;
        }

        [Template] @Element MyList {
            p { text { "Item 1" } }
            p { text { "Item 2" } }
            p { text { "Item 3" } }
        }

        body {
            // Index 1 should now be the first paragraph
            @Element MyList {
                delete p[1];
            }
        }
        """;
        String expectedHtml = "<body><p>Item 2</p><p>Item 3</p></body>";
        CompilationResult result = compile(chtlSource);
        assertHtmlEquals(expectedHtml, result.getHtml());
    }
}

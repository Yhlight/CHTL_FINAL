package com.chtholly;

import com.chtholly.chthl.CHTLConfig;
import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.generator.CHTLGenerator;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.parser.CHTLParser;
import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

public class CHTLConstraintTest {

    private void assertCompilationFails(String source, String expectedErrorMessage) {
        CHTLParser parser = new CHTLParser(new CHTLLexer(source).scanTokens(), null);
        List<Node> ast = parser.getAst();
        CHTLGenerator generator = new CHTLGenerator(ast, parser.getTemplateTable(), parser.getOriginTable(), new CHTLConfig());

        CHTLGenerator.CompilationError thrown = assertThrows(
            CHTLGenerator.CompilationError.class,
            generator::generate,
            "Expected compilation to fail, but it succeeded."
        );
        assertTrue(thrown.getMessage().contains(expectedErrorMessage));
    }

    @Test
    void testPreciseTagConstraint() {
        String chtlSource = """
        div {
            except span;
            p { text { "Allowed" } }
        }
        """;
        // This should compile without error
        assertDoesNotThrow(() -> {
            CHTLParser parser = new CHTLParser(new CHTLLexer(chtlSource).scanTokens(), null);
            new CHTLGenerator(parser.getAst(), parser.getTemplateTable(), parser.getOriginTable(), new CHTLConfig()).generate();
        });

        String failingSource = """
        div {
            except span;
            span { text { "Not Allowed" } }
        }
        """;
        assertCompilationFails(failingSource, "usage of 'span' is forbidden");
    }

    @Test
    void testHtmlTypeConstraint() {
        String failingSource = """
        div {
            except @Html;
            p { text { "Not Allowed" } }
        }
        """;
        assertCompilationFails(failingSource, "usage of HTML elements is forbidden");
    }

    @Test
    void testTemplateTypeConstraint() {
         String chtlSource = """
        [Template] @Style MyStyle { color: red; }

        div {
            except [Template];
            @Style MyStyle;
        }
        """;
        assertCompilationFails(chtlSource, "usage of [Template] is forbidden");
    }
}

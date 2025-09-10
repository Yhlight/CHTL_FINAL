package com.chtholly;

import com.chtholly.chthl.CHTLConfig;
import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.generator.CHTLGenerator;
import com.chtholly.chthl.generator.CompilationResult;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.parser.CHTLParser;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class CHTLNamespaceTest {
    private static final String TEMP_DIR = "temp_namespace_test_files";

    @BeforeAll
    static void setUp() throws IOException {
        Path tempPath = Paths.get(TEMP_DIR);
        if (Files.exists(tempPath)) {
            Files.walk(tempPath).sorted(Comparator.reverseOrder()).map(Path::toFile).forEach(File::delete);
        }
        Files.createDirectories(tempPath);
    }

    @AfterAll
    static void tearDown() throws IOException {
        Path tempPath = Paths.get(TEMP_DIR);
        Files.walk(tempPath).sorted(Comparator.reverseOrder()).map(Path::toFile).forEach(File::delete);
    }

    private CompilationResult compileFile(String filePath) throws IOException {
        String source = new String(Files.readAllBytes(Paths.get(filePath)), StandardCharsets.UTF_8);
        CHTLParser parser = new CHTLParser(new CHTLLexer(source).scanTokens(), filePath);
        List<Node> ast = parser.getAst();
        CHTLGenerator generator = new CHTLGenerator(ast, parser.getTemplateTable(), parser.getOriginTable(), new CHTLConfig());
        return generator.generate();
    }

    private String normalizeStyle(String html) {
        Pattern stylePattern = Pattern.compile("style=\\\"([^\\\"]*)\\\"");
        Matcher matcher = stylePattern.matcher(html);
        StringBuffer sb = new StringBuffer();
        while (matcher.find()) {
            String styleContent = matcher.group(1);
            String normalized = styleContent.replaceAll("\\s*;\\s*", ";").replaceAll("\\s*:\\s*", ":").replaceAll("\\s+", " ");
            if (normalized.endsWith(";")) {
                normalized = normalized.substring(0, normalized.length() - 1);
            }
            matcher.appendReplacement(sb, "style=\\\"" + normalized + "\\\"");
        }
        matcher.appendTail(sb);
        return sb.toString();
    }

    private void assertHtmlEquals(String expected, String actual) {
        assertEquals(normalizeStyle(expected), normalizeStyle(actual));
    }

    @Test
    void testScopedTemplateAccessWithFromClause() {
        String chtlSource = """
        [Namespace] MySpace {
            [Template] @Style GreenText {
                color: green;
            }
        }

        div {
            style {
                @Style GreenText from MySpace;
            }
        }
        """;
        CHTLParser parser = new CHTLParser(new CHTLLexer(chtlSource).scanTokens(), null);
        List<Node> ast = parser.getAst();
        CHTLGenerator generator = new CHTLGenerator(ast, parser.getTemplateTable(), parser.getOriginTable(), new CHTLConfig());
        CompilationResult result = generator.generate();

        assertHtmlEquals("<div style=\"color:green;\"></div>", result.getHtml());
    }

    @Test
    void testGlobalFallback() {
        String chtlSource = """
        [Template] @Style GlobalStyle {
            font-size: 20px;
        }

        [Namespace] MySpace {
            [Template] @Style LocalStyle {
                @Style GlobalStyle;
                color: blue;
            }
        }

        div {
            style {
                @Style LocalStyle from MySpace;
            }
        }
        """;
        CHTLParser parser = new CHTLParser(new CHTLLexer(chtlSource).scanTokens(), null);
        List<Node> ast = parser.getAst();
        CHTLGenerator generator = new CHTLGenerator(ast, parser.getTemplateTable(), parser.getOriginTable(), new CHTLConfig());
        CompilationResult result = generator.generate();

        assertHtmlEquals("<div style=\"font-size:20px;color:blue;\"></div>", result.getHtml());
    }

    @Test
    void testDefaultNamespaceOnImport() throws IOException {
        String importedContent = "[Template] @Style ImportedStyle { color: purple; }";
        Path importedPath = Paths.get(TEMP_DIR, "imports_for_default.chtl");
        Files.write(importedPath, importedContent.getBytes(StandardCharsets.UTF_8));

        String mainContent = """
        [Import] @Chtl from "./imports_for_default.chtl";
        div {
            style {
                @Style ImportedStyle from imports_for_default;
            }
        }
        """;
        Path mainPath = Paths.get(TEMP_DIR, "main_default.chtl");
        Files.write(mainPath, mainContent.getBytes(StandardCharsets.UTF_8));

        CompilationResult result = compileFile(mainPath.toString());
        assertHtmlEquals("<div style=\"color:purple;\"></div>", result.getHtml());
    }

    @Test
    void testExplicitNamespaceOnImport() throws IOException {
        String importedContent = """
        [Namespace] MyTheme {
            [Template] @Style ThemedBox { background-color: orange; }
        }
        """;
        Path importedPath = Paths.get(TEMP_DIR, "imports_for_explicit.chtl");
        Files.write(importedPath, importedContent.getBytes(StandardCharsets.UTF_8));

        String mainContent = """
        [Import] @Chtl from "./imports_for_explicit.chtl";
        div {
            style {
                @Style ThemedBox from MyTheme;
            }
        }
        """;
        Path mainPath = Paths.get(TEMP_DIR, "main_explicit.chtl");
        Files.write(mainPath, mainContent.getBytes(StandardCharsets.UTF_8));

        CompilationResult result = compileFile(mainPath.toString());
        assertHtmlEquals("<div style=\"background-color:orange;\"></div>", result.getHtml());
    }
}

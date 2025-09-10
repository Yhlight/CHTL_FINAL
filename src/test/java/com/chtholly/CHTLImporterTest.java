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

public class CHTLImporterTest {

    private static final String TEMP_DIR = "temp_importer_test_files";

    @BeforeAll
    static void setUp() throws IOException {
        // Create a temporary directory for test files
        Path tempPath = Paths.get(TEMP_DIR);
        if (Files.exists(tempPath)) {
            // Clean up old directory if it exists
            Files.walk(tempPath)
                 .sorted(Comparator.reverseOrder())
                 .map(Path::toFile)
                 .forEach(File::delete);
        }
        Files.createDirectories(tempPath);

        // Create the imported file
        String importedContent = "[Template] @Style ImportedStyle { color: green; font-size: 24px; }";
        Files.write(Paths.get(TEMP_DIR, "imports.chtl"), importedContent.getBytes(StandardCharsets.UTF_8));

        // Create the main file that imports the other file
        String mainContent = "[Import] @Chtl from \"./imports.chtl\";\n" +
                             "div { style { @Style ImportedStyle; } text { \"Hello Importer!\" } }";
        Files.write(Paths.get(TEMP_DIR, "main.chtl"), mainContent.getBytes(StandardCharsets.UTF_8));
    }

    @AfterAll
    static void tearDown() throws IOException {
        // Clean up the temporary directory
        Path tempPath = Paths.get(TEMP_DIR);
        Files.walk(tempPath)
             .sorted(Comparator.reverseOrder())
             .map(Path::toFile)
             .forEach(File::delete);
    }

    private CompilationResult compileFile(String filePath) throws IOException {
        String source = new String(Files.readAllBytes(Paths.get(filePath)), StandardCharsets.UTF_8);

        CHTLParser parser = new CHTLParser(new CHTLLexer(source).scanTokens(), filePath);
        List<Node> ast = parser.getAst();
        Map<String, com.chtholly.chthl.ast.template.TemplateNode> templateTable = parser.getTemplateTable();

        CHTLConfig config = new CHTLConfig();
        config.load(parser.getConfiguration());

        CHTLGenerator generator = new CHTLGenerator(ast, templateTable, parser.getOriginTable(), config);
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
    void testImportStyleTemplate() throws IOException {
        String mainFilePath = Paths.get(TEMP_DIR, "main.chtl").toString();

        CompilationResult result = compileFile(mainFilePath);

        String expectedHtml = "<div style=\"color:green;font-size:24px\">Hello Importer!</div>";
        assertHtmlEquals(expectedHtml, result.getHtml());
    }
}

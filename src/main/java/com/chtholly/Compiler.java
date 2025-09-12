package com.chtholly;

import com.chtholly.chthl.generator.CHTLGenerator;
import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.node.DocumentNode;
import com.chtholly.chthl.parser.CHTLParser;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

public class Compiler {
    public static void main(String[] args) {
        if (args.length == 0) {
            System.err.println("Error: No input file specified.");
            System.err.println("Usage: java -jar chtl-compiler.jar <filename.chtl>");
            System.exit(1);
        }

        String filename = args[0];
        try {
            String input = new String(Files.readAllBytes(Paths.get(filename)));

            CHTLLexer lexer = new CHTLLexer(input);
            CHTLParser parser = new CHTLParser(lexer);
            DocumentNode document = parser.parseDocument();

            List<String> errors = parser.getErrors();
            if (!errors.isEmpty()) {
                System.err.println("Parser encountered " + errors.size() + " errors:");
                for (String msg : errors) {
                    System.err.println("\t- " + msg);
                }
                System.exit(1);
            }

            CHTLGenerator generator = new CHTLGenerator();
            String output = generator.generate(document);

            System.out.println(output);

        } catch (java.io.IOException e) {
            System.err.println("Error reading file: " + filename);
            System.err.println(e.getMessage());
            System.exit(1);
        } catch (Exception e) {
            System.err.println("An unexpected error occurred during compilation:");
            e.printStackTrace();
            System.exit(1);
        }
    }
}

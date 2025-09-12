package com.chtholly;

import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class CHTLLexerTest {

    @Test
    void testNextToken() {
        String input = """
                // This is a comment
                html {
                    body {
                        div {
                            id: "main-div";
                            class = 'container';
                            text {
                                "Hello, CHTL!"
                            }
                        }
                        /* Another comment */
                    }
                }
                """;

        Token[] expectedTokens = new Token[]{
                new Token(TokenType.IDENTIFIER, "html", 2, 1),
                new Token(TokenType.LEFT_BRACE, "{", 2, 6),
                new Token(TokenType.IDENTIFIER, "body", 3, 5),
                new Token(TokenType.LEFT_BRACE, "{", 3, 10),
                new Token(TokenType.IDENTIFIER, "div", 4, 9),
                new Token(TokenType.LEFT_BRACE, "{", 4, 13),
                new Token(TokenType.IDENTIFIER, "id", 5, 13),
                new Token(TokenType.COLON, ":", 5, 15),
                new Token(TokenType.STRING, "main-div", 5, 17),
                new Token(TokenType.SEMICOLON, ";", 5, 28),
                new Token(TokenType.IDENTIFIER, "class", 6, 13),
                new Token(TokenType.EQUAL, "=", 6, 19),
                new Token(TokenType.STRING, "container", 6, 21),
                new Token(TokenType.SEMICOLON, ";", 6, 33),
                new Token(TokenType.IDENTIFIER, "text", 7, 13),
                new Token(TokenType.LEFT_BRACE, "{", 7, 18),
                new Token(TokenType.STRING, "Hello, CHTL!", 8, 17),
                new Token(TokenType.RIGHT_BRACE, "}", 9, 13),
                new Token(TokenType.RIGHT_BRACE, "}", 10, 9),
                new Token(TokenType.RIGHT_BRACE, "}", 12, 5),
                new Token(TokenType.RIGHT_BRACE, "}", 13, 1),
                new Token(TokenType.EOF, "", 14, 2)
        };

        CHTLLexer lexer = new CHTLLexer(input);

        for (Token expectedToken : expectedTokens) {
            Token actualToken = lexer.nextToken();
            // We don't care about the exact column numbers for this test, just the type and literal
            assertEquals(expectedToken.type, actualToken.type, "Token type mismatch");
            assertEquals(expectedToken.literal, actualToken.literal, "Token literal mismatch for type " + expectedToken.type);
        }
    }
}

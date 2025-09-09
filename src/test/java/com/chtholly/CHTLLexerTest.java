package com.chtholly;

import com.chtholly.chthl.lexer.CHTLLexer;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;
import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;

class CHTLLexerTest {

    @Test
    void testSimpleElements() {
        String input = "html { body {} }";
        CHTLLexer lexer = new CHTLLexer(input);
        List<Token> tokens = lexer.scanTokens();

        // Expected tokens: html, {, body, {, }, }, EOF
        assertEquals(7, tokens.size());
        assertEquals(TokenType.IDENTIFIER, tokens.get(0).getType());
        assertEquals("html", tokens.get(0).getLexeme());
        assertEquals(TokenType.LEFT_BRACE, tokens.get(1).getType());
        assertEquals(TokenType.IDENTIFIER, tokens.get(2).getType());
        assertEquals("body", tokens.get(2).getLexeme());
        assertEquals(TokenType.LEFT_BRACE, tokens.get(3).getType());
        assertEquals(TokenType.RIGHT_BRACE, tokens.get(4).getType());
        assertEquals(TokenType.RIGHT_BRACE, tokens.get(5).getType());
        assertEquals(TokenType.EOF, tokens.get(6).getType());
    }

    @Test
    void testTextNodeWithString() {
        String input = "text { \"this is a test\" }";
        CHTLLexer lexer = new CHTLLexer(input);
        List<Token> tokens = lexer.scanTokens();

        // Expected tokens: text, {, "this is a test", }, EOF
        assertEquals(5, tokens.size());
        assertEquals(TokenType.TEXT, tokens.get(0).getType());
        assertEquals(TokenType.LEFT_BRACE, tokens.get(1).getType());
        assertEquals(TokenType.STRING, tokens.get(2).getType());
        assertEquals("this is a test", tokens.get(2).getLiteral());
        assertEquals(TokenType.RIGHT_BRACE, tokens.get(3).getType());
        assertEquals(TokenType.EOF, tokens.get(4).getType());
    }

    @Test
    void testCommentsAreIgnored() {
        String input = """
        // this is a comment
        div {
            /* this is a
               multiline comment */
        }
        """;
        CHTLLexer lexer = new CHTLLexer(input);
        List<Token> tokens = lexer.scanTokens();

        // Expected tokens: div, {, }, EOF
        assertEquals(4, tokens.size());
        assertEquals(TokenType.IDENTIFIER, tokens.get(0).getType());
        assertEquals(TokenType.LEFT_BRACE, tokens.get(1).getType());
        assertEquals(TokenType.RIGHT_BRACE, tokens.get(2).getType());
        assertEquals(TokenType.EOF, tokens.get(3).getType());
    }

    @Test
    void testAttributes() {
        String input = "div { id: box; class: 'container'; }";
        CHTLLexer lexer = new CHTLLexer(input);
        List<Token> tokens = lexer.scanTokens();

        // Expected: div, {, id, :, box, ;, class, :, 'container', ;, }, EOF
        assertEquals(12, tokens.size());
        assertEquals(TokenType.IDENTIFIER, tokens.get(0).getType()); // div
        assertEquals(TokenType.LEFT_BRACE, tokens.get(1).getType()); // {
        assertEquals(TokenType.IDENTIFIER, tokens.get(2).getType()); // id
        assertEquals(TokenType.COLON, tokens.get(3).getType());      // :
        assertEquals(TokenType.IDENTIFIER, tokens.get(4).getType()); // box
        assertEquals(TokenType.SEMICOLON, tokens.get(5).getType());  // ;
        assertEquals(TokenType.IDENTIFIER, tokens.get(6).getType()); // class
        assertEquals(TokenType.COLON, tokens.get(7).getType());      // :
        assertEquals(TokenType.STRING, tokens.get(8).getType());     // 'container'
        assertEquals("container", tokens.get(8).getLiteral());
        assertEquals(TokenType.SEMICOLON, tokens.get(9).getType());  // ;
        assertEquals(TokenType.RIGHT_BRACE, tokens.get(10).getType());// }
        assertEquals(TokenType.EOF, tokens.get(11).getType());       // EOF
    }
}

package CHTL.Expression;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;

public class ExpressionLexer {
    private final String input;
    private int position = 0;
    private static final Map<String, ExpressionTokenType> OPERATORS = new HashMap<>();

    static {
        OPERATORS.put("+", ExpressionTokenType.PLUS);
        OPERATORS.put("-", ExpressionTokenType.MINUS);
        OPERATORS.put("*", ExpressionTokenType.STAR);
        OPERATORS.put("/", ExpressionTokenType.SLASH);
        OPERATORS.put("%", ExpressionTokenType.PERCENT);
        OPERATORS.put("**", ExpressionTokenType.POWER);
        OPERATORS.put("&&", ExpressionTokenType.AND);
        OPERATORS.put("||", ExpressionTokenType.OR);
        OPERATORS.put(">", ExpressionTokenType.GREATER);
        OPERATORS.put(">=", ExpressionTokenType.GREATER_EQUAL);
        OPERATORS.put("<", ExpressionTokenType.LESS);
        OPERATORS.put("<=", ExpressionTokenType.LESS_EQUAL);
        OPERATORS.put("?", ExpressionTokenType.QUESTION);
        OPERATORS.put(":", ExpressionTokenType.COLON);
        OPERATORS.put("(", ExpressionTokenType.LPAREN);
        OPERATORS.put(")", ExpressionTokenType.RPAREN);
        OPERATORS.put(".", ExpressionTokenType.DOT);
        OPERATORS.put(",", ExpressionTokenType.COMMA);
    }

    public ExpressionLexer(String input) {
        this.input = input;
    }

    public List<Token> tokenize() {
        List<Token> tokens = new ArrayList<>();
        while (position < input.length()) {
            char current = peek();
            if (Character.isWhitespace(current)) {
                advance();
                continue;
            }
            if (current == '#' || current == '.') {
                 tokens.add(selector());
            } else if (Character.isDigit(current)) {
                tokens.add(number());
            } else if (current == '"' || current == '\'') {
                tokens.add(string(current));
            } else if (Character.isLetter(current)) {
                tokens.add(identifier());
            } else {
                boolean matched = false;
                if (position + 1 < input.length()) {
                    String twoCharOp = input.substring(position, position + 2);
                    if (OPERATORS.containsKey(twoCharOp)) {
                        tokens.add(new Token(OPERATORS.get(twoCharOp), twoCharOp, position));
                        advance();
                        advance();
                        matched = true;
                    }
                }
                if (!matched) {
                    String oneCharOp = String.valueOf(current);
                    if (OPERATORS.containsKey(oneCharOp)) {
                        tokens.add(new Token(OPERATORS.get(oneCharOp), oneCharOp, position));
                        advance();
                    } else {
                        throw new RuntimeException("Unexpected character: " + current + " at position " + position);
                    }
                }
            }
        }
        tokens.add(new Token(ExpressionTokenType.EOF, "", position));
        return tokens;
    }

    private Token selector() {
        int start = position;
        while (position < input.length()) {
            char c = peek();
            if (OPERATORS.containsKey(String.valueOf(c)) && c != '.') {
                break; // Stop at an operator (but allow . for class selectors)
            }

            advance();

            // If we hit whitespace, peek ahead. If the next thing is an operator, stop.
            // This allows for descendant selectors like `.class div`
            if (Character.isWhitespace(c)) {
                int tempPos = position;
                while(tempPos < input.length() && Character.isWhitespace(input.charAt(tempPos))) {
                    tempPos++;
                }
                if (tempPos < input.length() && OPERATORS.containsKey(String.valueOf(input.charAt(tempPos)))) {
                    break;
                }
            }
        }
        return new Token(ExpressionTokenType.SELECTOR, input.substring(start, position).trim(), start);
    }

    private Token number() {
        int start = position;
        while (position < input.length() && (Character.isDigit(peek()) || peek() == '.')) {
            advance();
        }
        return new Token(ExpressionTokenType.NUMBER, input.substring(start, position), start);
    }

    private Token string(char quoteType) {
        int start = position;
        advance();
        while (position < input.length() && peek() != quoteType) {
            advance();
        }
        if (position < input.length()) {
             advance();
        }
        return new Token(ExpressionTokenType.STRING, input.substring(start + 1, position - 1), start);
    }

    private Token identifier() {
        int start = position;
        while (position < input.length() && Character.isLetterOrDigit(peek())) {
            advance();
        }
        return new Token(ExpressionTokenType.IDENTIFIER, input.substring(start, position), start);
    }

    private char peek() {
        if (position >= input.length()) return '\0';
        return input.charAt(position);
    }

    private String lookahead(int count) {
        if (position + count >= input.length()) {
            return input.substring(position);
        }
        return input.substring(position, position + count);
    }

    private void advance() {
        position++;
    }
}

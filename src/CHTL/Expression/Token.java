package CHTL.Expression;

/**
 * Represents a token in a style expression.
 * @param type The type of the token.
 * @param value The raw string value of the token.
 * @param position The starting position of the token in the input string.
 */
public record Token(ExpressionTokenType type, String value, int position) {
}

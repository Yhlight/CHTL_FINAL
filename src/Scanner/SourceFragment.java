package Scanner;

/**
 * Represents a fragment of the source code, tagged with its type.
 * @param type The type of the fragment (e.g., CHTL, GLOBAL_CSS).
 * @param content The raw string content of the fragment.
 */
public record SourceFragment(FragmentType type, String content) {
}

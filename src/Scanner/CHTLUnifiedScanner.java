package Scanner;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

public class CHTLUnifiedScanner {

    private enum State {
        DEFAULT,
        IN_STRING,
        IN_SINGLE_LINE_COMMENT,
        IN_MULTI_LINE_COMMENT
    }

    private final String source;
    private int cursor = 0;
    private State state = State.DEFAULT;
    private char stringDelimiter;
    private final Stack<String> contextStack = new Stack<>();

    public CHTLUnifiedScanner(String source) {
        this.source = source;
    }

    public List<SourceFragment> scan() {
        List<SourceFragment> fragments = new ArrayList<>();
        int lastSliceEnd = 0;

        while (cursor < source.length()) {
            updateState();

            if (state == State.DEFAULT) {
                 if (isAtWord("style") && contextStack.isEmpty()) {
                    if (cursor > lastSliceEnd) fragments.add(new SourceFragment(FragmentType.CHTL, source.substring(lastSliceEnd, cursor)));
                    lastSliceEnd = processBlock(FragmentType.GLOBAL_CSS, fragments);
                } else if (isAtWord("script")) {
                    if (cursor > lastSliceEnd) fragments.add(new SourceFragment(FragmentType.CHTL, source.substring(lastSliceEnd, cursor)));
                    FragmentType type = contextStack.isEmpty() ? FragmentType.GLOBAL_JS : FragmentType.CHTL_JS;
                    lastSliceEnd = processBlock(type, fragments);
                } else if (peek() == '{') {
                    contextStack.push("block");
                    cursor++;
                } else if (peek() == '}') {
                    if (!contextStack.isEmpty()) contextStack.pop();
                    cursor++;
                } else {
                    cursor++;
                }
            } else {
                cursor++;
            }
        }

        if (cursor > lastSliceEnd) {
            fragments.add(new SourceFragment(FragmentType.CHTL, source.substring(lastSliceEnd, cursor)));
        }

        return fragments;
    }

    private int processBlock(FragmentType type, List<SourceFragment> fragments) {
        // Move cursor past the keyword
        cursor += type == FragmentType.GLOBAL_CSS ? 5 : 6;

        // Find the opening brace, skipping comments and strings
        while (cursor < source.length()) {
            updateState();
            if (state == State.DEFAULT && peek() == '{') break;
            cursor++;
        }
        if (cursor == source.length()) return cursor;

        cursor++; // Move past '{'
        int braceDepth = 1;
        int contentStart = cursor;

        while (cursor < source.length() && braceDepth > 0) {
            updateState();
            if (state == State.DEFAULT) {
                if (peek() == '{') braceDepth++;
                else if (peek() == '}') braceDepth--;
            }
            cursor++;
        }

        int contentEnd = cursor - 1;
        fragments.add(new SourceFragment(type, source.substring(contentStart, contentEnd).trim()));
        return cursor;
    }

    private void updateState() {
        if (state == State.IN_STRING) {
            if (peek() == stringDelimiter) state = State.DEFAULT;
        } else if (state == State.IN_SINGLE_LINE_COMMENT) {
            if (peek() == '\n') state = State.DEFAULT;
        } else if (state == State.IN_MULTI_LINE_COMMENT) {
            if (peek() == '*' && lookahead() == '/') {
                state = State.DEFAULT;
                cursor++; // Consume extra char
            }
        } else { // DEFAULT state
            if (peek() == '"' || peek() == '\'') {
                state = State.IN_STRING;
                stringDelimiter = peek();
            } else if (peek() == '/' && lookahead() == '/') {
                state = State.IN_SINGLE_LINE_COMMENT;
            } else if (peek() == '/' && lookahead() == '*') {
                state = State.IN_MULTI_LINE_COMMENT;
            }
        }
    }

    private boolean isAtWord(String word) {
        if (cursor + word.length() > source.length()) return false;
        if (!source.substring(cursor, cursor + word.length()).equals(word)) return false;
        char charAfter = (cursor + word.length() < source.length()) ? source.charAt(cursor + word.length()) : ' ';
        char charBefore = (cursor > 0) ? source.charAt(cursor - 1) : ' ';
        return Character.isWhitespace(charBefore) && (Character.isWhitespace(charAfter) || charAfter == '{');
    }

    private char peek() {
        if (cursor >= source.length()) return '\0';
        return source.charAt(cursor);
    }

    private char lookahead() {
        if (cursor + 1 >= source.length()) return '\0';
        return source.charAt(cursor + 1);
    }
}

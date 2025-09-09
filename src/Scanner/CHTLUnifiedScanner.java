package Scanner;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

public class CHTLUnifiedScanner {

    private final String source;
    private int cursor = 0;
    private final Stack<String> contextStack = new Stack<>();

    public CHTLUnifiedScanner(String source) {
        this.source = source;
    }

    public List<SourceFragment> scan() {
        List<SourceFragment> fragments = new ArrayList<>();
        int lastSliceEnd = 0;

        while (cursor < source.length()) {
            char c = source.charAt(cursor);

            if (Character.isLetter(c)) {
                String word = readWord();
                if (word.equals("style") && contextStack.isEmpty()) {
                    if (cursor > lastSliceEnd) {
                        fragments.add(new SourceFragment(FragmentType.CHTL, source.substring(lastSliceEnd, cursor - word.length())));
                    }
                    lastSliceEnd = processSpecialBlock(fragments, FragmentType.GLOBAL_CSS);
                    continue;
                } else if (word.equals("script")) {
                     if (cursor > lastSliceEnd) {
                        fragments.add(new SourceFragment(FragmentType.CHTL, source.substring(lastSliceEnd, cursor - word.length())));
                    }
                    FragmentType type = contextStack.isEmpty() ? FragmentType.GLOBAL_JS : FragmentType.CHTL_JS;
                    lastSliceEnd = processSpecialBlock(fragments, type);
                    continue;
                }
                // It was a normal CHTL identifier, do nothing special
            } else if (c == '{') {
                contextStack.push("block");
            } else if (c == '}') {
                if (!contextStack.isEmpty()) contextStack.pop();
            }
            cursor++;
        }

        if (cursor > lastSliceEnd) {
            fragments.add(new SourceFragment(FragmentType.CHTL, source.substring(lastSliceEnd, cursor)));
        }

        return fragments;
    }

    private String readWord() {
        int start = cursor;
        while (cursor < source.length() && Character.isLetter(source.charAt(cursor))) {
            cursor++;
        }
        return source.substring(start, cursor);
    }

    private int processSpecialBlock(List<SourceFragment> fragments, FragmentType type) {
        int blockStart = cursor;
        // Move to the opening brace
        while (cursor < source.length() && source.charAt(cursor) != '{') {
            cursor++;
        }
        if (cursor == source.length()) return blockStart; // Malformed

        cursor++; // Move past '{'
        int braceDepth = 1;
        int contentStart = cursor;

        while (cursor < source.length() && braceDepth > 0) {
            if (source.charAt(cursor) == '{') braceDepth++;
            if (source.charAt(cursor) == '}') braceDepth--;
            cursor++;
        }

        int contentEnd = cursor - 1;
        fragments.add(new SourceFragment(type, source.substring(contentStart, contentEnd).trim()));
        return cursor;
    }
}

#include "UnifiedScanner.h"

#include <cctype>

namespace chtl {

namespace {

bool isIdentifierStart(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '@' || c == '[';
}

bool isIdentifierChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '@' || c == '.' || c == '[' || c == ']';
}

// Skip whitespace
size_t skipSpace(const std::string& s, size_t i) {
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) i++;
    return i;
}

// Skip single-line and multi-line comments, return advanced index
size_t skipComments(const std::string& s, size_t i) {
    bool advanced = true;
    while (advanced && i < s.size()) {
        advanced = false;
        if (i + 1 < s.size() && s[i] == '/' && s[i + 1] == '/') {
            // // comment
            i += 2;
            while (i < s.size() && s[i] != '\n') i++;
            advanced = true;
        } else if (i + 1 < s.size() && s[i] == '/' && s[i + 1] == '*') {
            // /* */ comment
            i += 2;
            while (i + 1 < s.size() && !(s[i] == '*' && s[i + 1] == '/')) i++;
            if (i + 1 < s.size()) i += 2;
            advanced = true;
        }
    }
    return i;
}

// Read identifier-like token
std::string readIdent(const std::string& s, size_t& i) {
    size_t start = i;
    while (i < s.size() && isIdentifierChar(s[i])) i++;
    return s.substr(start, i - start);
}

// Capture a balanced block starting at '{', returns end index after block
size_t captureBalanced(const std::string& s, size_t i, int& braceDepth) {
    // assumes s[i] == '{'
    if (i >= s.size() || s[i] != '{') return i;
    braceDepth++;
    i++;
    while (i < s.size() && braceDepth > 0) {
        if (s[i] == '"' || s[i] == '\'') {
            // string literal, skip until matching quote, handle escapes
            char quote = s[i++];
            while (i < s.size()) {
                if (s[i] == '\\') { i += 2; continue; }
                if (s[i] == quote) { i++; break; }
                i++;
            }
            continue;
        }
        if (i + 1 < s.size() && s[i] == '/' && s[i + 1] == '/') {
            while (i < s.size() && s[i] != '\n') i++;
            continue;
        }
        if (i + 1 < s.size() && s[i] == '/' && s[i + 1] == '*') {
            i += 2;
            while (i + 1 < s.size() && !(s[i] == '*' && s[i + 1] == '/')) i++;
            if (i + 1 < s.size()) i += 2;
            continue;
        }
        if (s[i] == '{') { braceDepth++; i++; continue; }
        if (s[i] == '}') { braceDepth--; i++; if (braceDepth <= 0) break; continue; }
        i++;
    }
    return i;
}

}

std::vector<CodeSlice> UnifiedScanner::scan(const std::string& source) {
    std::vector<CodeSlice> slices;
    size_t i = 0;
    int elementDepth = 0; // depth of generic CHTL element blocks

    auto flush = [&](CodeSliceType type, size_t start, size_t end) {
        if (end > start) {
            slices.push_back(CodeSlice{ type, source.substr(start, end - start) });
        }
    };

    size_t chunkStart = 0;
    CodeSliceType current = CodeSliceType::CHTL;

    while (i < source.size()) {
        i = skipSpace(source, i);
        i = skipComments(source, i);
        if (i >= source.size()) break;

        // Detect identifiers at this point
        if (isIdentifierStart(source[i])) {
            size_t identStart = i;
            std::string ident = readIdent(source, i);
            size_t after = skipSpace(source, i);
            if (after < source.size() && source[after] == '{') {
                // Block begins. Determine classification.
                bool isTopLevel = (elementDepth == 0);
                CodeSliceType nextType = CodeSliceType::CHTL;

                if (isTopLevel && ident == "style") {
                    nextType = CodeSliceType::CSS; // global CSS
                } else if (isTopLevel && ident == "script") {
                    nextType = CodeSliceType::JS; // global JS (may include CHTL JS later via placeholders)
                } else {
                    // treat as CHTL element or directive
                    nextType = CodeSliceType::CHTL;
                }

                // If classification changes, flush previous chunk
                if (nextType != current) {
                    flush(current, chunkStart, identStart);
                    chunkStart = identStart;
                    current = nextType;
                }

                // Advance through balanced block and adjust elementDepth for non global style/script
                int braceDepth = 0;
                // Count this block as element depth if not global style/script
                if (!(isTopLevel && (ident == "style" || ident == "script"))) {
                    elementDepth++;
                }
                i = after; // at '{'
                i = captureBalanced(source, i, braceDepth); // consumes until matching '}'
                if (!(isTopLevel && (ident == "style" || ident == "script"))) {
                    if (elementDepth > 0) elementDepth--;
                }
                continue;
            } else {
                // Not a block start; continue
                i = after;
                continue;
            }
        }

        // Single characters processing
        if (source[i] == '{') {
            elementDepth++;
            i++;
            continue;
        }
        if (source[i] == '}') {
            if (elementDepth > 0) elementDepth--;
            i++;
            continue;
        }

        i++;
    }

    // Flush remainder
    flush(current, chunkStart, source.size());

    return slices;
}

std::string UnifiedScanner::encodePlaceholders(const std::string& source) {
    // Collapse non-CHTL-JS segments while preserving {{ ... }} occurrences.
    // This is a foundational implementation; nested-safe and registry-based decoding can be added later.
    std::string out;
    out.reserve(source.size());
    const std::string ph = "_JS_CODE_PLACEHOLDER_";
    size_t i = 0;
    bool inString = false;
    char stringQuote = 0;
    bool justEmittedPH = false;
    while (i < source.size()) {
        char c = source[i];
        if (!inString && i + 1 < source.size() && source[i] == '{' && source[i + 1] == '{') {
            // emit any separator before
            size_t start = i;
            // copy balanced {{ ... }} including braces
            out.append("{{");
            i += 2;
            // copy until next }} (no nesting for selectors by design)
            while (i + 1 < source.size() && !(source[i] == '}' && source[i + 1] == '}')) {
                out.push_back(source[i]);
                i++;
            }
            if (i + 1 < source.size()) {
                out.append("}}");
                i += 2;
            }
            justEmittedPH = false; // after a preserved token
            continue;
        }

        // track strings to avoid replacing inside
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringQuote = c;
            out.push_back(c);
            i++;
            continue;
        }
        if (inString) {
            out.push_back(c);
            if (c == '\\' && i + 1 < source.size()) {
                out.push_back(source[i + 1]);
                i += 2;
                continue;
            }
            if (c == stringQuote) {
                inString = false;
                stringQuote = 0;
            }
            i++;
            continue;
        }

        // comments: preserve as part of placeholder collapsing boundaries
        if (i + 1 < source.size() && source[i] == '/' && source[i + 1] == '/') {
            if (!justEmittedPH) { out.append(ph); out.push_back(' '); justEmittedPH = true; }
            while (i < source.size() && source[i] != '\n') i++;
            continue;
        }
        if (i + 1 < source.size() && source[i] == '/' && source[i + 1] == '*') {
            if (!justEmittedPH) { out.append(ph); out.push_back(' '); justEmittedPH = true; }
            i += 2;
            while (i + 1 < source.size() && !(source[i] == '*' && source[i + 1] == '/')) i++;
            if (i + 1 < source.size()) i += 2;
            continue;
        }

        // whitespace: keep minimal single spaces around placeholders
        if (std::isspace(static_cast<unsigned char>(c))) {
            out.push_back(c);
            i++;
            continue;
        }

        // any other code outside preserved segments collapses to one placeholder token
        if (!justEmittedPH) {
            out.append(ph);
            out.push_back(' ');
            justEmittedPH = true;
        }
        i++;
    }
    return out;
}

std::string UnifiedScanner::decodePlaceholders(const std::string& sourceWithPlaceholders) {
    return sourceWithPlaceholders;
}

} // namespace chtl


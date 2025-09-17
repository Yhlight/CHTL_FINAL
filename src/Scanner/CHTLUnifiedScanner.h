#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace CHTL {

// Represents the different language fragments the scanner can identify.
enum class FragmentType {
    CHTL,
    CSS,
    JS,
    CHTL_JS,
    UNKNOWN
};

// A fragment of code identified by the scanner.
struct CodeFragment {
    std::string content;
    FragmentType type;
};

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    std::vector<CodeFragment> scan();

    // The CodeMerger will need this map to reconstruct the final JS file.
    const std::map<std::string, std::string>& getPlaceholderMap() const;

    // Public for the static helper function in the .cpp file
    enum class ParsingState {
        DEFAULT,
        IN_STRING,
        IN_SINGLE_LINE_COMMENT,
        IN_MULTI_LINE_COMMENT,
    };

private:
    // Main processing logic
    void process();
    bool isKeywordAt(size_t pos, const std::string& keyword);

    // This is the new core function for placeholder-based scanning.
    // It scans a given string (a script block) and returns the modified
    // string with placeholders, while populating the fragments vector.
    std::string scanJsAndChtlJs(const std::string& script_content);

    // Helper methods for scanning and state management
    void advance(size_t n = 1);
    char peek(size_t offset = 0) const;
    bool isAtEnd() const;

    // Finds the end of a CHTL construct like script {} or [Template] {}
    size_t findEndOfBlock(size_t start_pos, char open_brace, char close_brace);

    // Placeholder and fragment management
    std::string createPlaceholder(const std::string& content);
    void addFragment(const std::string& content, FragmentType type);

    // Member variables
    const std::string& source_;
    std::vector<CodeFragment> fragments_;
    std::map<std::string, std::string> placeholder_map_;

    size_t cursor_ = 0;
    int placeholder_id_ = 0;

    // State machine tracking variables
    ParsingState state_ = ParsingState::DEFAULT;
    char string_delimiter_ = '\0';
    int brace_level_ = 0;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H

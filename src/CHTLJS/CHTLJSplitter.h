#ifndef CHTL_JS_SPLITTER_H
#define CHTL_JS_SPLITTER_H

#include <string>
#include <vector>
#include <map>

namespace CHTLJS {

// The result of splitting a script block into CHTL JS and pure JS.
struct JsSplitResult {
    // The CHTL JS constructs with pure JS replaced by placeholders.
    std::string cjs_with_placeholders;
    // A map from placeholder keys to the original pure JS fragments.
    std::map<std::string, std::string> js_placeholder_map;
};

class CHTLJSplitter {
public:
    explicit CHTLJSplitter(const std::string& source);
    JsSplitResult split();

private:
    void process();
    size_t findNextChtlJsConstruct();
    void flushJsChunk(size_t end);

    const std::string& source_;
    JsSplitResult result_;
    size_t cursor_ = 0;
    size_t last_flush_pos_ = 0;
    int placeholder_id_ = 0;

    static const std::vector<std::string> CHTLJS_KEYWORDS;
};

} // namespace CHTLJS

#endif // CHTL_JS_SPLITTER_H

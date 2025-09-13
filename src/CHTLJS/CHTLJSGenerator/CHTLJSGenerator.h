#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "Scanner/CHTLUnifiedScanner.h" // For CodeChunk
#include "CHTLJS/CHTLJSParser/CHTLJSParser.h" // For CHTLJSParser
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTLJS {

// Forward declarations
class EnhancedSelectorNode;
class ListenNode;
class DelegateNode;
class AnimateNode;
class ValueNode;

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    std::string generate(const std::vector<CHTL::CodeChunk>& chunks, const std::map<std::string, std::string>& placeholder_map, std::shared_ptr<CHTLJSContext> context);

private:
    void visit(const CHTLJSNode* node);
    void visitEnhancedSelector(const EnhancedSelectorNode* node);
    void visitListenNode(const ListenNode* node);
    void visitDelegateNode(const DelegateNode* node);
    void visitAnimateNode(const AnimateNode* node);
    void visitValueNode(const ValueNode* node);

    std::string output_;
};

} // namespace CHTLJS

#endif // CHTL_JS_GENERATOR_H

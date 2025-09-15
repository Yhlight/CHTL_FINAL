#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "../CHTLJSNode/CHTLJSNode.h"
#include <string>

namespace CHTLJS {

// Forward declarations
class EnhancedSelectorNode;
class ListenNode;
class DelegateNode;
class AnimateNode;
class ValueNode;

// Forward declarations for all node types
class SequenceNode;
class EnhancedSelectorNode;
class ListenNode;
class DelegateNode;
class AnimateNode;
class ValueNode;
class PlaceholderNode;
class VirNode;

class CHTLJSGenerator {
public:
    CHTLJSGenerator() = default;
    std::string generate(const CHTLJSNode& root);

private:
    std::string visit(const CHTLJSNode* node);
    std::string visitSequenceNode(const SequenceNode* node);
    std::string visitEnhancedSelector(const EnhancedSelectorNode* node);
    std::string visitListenNode(const ListenNode* node);
    std::string visitDelegateNode(const DelegateNode* node);
    std::string visitAnimateNode(const AnimateNode* node);
    std::string visitValueNode(const ValueNode* node);
    std::string visitPlaceholderNode(const PlaceholderNode* node);
    std::string visitVirNode(const VirNode* node);
};

} // namespace CHTLJS

#endif // CHTL_JS_GENERATOR_H

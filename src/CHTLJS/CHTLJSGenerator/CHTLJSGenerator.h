#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSParser/CHTLJSContext.h"
#include <string>
#include <map>
#include <memory>

namespace CHTLJS {

// Forward declarations for all node types
class SequenceNode;
class EnhancedSelectorNode;
class ListenNode;
class DelegateNode;
class AnimateNode;
class ValueNode;
class PlaceholderNode;
class VirNode;
class RouterNode;
class ScriptLoaderNode;

class CHTLJSGenerator {
public:
    explicit CHTLJSGenerator(std::shared_ptr<CHTLJSContext> context);

    // The placeholder map from the scanner is required for generation.
    std::string generate(const CHTLJSNode& root, const std::map<std::string, std::string>& placeholder_map);

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
    std::string visitRouterNode(const RouterNode* node);
    std::string visitScriptLoaderNode(const ScriptLoaderNode* node);

    std::shared_ptr<CHTLJSContext> context_;
    const std::map<std::string, std::string>* placeholder_map_ = nullptr;
};

} // namespace CHTLJS

#endif // CHTL_JS_GENERATOR_H

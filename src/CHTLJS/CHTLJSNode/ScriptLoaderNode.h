#ifndef CHTL_JS_SCRIPT_LOADER_NODE_H
#define CHTL_JS_SCRIPT_LOADER_NODE_H

#include "CHTLJSNode.h"
#include <string>
#include <vector>

namespace CHTLJS {

class ScriptLoaderNode : public CHTLJSNode {
public:
    ScriptLoaderNode() : CHTLJSNode(CHTLJSNodeType::ScriptLoader) {}

    std::vector<std::string> file_paths;

    void addPath(const std::string& path) {
        file_paths.push_back(path);
    }
};

} // namespace CHTLJS

#endif // CHTL_JS_SCRIPT_LOADER_NODE_H

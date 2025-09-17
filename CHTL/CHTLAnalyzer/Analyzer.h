#ifndef ANALYZER_H
#define ANALYZER_H

#include "../CHTLNode/Node.h"

namespace CHTL {

// The Analyzer class is responsible for walking the AST after parsing
// and applying semantic rules, such as automatic attribute addition.
class Analyzer {
public:
    Analyzer();

    // The main entry point for the analysis pass.
    void analyze(RootNode* root);

private:
    // Main dispatch function for visiting nodes.
    void visit(Node* node);

    // Specific visitor functions for each node type that needs processing.
    void visitElementNode(ElementNode* node);
    // Other node types like StyleNode might be visited from within ElementNode.
};

} // namespace CHTL

#endif // ANALYZER_H

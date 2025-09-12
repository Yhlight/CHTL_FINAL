#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLExpression/ExprNode.h"

#include <string>
#include <memory>
#include <sstream>
#include <map>
#include <set>

namespace CHTL
{
    struct EvaluatedValue
    {
        double value;
        std::string unit;
        std::string stringValue;
    };

    class Generator
    {
    public:
        Generator(std::unique_ptr<ProgramNode> ast);
        std::string generate();

    private:
        // Evaluation methods
        EvaluatedValue evaluate(ExprNode* node);
        EvaluatedValue evaluateStyleProperty(const std::string& selector, const std::string& propertyName);

        // AST traversal methods
        void collectSymbols(BaseNode* node);
        void visit(BaseNode* node);
        void visitProgramNode(ProgramNode* node);
        void visitElementNode(ElementNode* node);
        void visitTextNode(TextNode* node);
        void visitCommentNode(CommentNode* node);
        void visitStyleNode(StyleNode* node);

        // State
        std::unique_ptr<ProgramNode> m_ast;
        std::stringstream m_output;
        std::stringstream m_global_styles;
        std::map<std::string, ElementNode*> m_symbol_table;
        std::set<ElementNode*> m_evaluation_stack;
        int m_indentLevel = 0;
    };
}

#endif // CHTL_GENERATOR_H

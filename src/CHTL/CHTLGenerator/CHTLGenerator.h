#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/PropertyReferenceNode.h"
#include "../CHTLNode/PropertyValue.h"
#include "../CHTLNode/ExpressionNode.h"
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <vector>

namespace CHTL {

class CHTLGenerator {
public:
    CHTLGenerator() = default;
    std::string generate(RootNode& root);

private:
    class ExpressionEvaluator : public ExpressionVisitor {
    public:
        ExpressionEvaluator(CHTLGenerator& generator, const ElementNode* context_element);
        std::string evaluate(const ExpressionNode& expr);

        void visit(const BinaryExpression& expr) override;
        void visit(const ConditionalExpression& expr) override;
        void visit(const LiteralExpression& expr) override;
        void visit(const ReferenceExpression& expr) override;

    private:
        CHTLGenerator& generator_;
        const ElementNode* context_element_;
        std::string result_;
    };

    void render(const Node* node);
    void renderElement(const ElementNode* node);
    void renderText(const TextNode* node);
    void renderComment(const CommentNode* node);

    void indent();

    std::stringstream output_;
    std::stringstream global_styles_;
    int indentLevel_ = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H

#include <iostream>
#include <string>
#include <memory>
#include "CHTL/Expression/Expr.h"
#include "CHTL/Expression/ExpressionEvaluator.h"

using namespace CHTL;

int main() {
    // Create a simple LiteralExpr for "red"
    auto expr = std::make_unique<LiteralExpr>(0, "red", true);
    
    // Create evaluator
    std::map<std::string, std::map<std::string, TemplateDefinitionNode>> templates;
    ExpressionEvaluator evaluator(templates, nullptr);
    
    // Evaluate the expression
    try {
        EvaluatedValue result = evaluator.evaluate(expr.get(), nullptr);
        std::cout << "Type: " << (result.type == ValueType::STRING ? "STRING" : "NUMERIC") << std::endl;
        std::cout << "Value: " << result.value << std::endl;
        std::cout << "Unit: '" << result.unit << "'" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
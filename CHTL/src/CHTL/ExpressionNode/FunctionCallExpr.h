#pragma once

#include "Expr.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class FunctionCallExpr : public Expr {
public:
    FunctionCallExpr(std::unique_ptr<Expr> callee, std::vector<std::unique_ptr<Expr>> arguments)
        : m_callee(std::move(callee)), m_arguments(std::move(arguments)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::unique_ptr<Expr> clone() const override {
        std::vector<std::unique_ptr<Expr>> cloned_args;
        for (const auto& arg : m_arguments) {
            cloned_args.push_back(arg->clone());
        }
        return std::make_unique<FunctionCallExpr>(m_callee->clone(), std::move(cloned_args));
    }

    const Expr* getCallee() const { return m_callee.get(); }
    const std::vector<std::unique_ptr<Expr>>& getArguments() const { return m_arguments; }

private:
    std::unique_ptr<Expr> m_callee;
    std::vector<std::unique_ptr<Expr>> m_arguments;
};

} // namespace CHTL

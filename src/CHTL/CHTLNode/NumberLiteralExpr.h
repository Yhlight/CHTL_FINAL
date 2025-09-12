#ifndef CHTL_NUMBERLITERALEXPR_H
#define CHTL_NUMBERLITERALEXPR_H

#include "ExprNode.h"
#include <string>

namespace CHTL {

class NumberLiteralExpr : public ExprNode {
public:
    explicit NumberLiteralExpr(const std::string& value) {
        // A simple way to separate value and unit
        size_t first_alpha = value.find_first_not_of("-.0123456789");
        if (first_alpha != std::string::npos) {
            numberValue = std::stod(value.substr(0, first_alpha));
            unit = value.substr(first_alpha);
        } else {
            numberValue = std::stod(value);
            unit = "";
        }
    }

    void accept(ExprVisitor& visitor) override;

    double getNumberValue() const { return numberValue; }
    const std::string& getUnit() const { return unit; }

private:
    double numberValue;
    std::string unit;
};

} // namespace CHTL

#endif // CHTL_NUMBERLITERALEXPR_H

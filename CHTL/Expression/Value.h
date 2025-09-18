#ifndef CHTL_VALUE_H
#define CHTL_VALUE_H

#include <string>
#include <stdexcept>

namespace CHTL {

    struct Value {
        enum class Type {
            EMPTY,
            BOOL,
            NUMBER,
            STRING
        };

        Type type = Type::EMPTY;

        bool bool_val = false;
        double number_val = 0.0;
        std::string string_val;
        std::string unit; // e.g., "px", "%", "em"

        // Constructors
        Value() : type(Type::EMPTY) {}
        explicit Value(bool val) : type(Type::BOOL), bool_val(val) {}
        Value(double val, std::string unit_val) : type(Type::NUMBER), number_val(val), unit(std::move(unit_val)) {}
        explicit Value(std::string val) : type(Type::STRING), string_val(std::move(val)) {}

        bool isTruthy() const {
            switch (type) {
                case Type::BOOL: return bool_val;
                case Type::NUMBER: return number_val != 0.0;
                case Type::STRING: return !string_val.empty();
                default: return false;
            }
        }

        std::string toString() const {
            switch (type) {
                case Type::NUMBER: {
                    // Avoid trailing zeros for whole numbers
                    if (number_val == static_cast<long long>(number_val)) {
                        return std::to_string(static_cast<long long>(number_val)) + unit;
                    }
                    // Format to a reasonable precision
                    std::string str = std::to_string(number_val);
                    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                     if (str.back() == '.') str.pop_back();
                    return str + unit;
                }
                case Type::STRING:
                    // In CSS, strings that aren't keywords should be quoted.
                    // We assume for now that the parser provides them unquoted.
                    return string_val;
                case Type::BOOL: // Booleans don't typically appear in final CSS
                    return bool_val ? "true" : "false";
                default:
                    return "";
            }
        }
    };

} // namespace CHTL

#endif // CHTL_VALUE_H

#pragma once

#include <string>
#include <stdexcept>

struct Value {
    double number = 0.0;
    std::string unit;
    std::string stringValue;
    bool isString = false;

    Value() = default;

    // Constructor for numeric values
    Value(double number, std::string unit) : number(number), unit(unit), isString(false) {}

    // Constructor for string-only values
    explicit Value(std::string stringValue) : stringValue(stringValue), isString(true) {}

    // Helper to convert back to a string for CSS
    std::string toString() const {
        if (isString) {
            return stringValue;
        }
        if (unit.empty()) {
            // This logic cleans up trailing zeros and decimal points
            std::string numStr = std::to_string(number);
            numStr.erase(numStr.find_last_not_of('0') + 1, std::string::npos);
            if (numStr.back() == '.') numStr.pop_back();
            return numStr;
        }
        std::string numStr = std::to_string(number);
        numStr.erase(numStr.find_last_not_of('0') + 1, std::string::npos);
        if (numStr.back() == '.') numStr.pop_back();
        return numStr + unit;
    }

    // Helper to parse a string like "100px" into a Value
    static Value fromString(const std::string& s) {
        if (s.empty()) return Value();

        // Check if the string is purely numeric (possibly with a unit)
        size_t first_char = s.find_first_not_of("-.0123456789");

        // If it's all numbers
        if (first_char == std::string::npos) {
            return Value(std::stod(s), "");
        }

        // If it starts with non-numeric characters, it's a string value like 'auto'
        if (first_char == 0) {
            return Value(s);
        }

        // Otherwise, it's a number with a unit
        std::string num_part = s.substr(0, first_char);
        std::string unit_part = s.substr(first_char);
        return Value(std::stod(num_part), unit_part);
    }
};

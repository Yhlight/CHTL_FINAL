#include "Arg.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace CHTL {

Arg::Arg() : type(ArgType::Undefined) {}

Arg::Arg(const std::string& value) : type(ArgType::String) {
    setValue(value);
}

Arg::Arg(int value) : type(ArgType::Number) {
    setValue(static_cast<double>(value));
}

Arg::Arg(double value) : type(ArgType::Number) {
    setValue(value);
}

Arg::Arg(bool value) : type(ArgType::Boolean) {
    setValue(value);
}

Arg::Arg(const std::vector<Arg>& value) : type(ArgType::Array) {
    setValue(value);
}

Arg::Arg(const std::map<std::string, Arg>& value) : type(ArgType::Object) {
    setValue(value);
}

std::string Arg::getTypeName() const {
    switch (type) {
        case ArgType::String: return "string";
        case ArgType::Number: return "number";
        case ArgType::Boolean: return "boolean";
        case ArgType::Array: return "array";
        case ArgType::Object: return "object";
        case ArgType::Function: return "function";
        case ArgType::Null: return "null";
        case ArgType::Undefined: return "undefined";
        default: return "unknown";
    }
}

std::string Arg::getString() const {
    if (type == ArgType::String) {
        return getValue<std::string>();
    }
    throw std::runtime_error("Cannot get string from non-string Arg");
}

int Arg::getInt() const {
    if (type == ArgType::Number) {
        return static_cast<int>(getValue<double>());
    }
    throw std::runtime_error("Cannot get int from non-number Arg");
}

double Arg::getDouble() const {
    if (type == ArgType::Number) {
        return getValue<double>();
    }
    throw std::runtime_error("Cannot get double from non-number Arg");
}

bool Arg::getBool() const {
    if (type == ArgType::Boolean) {
        return getValue<bool>();
    }
    throw std::runtime_error("Cannot get bool from non-boolean Arg");
}

std::vector<Arg> Arg::getArray() const {
    if (type == ArgType::Array) {
        return getValue<std::vector<Arg>>();
    }
    throw std::runtime_error("Cannot get array from non-array Arg");
}

std::map<std::string, Arg> Arg::getObject() const {
    if (type == ArgType::Object) {
        return getValue<std::map<std::string, Arg>>();
    }
    throw std::runtime_error("Cannot get object from non-object Arg");
}

void Arg::setString(const std::string& value) {
    type = ArgType::String;
    setValue(value);
}

void Arg::setInt(int value) {
    type = ArgType::Number;
    setValue(static_cast<double>(value));
}

void Arg::setDouble(double value) {
    type = ArgType::Number;
    setValue(value);
}

void Arg::setBool(bool value) {
    type = ArgType::Boolean;
    setValue(value);
}

void Arg::setArray(const std::vector<Arg>& value) {
    type = ArgType::Array;
    setValue(value);
}

void Arg::setObject(const std::map<std::string, Arg>& value) {
    type = ArgType::Object;
    setValue(value);
}

std::string Arg::toString() const {
    switch (type) {
        case ArgType::String: return getString();
        case ArgType::Number: return std::to_string(getDouble());
        case ArgType::Boolean: return getBool() ? "true" : "false";
        case ArgType::Array: {
            std::stringstream ss;
            ss << "[";
            auto arr = getArray();
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arr[i].toString();
            }
            ss << "]";
            return ss.str();
        }
        case ArgType::Object: {
            std::stringstream ss;
            ss << "{";
            auto obj = getObject();
            size_t i = 0;
            for (const auto& pair : obj) {
                if (i > 0) ss << ", ";
                ss << pair.first << ": " << pair.second.toString();
                ++i;
            }
            ss << "}";
            return ss.str();
        }
        case ArgType::Function: return "[Function]";
        case ArgType::Null: return "null";
        case ArgType::Undefined: return "undefined";
        default: return "unknown";
    }
}

int Arg::toInt() const {
    switch (type) {
        case ArgType::Number: return getInt();
        case ArgType::String: {
            try {
                return std::stoi(getString());
            } catch (...) {
                return 0;
            }
        }
        case ArgType::Boolean: return getBool() ? 1 : 0;
        default: return 0;
    }
}

double Arg::toDouble() const {
    switch (type) {
        case ArgType::Number: return getDouble();
        case ArgType::String: {
            try {
                return std::stod(getString());
            } catch (...) {
                return 0.0;
            }
        }
        case ArgType::Boolean: return getBool() ? 1.0 : 0.0;
        default: return 0.0;
    }
}

bool Arg::toBool() const {
    switch (type) {
        case ArgType::Boolean: return getBool();
        case ArgType::String: return !getString().empty();
        case ArgType::Number: return getDouble() != 0.0;
        case ArgType::Array: return !getArray().empty();
        case ArgType::Object: return !getObject().empty();
        case ArgType::Null: return false;
        case ArgType::Undefined: return false;
        default: return false;
    }
}

bool Arg::operator==(const Arg& other) const {
    if (type != other.type) return false;
    
    switch (type) {
        case ArgType::String: return getString() == other.getString();
        case ArgType::Number: return getDouble() == other.getDouble();
        case ArgType::Boolean: return getBool() == other.getBool();
        case ArgType::Array: return getArray() == other.getArray();
        case ArgType::Object: return getObject() == other.getObject();
        case ArgType::Null: return true;
        case ArgType::Undefined: return true;
        default: return false;
    }
}

bool Arg::operator!=(const Arg& other) const {
    return !(*this == other);
}

bool Arg::operator<(const Arg& other) const {
    if (type != other.type) return static_cast<int>(type) < static_cast<int>(other.type);
    
    switch (type) {
        case ArgType::String: return getString() < other.getString();
        case ArgType::Number: return getDouble() < other.getDouble();
        case ArgType::Boolean: return getBool() < other.getBool();
        default: return false;
    }
}

bool Arg::operator>(const Arg& other) const {
    return other < *this;
}

bool Arg::operator<=(const Arg& other) const {
    return *this < other || *this == other;
}

bool Arg::operator>=(const Arg& other) const {
    return *this > other || *this == other;
}

std::string Arg::toJSON() const {
    switch (type) {
        case ArgType::String: return "\"" + getString() + "\"";
        case ArgType::Number: return std::to_string(getDouble());
        case ArgType::Boolean: return getBool() ? "true" : "false";
        case ArgType::Array: {
            std::stringstream ss;
            ss << "[";
            auto arr = getArray();
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arr[i].toJSON();
            }
            ss << "]";
            return ss.str();
        }
        case ArgType::Object: {
            std::stringstream ss;
            ss << "{";
            auto obj = getObject();
            size_t i = 0;
            for (const auto& pair : obj) {
                if (i > 0) ss << ", ";
                ss << "\"" << pair.first << "\": " << pair.second.toJSON();
                ++i;
            }
            ss << "}";
            return ss.str();
        }
        case ArgType::Function: return "\"[Function]\"";
        case ArgType::Null: return "null";
        case ArgType::Undefined: return "undefined";
        default: return "\"unknown\"";
    }
}

template<typename T>
T Arg::getValue() const {
    try {
        return std::any_cast<T>(value);
    } catch (const std::bad_any_cast& e) {
        throw std::runtime_error("Type cast error: " + std::string(e.what()));
    }
}

template<typename T>
void Arg::setValue(const T& val) {
    value = val;
}

} // namespace CHTL
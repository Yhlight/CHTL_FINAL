#include "Arg.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {
namespace CJMOD {

Arg::Arg() {}

void Arg::addToken(const std::string& token) {
    auto atomArg = std::make_shared<AtomArg>(token);
    args.push_back(atomArg);
}

void Arg::addArg(std::shared_ptr<AtomArg> arg) {
    if (arg) {
        args.push_back(arg);
    }
}

void Arg::removeArg(size_t index) {
    if (index < args.size()) {
        args.erase(args.begin() + index);
    }
}

void Arg::clear() {
    args.clear();
    bindings.clear();
    matchCounters.clear();
    transformResult.clear();
}

std::shared_ptr<AtomArg> Arg::getArg(size_t index) const {
    if (index < args.size()) {
        return args[index];
    }
    return nullptr;
}

std::string Arg::getToken(size_t index) const {
    if (index < args.size()) {
        return args[index]->getToken();
    }
    return "";
}

size_t Arg::size() const {
    return args.size();
}

bool Arg::empty() const {
    return args.empty();
}

void Arg::bind(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    bindings[placeholder] = func;
}

void Arg::bind(const std::string& placeholder, const std::string& value) {
    bindings[placeholder] = [value](const std::string&) { return value; };
}

void Arg::unbind(const std::string& placeholder) {
    bindings.erase(placeholder);
}

bool Arg::hasBinding(const std::string& placeholder) const {
    return bindings.find(placeholder) != bindings.end();
}

std::string Arg::match(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    // 自动增加匹配计数器
    matchCounters[placeholder]++;
    
    // 查找匹配的占位符
    for (auto& arg : args) {
        if (arg->getToken() == placeholder) {
            return func(arg->getValue());
        }
    }
    
    return "";
}

std::string Arg::match(const std::string& placeholder, const std::string& value) {
    return match(placeholder, [value](const std::string&) { return value; });
}

void Arg::resetMatchCounters() {
    matchCounters.clear();
}

void Arg::fillValue(const Arg& source) {
    if (source.size() != size()) {
        return;
    }
    
    for (size_t i = 0; i < size(); ++i) {
        if (args[i] && source.args[i]) {
            args[i]->fillValue(source.args[i]->getValue());
        }
    }
}

void Arg::fillValue(size_t index, const std::string& value) {
    if (index < args.size()) {
        args[index]->fillValue(value);
    }
}

void Arg::fillValue(const std::string& placeholder, const std::string& value) {
    for (auto& arg : args) {
        if (arg->getToken() == placeholder) {
            arg->fillValue(value);
        }
    }
}

void Arg::transform(const std::string& template) {
    transformResult = template;
    
    // 替换占位符
    for (const auto& arg : args) {
        if (arg->isPlaceholder()) {
            std::string placeholder = arg->getToken();
            std::string value = arg->getValue();
            
            // 应用绑定函数
            if (hasBinding(placeholder)) {
                value = bindings[placeholder](value);
            }
            
            // 替换模板中的占位符
            size_t pos = 0;
            while ((pos = transformResult.find(placeholder, pos)) != std::string::npos) {
                transformResult.replace(pos, placeholder.length(), value);
                pos += value.length();
            }
        }
    }
}

void Arg::transform(const std::string& template, const std::vector<std::string>& values) {
    transformResult = template;
    
    // 按顺序替换占位符
    size_t valueIndex = 0;
    for (const auto& arg : args) {
        if (arg->isPlaceholder() && valueIndex < values.size()) {
            std::string placeholder = arg->getToken();
            std::string value = values[valueIndex++];
            
            // 应用绑定函数
            if (hasBinding(placeholder)) {
                value = bindings[placeholder](value);
            }
            
            // 替换模板中的占位符
            size_t pos = 0;
            while ((pos = transformResult.find(placeholder, pos)) != std::string::npos) {
                transformResult.replace(pos, placeholder.length(), value);
                pos += value.length();
            }
        }
    }
}

std::string Arg::getTransformResult() const {
    return transformResult;
}

void Arg::clearTransform() {
    transformResult.clear();
}

std::vector<size_t> Arg::findPlaceholder(const std::string& placeholder) const {
    std::vector<size_t> indices;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] && args[i]->getToken() == placeholder) {
            indices.push_back(i);
        }
    }
    
    return indices;
}

std::vector<std::string> Arg::getAllPlaceholders() const {
    std::vector<std::string> placeholders;
    
    for (const auto& arg : args) {
        if (arg && arg->isPlaceholder()) {
            std::string token = arg->getToken();
            if (std::find(placeholders.begin(), placeholders.end(), token) == placeholders.end()) {
                placeholders.push_back(token);
            }
        }
    }
    
    return placeholders;
}

std::vector<std::string> Arg::getAllTokens() const {
    std::vector<std::string> tokens;
    
    for (const auto& arg : args) {
        if (arg) {
            tokens.push_back(arg->getToken());
        }
    }
    
    return tokens;
}

bool Arg::validate() const {
    for (const auto& arg : args) {
        if (!arg) {
            return false;
        }
        
        if (!arg->validate()) {
            return false;
        }
    }
    
    return true;
}

bool Arg::hasRequiredPlaceholders() const {
    for (const auto& arg : args) {
        if (arg && arg->isRequired()) {
            return true;
        }
    }
    return false;
}

bool Arg::hasOptionalPlaceholders() const {
    for (const auto& arg : args) {
        if (arg && arg->isOptional()) {
            return true;
        }
    }
    return false;
}

bool Arg::hasUnorderedPlaceholders() const {
    for (const auto& arg : args) {
        if (arg && arg->isUnordered()) {
            return true;
        }
    }
    return false;
}

std::unordered_map<std::string, int> Arg::getPlaceholderCounts() const {
    std::unordered_map<std::string, int> counts;
    
    for (const auto& arg : args) {
        if (arg && arg->isPlaceholder()) {
            counts[arg->getToken()]++;
        }
    }
    
    return counts;
}

std::unordered_map<std::string, int> Arg::getTokenCounts() const {
    std::unordered_map<std::string, int> counts;
    
    for (const auto& arg : args) {
        if (arg) {
            counts[arg->getToken()]++;
        }
    }
    
    return counts;
}

void Arg::sortByPlaceholder() {
    std::sort(args.begin(), args.end(), 
        [](const std::shared_ptr<AtomArg>& a, const std::shared_ptr<AtomArg>& b) {
            if (!a || !b) return false;
            return a->getToken() < b->getToken();
        });
}

void Arg::sortByToken() {
    std::sort(args.begin(), args.end(), 
        [](const std::shared_ptr<AtomArg>& a, const std::shared_ptr<AtomArg>& b) {
            if (!a || !b) return false;
            return a->getToken() < b->getToken();
        });
}

void Arg::reverse() {
    std::reverse(args.begin(), args.end());
}

void Arg::merge(const Arg& other) {
    for (const auto& arg : other.args) {
        if (arg) {
            args.push_back(arg);
        }
    }
    
    // 合并绑定
    for (const auto& binding : other.bindings) {
        bindings[binding.first] = binding.second;
    }
}

void Arg::merge(const std::vector<std::string>& tokens) {
    for (const auto& token : tokens) {
        addToken(token);
    }
}

std::vector<Arg> Arg::splitByPlaceholder(const std::string& placeholder) const {
    std::vector<Arg> result;
    Arg current;
    
    for (const auto& arg : args) {
        if (arg && arg->getToken() == placeholder) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current.addArg(arg);
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

std::vector<Arg> Arg::splitByToken(const std::string& token) const {
    std::vector<Arg> result;
    Arg current;
    
    for (const auto& arg : args) {
        if (arg && arg->getToken() == token) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current.addArg(arg);
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

Arg Arg::filterByPlaceholder(const std::string& placeholder) const {
    Arg result;
    
    for (const auto& arg : args) {
        if (arg && arg->getToken() == placeholder) {
            result.addArg(arg);
        }
    }
    
    return result;
}

Arg Arg::filterByToken(const std::string& token) const {
    Arg result;
    
    for (const auto& arg : args) {
        if (arg && arg->getToken() == token) {
            result.addArg(arg);
        }
    }
    
    return result;
}

Arg Arg::filterByIndex(const std::vector<size_t>& indices) const {
    Arg result;
    
    for (size_t index : indices) {
        if (index < args.size()) {
            result.addArg(args[index]);
        }
    }
    
    return result;
}

void Arg::replacePlaceholder(const std::string& oldPlaceholder, const std::string& newPlaceholder) {
    for (auto& arg : args) {
        if (arg && arg->getToken() == oldPlaceholder) {
            arg->setToken(newPlaceholder);
        }
    }
}

void Arg::replaceToken(const std::string& oldToken, const std::string& newToken) {
    for (auto& arg : args) {
        if (arg && arg->getToken() == oldToken) {
            arg->setToken(newToken);
        }
    }
}

std::string Arg::format() const {
    return format(" ");
}

std::string Arg::format(const std::string& separator) const {
    std::ostringstream oss;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            oss << separator;
        }
        if (args[i]) {
            oss << args[i]->getToken();
        }
    }
    
    return oss.str();
}

std::string Arg::format(const std::string& prefix, const std::string& suffix, const std::string& separator) const {
    std::ostringstream oss;
    oss << prefix;
    oss << format(separator);
    oss << suffix;
    return oss.str();
}

void Arg::print() const {
    std::cout << "Arg: [";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) std::cout << ", ";
        if (args[i]) {
            std::cout << "\"" << args[i]->getToken() << "\"";
        }
    }
    std::cout << "]" << std::endl;
}

void Arg::printBindings() const {
    std::cout << "Bindings:" << std::endl;
    for (const auto& binding : bindings) {
        std::cout << "  " << binding.first << " -> [function]" << std::endl;
    }
}

void Arg::printMatchCounters() const {
    std::cout << "Match Counters:" << std::endl;
    for (const auto& counter : matchCounters) {
        std::cout << "  " << counter.first << ": " << counter.second << std::endl;
    }
}

void Arg::printStatistics() const {
    std::cout << "Arg Statistics:" << std::endl;
    std::cout << "  Size: " << size() << std::endl;
    std::cout << "  Bindings: " << bindings.size() << std::endl;
    std::cout << "  Placeholders: " << getAllPlaceholders().size() << std::endl;
    std::cout << "  Required: " << (hasRequiredPlaceholders() ? "Yes" : "No") << std::endl;
    std::cout << "  Optional: " << (hasOptionalPlaceholders() ? "Yes" : "No") << std::endl;
    std::cout << "  Unordered: " << (hasUnorderedPlaceholders() ? "Yes" : "No") << std::endl;
}

std::shared_ptr<AtomArg> Arg::operator[](size_t index) const {
    return getArg(index);
}

Arg& Arg::operator+=(const Arg& other) {
    merge(other);
    return *this;
}

Arg Arg::operator+(const Arg& other) const {
    Arg result = *this;
    result.merge(other);
    return result;
}

bool Arg::operator==(const Arg& other) const {
    if (size() != other.size()) {
        return false;
    }
    
    for (size_t i = 0; i < size(); ++i) {
        if (args[i] && other.args[i]) {
            if (args[i]->getToken() != other.args[i]->getToken()) {
                return false;
            }
        } else if (args[i] != other.args[i]) {
            return false;
        }
    }
    
    return true;
}

bool Arg::operator!=(const Arg& other) const {
    return !(*this == other);
}

std::vector<std::shared_ptr<AtomArg>>::iterator Arg::begin() {
    return args.begin();
}

std::vector<std::shared_ptr<AtomArg>>::iterator Arg::end() {
    return args.end();
}

std::vector<std::shared_ptr<AtomArg>>::const_iterator Arg::begin() const {
    return args.begin();
}

std::vector<std::shared_ptr<AtomArg>>::const_iterator Arg::end() const {
    return args.end();
}

} // namespace CJMOD
} // namespace CHTL
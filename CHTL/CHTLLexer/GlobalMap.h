#ifndef GLOBAL_MAP_H
#define GLOBAL_MAP_H

#include <string>
#include <unordered_map>
#include <any>

namespace CHTL {

// A global map for storing parser-level information like templates, variables, etc.
// This might be better designed as a "Context" object passed around, rather than a true global singleton.
class GlobalMap {
public:
    void store(const std::string& key, std::any value);
    std::any retrieve(const std::string& key);

private:
    std::unordered_map<std::string, std::any> m_map;
};

} // namespace CHTL

#endif // GLOBAL_MAP_H

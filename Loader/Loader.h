#pragma once

#include <string>

class Loader {
public:
    Loader() = default;
    std::string readTextFile(const std::string& path);
};

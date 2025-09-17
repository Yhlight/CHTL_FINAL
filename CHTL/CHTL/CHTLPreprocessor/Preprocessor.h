#pragma once

#include <string>
#include <vector>
#include <set>

class Preprocessor {
public:
    Preprocessor() = default;
    std::string process(const std::string& source, const std::string& currentFilePath);

private:
    std::string readFile(const std::string& path);
    std::string getBasePath(const std::string& filePath);

    // To prevent circular imports
    std::set<std::string> includedFiles;
};

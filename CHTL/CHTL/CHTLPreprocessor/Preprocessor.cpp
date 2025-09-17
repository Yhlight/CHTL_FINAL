#include "Preprocessor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

std::string Preprocessor::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        // It's an error, but maybe the path is relative. The caller should handle this.
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Preprocessor::getBasePath(const std::string& filePath) {
    size_t last_slash_idx = filePath.rfind('/');
    if (std::string::npos != last_slash_idx) {
        return filePath.substr(0, last_slash_idx + 1);
    }
    return "";
}


std::string Preprocessor::process(const std::string& source, const std::string& currentFilePath) {
    if (includedFiles.count(currentFilePath)) {
        // Circular dependency detected
        return "";
    }
    includedFiles.insert(currentFilePath);

    std::string processedSource = source;
    std::regex importRegex(R"(\[Import\]\s*@Chtl\s*from\s*["']([^"']+)["'];)");

    std::smatch match;
    while (std::regex_search(processedSource, match, importRegex)) {
        std::string importStatement = match[0].str();
        std::string relativePath = match[1].str();

        std::string basePath = getBasePath(currentFilePath);
        std::string absolutePath = basePath + relativePath;

        std::string fileContent = readFile(absolutePath);
        if (fileContent.empty()) {
            throw std::runtime_error("Could not read import file: " + absolutePath);
        }

        // Recursively process the imported file
        std::string processedContent = process(fileContent, absolutePath);

        // Replace the import statement with the processed content
        size_t pos = processedSource.find(importStatement);
        if (pos != std::string::npos) {
            processedSource.replace(pos, importStatement.length(), processedContent);
        }
    }

    // After processing this file, it can be included again by other files
    includedFiles.erase(currentFilePath);
    return processedSource;
}

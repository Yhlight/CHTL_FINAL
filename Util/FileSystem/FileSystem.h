#ifndef CHTL_FILESYSTEM_H
#define CHTL_FILESYSTEM_H

#include <string>

namespace CHTL
{
    class FileSystem
    {
    public:
        // Read the entire content of a file into a string
        static std::string readFile(const std::string &path);

        // Write a string to a file
        static bool writeFile(const std::string &path, const std::string &content);

        // Check if a file exists
        static bool fileExists(const std::string &path);
    };
}

#endif // CHTL_FILESYSTEM_H

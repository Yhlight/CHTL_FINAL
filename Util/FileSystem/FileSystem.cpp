#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace CHTL
{
    std::string FileSystem::readFile(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            // In a real application, you might throw an exception here
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    bool FileSystem::writeFile(const std::string &path, const std::string &content)
    {
        std::ofstream file(path);
        if (!file.is_open())
        {
            return false;
        }
        file << content;
        return true;
    }

    bool FileSystem::fileExists(const std::string &path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }
}

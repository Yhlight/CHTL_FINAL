#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

namespace CHTL {

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string FileSystem::getDirectory(const std::string& path) {
    size_t found = path.find_last_of("/\\");
    if (found != std::string::npos) {
        return path.substr(0, found + 1);
    }
    return "";
}

void FileSystem::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file for writing: " << path << std::endl;
        return;
    }
    file << content;
}

bool FileSystem::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool FileSystem::isDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

std::vector<std::string> FileSystem::listDirectory(const std::string& path) {
    std::vector<std::string> files;
#ifdef _WIN32
    std::string search_path = path + "/*.*";
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                files.push_back(fd.cFileName);
            } else {
                 // Also add directories, but ignore '.' and '..'
                if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {
                    files.push_back(fd.cFileName);
                }
            }
        } while (::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
#else
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string file_name = ent->d_name;
            if (file_name != "." && file_name != "..") {
                files.push_back(file_name);
            }
        }
        closedir(dir);
    }
#endif
    return files;
}

} // namespace CHTL

#ifndef ZIPUTIL_H
#define ZIPUTIL_H

#include <string>
#include <vector>
#include <map>

class ZipUtil {
public:
    static bool compress(const std::string& inputPath, const std::string& outputPath);
    static bool decompress(const std::string& inputPath, const std::string& outputPath);
    static std::vector<std::string> listFiles(const std::string& zipPath);
    static bool extractFile(const std::string& zipPath, const std::string& filePath, const std::string& outputPath);
    static bool addFile(const std::string& zipPath, const std::string& filePath, const std::string& content);
    static bool removeFile(const std::string& zipPath, const std::string& filePath);
    static std::map<std::string, std::string> getFileContents(const std::string& zipPath);
    static bool isValidZip(const std::string& zipPath);
    static size_t getFileSize(const std::string& zipPath);
    static std::string getFileInfo(const std::string& zipPath);
};

#endif // ZIPUTIL_H
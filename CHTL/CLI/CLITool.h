#ifndef CHTL_CLI_TOOL_H
#define CHTL_CLI_TOOL_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <functional>

namespace CHTL {

// 命令行参数解析器
class CLIParser {
public:
    struct Option {
        std::string name;
        std::string shortName;
        std::string description;
        bool hasValue;
        std::string defaultValue;
        bool required;
    };

    CLIParser();
    ~CLIParser() = default;

    void addOption(const Option& option);
    bool parse(int argc, char* argv[]);
    std::string getValue(const std::string& name) const;
    bool hasOption(const std::string& name) const;
    void printHelp() const;
    void printVersion() const;

private:
    std::vector<Option> options;
    std::map<std::string, std::string> values;
    std::string programName;
    std::string version;
    std::string description;
};

// 颜色输出支持
class ColorOutput {
public:
    enum Color {
        RED = 31,
        GREEN = 32,
        YELLOW = 33,
        BLUE = 34,
        MAGENTA = 35,
        CYAN = 36,
        WHITE = 37,
        RESET = 0
    };

    static void setColor(std::ostream& os, Color color);
    static void resetColor(std::ostream& os);
    static void printColored(std::ostream& os, const std::string& text, Color color);
    static void printError(const std::string& message);
    static void printSuccess(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
};

// 文件处理器
class FileProcessor {
public:
    static bool fileExists(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static std::string getFileExtension(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getDirectory(const std::string& path);
    static bool createDirectory(const std::string& path);
    static std::vector<std::string> listFiles(const std::string& directory, const std::string& extension = "");
};

// 进度条显示
class ProgressBar {
public:
    ProgressBar(int total, int width = 50);
    void update(int current);
    void finish();
    void reset();

private:
    int total;
    int width;
    int current;
    bool finished;
};

// 主CLI工具类
class CLITool {
public:
    CLITool();
    ~CLITool() = default;

    int run(int argc, char* argv[]);
    void setVersion(const std::string& version);
    void setDescription(const std::string& description);

private:
    void initializeParser();
    void processFile(const std::string& inputFile, const std::string& outputFile);
    void processDirectory(const std::string& inputDir, const std::string& outputDir);
    void processBatch(const std::vector<std::string>& files);
    void showHelp();
    void showVersion();
    void showStatus();
    void showConfig();
    void showInfo();
    void showExamples();
    void showLicense();
    void showCredits();
    
    // 高级功能
    void watchMode();
    void serverMode();
    void interactiveMode();
    void benchmarkMode();
    void validateMode();
    void formatMode();
    void minifyMode();
    void beautifyMode();
    
    // 配置管理
    void loadConfig(const std::string& configFile);
    void saveConfig(const std::string& configFile);
    void resetConfig();
    void showConfigHelp();
    
    // 输出管理
    void setupOutput();
    void cleanupOutput();
    void redirectOutput();
    
    std::unique_ptr<CLIParser> parser;
    std::string version;
    std::string description;
    bool verbose;
    bool debug;
    bool force;
    bool watch;
    bool server;
    bool interactive;
    bool benchmark;
    bool validate;
    bool format;
    bool minify;
    bool beautify;
    std::string configFile;
    std::string outputFormat;
    std::string outputDir;
    int serverPort;
    std::string logFile;
    std::string errorFile;
};

} // namespace CHTL

#endif // CHTL_CLI_TOOL_H
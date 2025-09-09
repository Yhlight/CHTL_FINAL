#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace CHTL {

/**
 * @brief 命令行参数类型
 */
enum class CliArgType {
    STRING,         // 字符串参数
    INTEGER,        // 整数参数
    BOOLEAN,        // 布尔参数
    FILE,           // 文件参数
    DIRECTORY       // 目录参数
};

/**
 * @brief 命令行参数
 */
class CliArgument {
public:
    CliArgument(const std::string& name, const std::string& description, 
                CliArgType type, bool required = false, const std::string& defaultValue = "");
    ~CliArgument() = default;

    std::string getName() const { return m_name; }
    std::string getDescription() const { return m_description; }
    CliArgType getType() const { return m_type; }
    bool isRequired() const { return m_required; }
    std::string getDefaultValue() const { return m_defaultValue; }
    
    void setValue(const std::string& value) { m_value = value; }
    std::string getValue() const { return m_value.empty() ? m_defaultValue : m_value; }
    bool hasValue() const { return !m_value.empty() || !m_defaultValue.empty(); }

private:
    std::string m_name;
    std::string m_description;
    CliArgType m_type;
    bool m_required;
    std::string m_defaultValue;
    std::string m_value;
};

/**
 * @brief 命令行选项
 */
class CliOption {
public:
    CliOption(const std::string& shortName, const std::string& longName, 
              const std::string& description, CliArgType type = CliArgType::BOOLEAN, 
              bool required = false, const std::string& defaultValue = "");
    ~CliOption() = default;

    std::string getShortName() const { return m_shortName; }
    std::string getLongName() const { return m_longName; }
    std::string getDescription() const { return m_description; }
    CliArgType getType() const { return m_type; }
    bool isRequired() const { return m_required; }
    std::string getDefaultValue() const { return m_defaultValue; }
    
    void setValue(const std::string& value) { m_value = value; }
    std::string getValue() const { return m_value.empty() ? m_defaultValue : m_value; }
    bool hasValue() const { return !m_value.empty() || !m_defaultValue.empty(); }
    bool isSet() const { return m_isSet; }
    void setSet(bool isSet) { m_isSet = isSet; }

private:
    std::string m_shortName;
    std::string m_longName;
    std::string m_description;
    CliArgType m_type;
    bool m_required;
    std::string m_defaultValue;
    std::string m_value;
    bool m_isSet;
};

/**
 * @brief 命令行解析器
 */
class CliParser {
public:
    explicit CliParser(std::shared_ptr<CHTLContext> context);
    ~CliParser() = default;

    // 参数和选项管理
    void addArgument(const std::string& name, const std::string& description, 
                    CliArgType type, bool required = false, const std::string& defaultValue = "");
    void addOption(const std::string& shortName, const std::string& longName, 
                  const std::string& description, CliArgType type = CliArgType::BOOLEAN, 
                  bool required = false, const std::string& defaultValue = "");
    
    // 解析命令行
    bool parse(int argc, char* argv[]);
    bool parse(const std::vector<std::string>& args);
    
    // 获取参数和选项
    std::string getArgument(const std::string& name) const;
    std::string getOption(const std::string& name) const;
    bool hasOption(const std::string& name) const;
    bool hasArgument(const std::string& name) const;
    
    // 验证
    bool validate() const;
    std::vector<std::string> getErrors() const;
    
    // 帮助信息
    std::string generateHelp() const;
    std::string generateUsage() const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<CliArgument>> m_arguments;
    std::unordered_map<std::string, std::shared_ptr<CliOption>> m_options;
    std::vector<std::string> m_errors;
    
    // 辅助方法
    bool parseArgument(const std::string& arg);
    bool parseOption(const std::string& option, const std::string& value = "");
    bool validateArgument(const std::string& name, const std::string& value) const;
    bool validateOption(const std::string& name, const std::string& value) const;
    std::string formatArgument(const std::string& name) const;
    std::string formatOption(const std::string& name) const;
};

/**
 * @brief 命令行处理器
 */
class CliProcessor {
public:
    explicit CliProcessor(std::shared_ptr<CHTLContext> context);
    ~CliProcessor() = default;

    // 命令处理
    void processCommand(const std::string& command, const std::vector<std::string>& args);
    void processFile(const std::string& inputFile, const std::string& outputFile = "");
    void processDirectory(const std::string& inputDir, const std::string& outputDir = "");
    
    // 编译处理
    void compileFile(const std::string& inputFile, const std::string& outputFile = "");
    void compileDirectory(const std::string& inputDir, const std::string& outputDir = "");
    
    // 验证处理
    void validateFile(const std::string& inputFile);
    void validateDirectory(const std::string& inputDir);
    
    // 格式化处理
    void formatFile(const std::string& inputFile, const std::string& outputFile = "");
    void formatDirectory(const std::string& inputDir, const std::string& outputDir = "");
    
    // 模块处理
    void createModule(const std::string& moduleName, const std::string& moduleType);
    void buildModule(const std::string& modulePath);
    void installModule(const std::string& modulePath);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::string getOutputPath(const std::string& inputPath, const std::string& outputPath = "") const;
    bool isCHTLFile(const std::string& filePath) const;
    bool isCMODFile(const std::string& filePath) const;
    bool isCJMODFile(const std::string& filePath) const;
    std::string getFileExtension(const std::string& filePath) const;
    std::vector<std::string> findCHTLFiles(const std::string& directory) const;
};

/**
 * @brief 命令行应用程序
 */
class CliApplication {
public:
    explicit CliApplication(std::shared_ptr<CHTLContext> context);
    ~CliApplication() = default;

    // 应用程序管理
    void initialize();
    void run(int argc, char* argv[]);
    void shutdown();
    
    // 命令注册
    void registerCommand(const std::string& name, const std::string& description, 
                        std::function<void(const std::vector<std::string>&)> handler);
    void registerDefaultCommands();
    
    // 命令执行
    void executeCommand(const std::string& command, const std::vector<std::string>& args);
    bool hasCommand(const std::string& command) const;
    
    // 帮助系统
    void showHelp();
    void showHelp(const std::string& command);
    void showVersion();
    void showUsage();

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<CliParser> m_parser;
    std::unique_ptr<CliProcessor> m_processor;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> m_commands;
    std::unordered_map<std::string, std::string> m_commandDescriptions;
    
    // 默认命令
    void cmdCompile(const std::vector<std::string>& args);
    void cmdValidate(const std::vector<std::string>& args);
    void cmdFormat(const std::vector<std::string>& args);
    void cmdModule(const std::vector<std::string>& args);
    void cmdHelp(const std::vector<std::string>& args);
    void cmdVersion(const std::vector<std::string>& args);
};

/**
 * @brief 命令行渲染器
 */
class CliRenderer {
public:
    explicit CliRenderer(std::shared_ptr<CHTLContext> context);
    ~CliRenderer() = default;

    // 渲染功能
    void renderFile(const std::string& inputFile, const std::string& outputFile = "");
    void renderDirectory(const std::string& inputDir, const std::string& outputDir = "");
    
    // 预览功能
    void previewFile(const std::string& inputFile);
    void previewDirectory(const std::string& inputDir);
    
    // 实时渲染
    void startLiveRender(const std::string& inputFile);
    void stopLiveRender();
    bool isLiveRendering() const;

private:
    std::shared_ptr<CHTLContext> m_context;
    bool m_isLiveRendering;
    std::string m_currentFile;
    
    // 辅助方法
    std::string generatePreview(const std::string& inputFile) const;
    void watchFile(const std::string& filePath);
    void onFileChanged(const std::string& filePath);
};

} // namespace CHTL
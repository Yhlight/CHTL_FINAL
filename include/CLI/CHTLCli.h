#ifndef CHTL_CLI_H
#define CHTL_CLI_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Compiler/CompilerDispatcher.h"
#include "Module/CMODSystem.h"
#include "Module/CJMODSystem.h"

namespace CHTL {

/**
 * @brief CLI命令类型
 */
enum class CommandType {
    COMPILE,        // 编译CHTL文件
    WATCH,          // 监视文件变化
    MODULE,         // 模块管理
    HELP,           // 帮助信息
    VERSION,        // 版本信息
    INIT,           // 初始化项目
    BUILD,          // 构建项目
    CLEAN,          // 清理构建文件
    SERVE,          // 启动开发服务器
    UNKNOWN         // 未知命令
};

/**
 * @brief CLI选项结构
 */
struct CLIOptions {
    bool verbose = false;           // 详细输出
    bool debug = false;            // 调试模式
    bool watch = false;            // 监视模式
    bool minify = false;           // 压缩输出
    bool source_map = false;       // 生成源码映射
    bool hot_reload = false;       // 热重载
    std::string output_dir = ".";  // 输出目录
    std::string config_file = "";  // 配置文件
    std::vector<std::string> include_paths;  // 包含路径
    std::vector<std::string> module_paths;   // 模块路径
    std::string target = "html";   // 编译目标
    std::string format = "pretty"; // 输出格式
};

/**
 * @brief CLI命令结构
 */
struct CLICommand {
    CommandType type;
    std::string name;
    std::string description;
    std::vector<std::string> aliases;
    std::vector<std::string> arguments;
    std::map<std::string, std::string> options;
};

/**
 * @brief CHTL CLI类
 * 提供完整的命令行界面
 */
class CHTLCli {
public:
    CHTLCli();
    ~CHTLCli() = default;
    
    /**
     * @brief 解析命令行参数
     * @param argc 参数个数
     * @param argv 参数数组
     * @return 解析的命令类型
     */
    CommandType parseArguments(int argc, char* argv[]);
    
    /**
     * @brief 执行命令
     * @param command_type 命令类型
     * @return 执行结果
     */
    int executeCommand(CommandType command_type);
    
    /**
     * @brief 编译文件
     * @param input_file 输入文件
     * @param output_file 输出文件
     * @return 执行结果
     */
    int compileFile(const std::string& input_file, const std::string& output_file = "");
    
    /**
     * @brief 编译目录
     * @param input_dir 输入目录
     * @param output_dir 输出目录
     * @return 执行结果
     */
    int compileDirectory(const std::string& input_dir, const std::string& output_dir);
    
    /**
     * @brief 监视文件变化
     * @param input_path 输入路径
     * @return 执行结果
     */
    int watchFiles(const std::string& input_path);
    
    /**
     * @brief 管理模块
     * @param action 操作类型
     * @param module_name 模块名称
     * @return 执行结果
     */
    int manageModule(const std::string& action, const std::string& module_name = "");
    
    /**
     * @brief 初始化项目
     * @param project_name 项目名称
     * @return 执行结果
     */
    int initializeProject(const std::string& project_name);
    
    /**
     * @brief 构建项目
     * @param project_dir 项目目录
     * @return 执行结果
     */
    int buildProject(const std::string& project_dir);
    
    /**
     * @brief 清理构建文件
     * @param project_dir 项目目录
     * @return 执行结果
     */
    int cleanProject(const std::string& project_dir);
    
    /**
     * @brief 启动开发服务器
     * @param port 端口号
     * @param root_dir 根目录
     * @return 执行结果
     */
    int startServer(int port = 8080, const std::string& root_dir = ".");
    
    /**
     * @brief 显示帮助信息
     * @param command 特定命令的帮助（可选）
     * @return 执行结果
     */
    int showHelp(const std::string& command = "");
    
    /**
     * @brief 显示版本信息
     * @return 执行结果
     */
    int showVersion();
    
    /**
     * @brief 设置选项
     * @param options CLI选项
     */
    void setOptions(const CLIOptions& options);
    
    /**
     * @brief 获取选项
     * @return CLI选项
     */
    const CLIOptions& getOptions() const;
    
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 获取错误信息
     * @return 错误信息列表
     */
    std::vector<std::string> getErrors() const;
    
    /**
     * @brief 清除错误信息
     */
    void clearErrors();

private:
    CLIOptions options_;
    std::vector<std::string> args_;
    std::vector<std::string> errors_;
    bool debug_mode_;
    
    // 编译器组件
    std::unique_ptr<CompilerDispatcher> compiler_;
    std::unique_ptr<CMODSystem> cmod_system_;
    std::unique_ptr<CJMODSystem> cjmod_system_;
    
    // 命令定义
    std::vector<CLICommand> commands_;
    
    /**
     * @brief 初始化命令列表
     */
    void initializeCommands();
    
    /**
     * @brief 添加错误信息
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * @brief 解析选项
     * @param arg 参数
     * @return 是否成功解析
     */
    bool parseOption(const std::string& arg);
    
    /**
     * @brief 查找命令
     * @param name 命令名称
     * @return 命令指针，未找到返回nullptr
     */
    const CLICommand* findCommand(const std::string& name);
    
    /**
     * @brief 验证文件路径
     * @param file_path 文件路径
     * @return 是否有效
     */
    bool validateFilePath(const std::string& file_path);
    
    /**
     * @brief 创建输出目录
     * @param dir_path 目录路径
     * @return 是否成功
     */
    bool createOutputDirectory(const std::string& dir_path);
    
    /**
     * @brief 获取文件扩展名
     * @param file_path 文件路径
     * @return 扩展名
     */
    std::string getFileExtension(const std::string& file_path);
    
    /**
     * @brief 生成输出文件名
     * @param input_file 输入文件
     * @param extension 输出扩展名
     * @return 输出文件名
     */
    std::string generateOutputFileName(const std::string& input_file, const std::string& extension);
    
    /**
     * @brief 加载配置文件
     * @param config_file 配置文件路径
     * @return 是否成功
     */
    bool loadConfigFile(const std::string& config_file);
    
    /**
     * @brief 保存配置文件
     * @param config_file 配置文件路径
     * @return 是否成功
     */
    bool saveConfigFile(const std::string& config_file);
    
    /**
     * @brief 扫描目录中的CHTL文件
     * @param dir_path 目录路径
     * @return CHTL文件列表
     */
    std::vector<std::string> scanCHTLFiles(const std::string& dir_path);
    
    /**
     * @brief 监视文件变化（实现）
     * @param file_path 文件路径
     * @param callback 变化回调
     */
    void watchFile(const std::string& file_path, std::function<void(const std::string&)> callback);
};

} // namespace CHTL

#endif // CHTL_CLI_H
#ifndef CHTL_CMOD_SYSTEM_H
#define CHTL_CMOD_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>

namespace CHTL {

/**
 * @brief CMOD模块信息结构
 */
struct CMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    
    // 导出表
    std::unordered_map<std::string, std::string> exports;
};

/**
 * @brief CMOD模块结构
 */
struct CMODModule {
    std::string name;
    std::string path;
    CMODInfo info;
    std::string source_code;
    std::vector<std::shared_ptr<CMODModule>> submodules;
    bool is_loaded;
    
    CMODModule() : is_loaded(false) {}
};

/**
 * @brief CMOD系统类
 * 负责管理CMOD模块的加载、解析和查询
 */
class CMODSystem {
public:
    CMODSystem();
    ~CMODSystem() = default;
    
    /**
     * @brief 初始化CMOD系统
     * @param official_module_path 官方模块路径
     * @param user_module_path 用户模块路径
     */
    void initialize(const std::string& official_module_path, const std::string& user_module_path);
    
    /**
     * @brief 加载CMOD模块
     * @param module_name 模块名称
     * @return 加载的模块指针，失败返回nullptr
     */
    std::shared_ptr<CMODModule> loadModule(const std::string& module_name);
    
    /**
     * @brief 加载CMOD模块（通过路径）
     * @param module_path 模块路径
     * @return 加载的模块指针，失败返回nullptr
     */
    std::shared_ptr<CMODModule> loadModuleFromPath(const std::string& module_path);
    
    /**
     * @brief 解析CMOD模块信息
     * @param module_path 模块路径
     * @return 模块信息，失败返回空信息
     */
    CMODInfo parseModuleInfo(const std::string& module_path);
    
    /**
     * @brief 解析CMOD模块源码
     * @param module_path 模块路径
     * @return 源码内容，失败返回空字符串
     */
    std::string parseModuleSource(const std::string& module_path);
    
    /**
     * @brief 搜索模块
     * @param module_name 模块名称
     * @return 找到的模块路径，未找到返回空字符串
     */
    std::string searchModule(const std::string& module_name);
    
    /**
     * @brief 获取模块的导出表
     * @param module_name 模块名称
     * @return 导出表，失败返回空表
     */
    std::unordered_map<std::string, std::string> getModuleExports(const std::string& module_name);
    
    /**
     * @brief 检查模块是否存在
     * @param module_name 模块名称
     * @return 是否存在
     */
    bool moduleExists(const std::string& module_name);
    
    /**
     * @brief 获取所有已加载的模块
     * @return 已加载模块列表
     */
    std::vector<std::shared_ptr<CMODModule>> getLoadedModules() const;
    
    /**
     * @brief 清除所有已加载的模块
     */
    void clearLoadedModules();
    
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
    std::string official_module_path_;
    std::string user_module_path_;
    std::unordered_map<std::string, std::shared_ptr<CMODModule>> loaded_modules_;
    std::vector<std::string> errors_;
    bool debug_mode_;
    
    /**
     * @brief 添加错误信息
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * @brief 搜索模块文件
     * @param module_name 模块名称
     * @param search_paths 搜索路径列表
     * @return 找到的模块路径
     */
    std::string searchModuleInPaths(const std::string& module_name, 
                                   const std::vector<std::string>& search_paths);
    
    /**
     * @brief 解析模块信息文件
     * @param info_file_path 信息文件路径
     * @return 解析的模块信息
     */
    CMODInfo parseInfoFile(const std::string& info_file_path);
    
    /**
     * @brief 解析导出表
     * @param source_code 源码内容
     * @return 导出表
     */
    std::unordered_map<std::string, std::string> parseExports(const std::string& source_code);
    
    /**
     * @brief 加载子模块
     * @param parent_module 父模块
     * @param module_path 模块路径
     */
    void loadSubmodules(std::shared_ptr<CMODModule> parent_module, const std::string& module_path);
    
    /**
     * @brief 检查文件是否存在
     * @param file_path 文件路径
     * @return 是否存在
     */
    bool fileExists(const std::string& file_path);
    
    /**
     * @brief 检查目录是否存在
     * @param dir_path 目录路径
     * @return 是否存在
     */
    bool directoryExists(const std::string& dir_path);
    
    /**
     * @brief 获取目录下的所有文件
     * @param dir_path 目录路径
     * @param extension 文件扩展名（可选）
     * @return 文件列表
     */
    std::vector<std::string> getFilesInDirectory(const std::string& dir_path, 
                                                const std::string& extension = "");
    
    /**
     * @brief 读取文件内容
     * @param file_path 文件路径
     * @return 文件内容
     */
    std::string readFile(const std::string& file_path);
};

} // namespace CHTL

#endif // CHTL_CMOD_SYSTEM_H
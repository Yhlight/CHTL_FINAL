#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <filesystem>

namespace chtl {
namespace import {

// 导入类型
enum class ImportType {
    CHTL,           // CHTL文件
    HTML,           // HTML文件
    CSS,            // CSS文件
    JAVASCRIPT,     // JavaScript文件
    CJMOD,          // CHTL JS模块
    UNKNOWN         // 未知类型
};

// 导入模式
enum class ImportMode {
    PRECISE,        // 精确导入
    TYPE,           // 类型导入
    WILDCARD        // 通配符导入
};

// 导入项
struct ImportItem {
    std::string name;           // 导入的名称
    std::string alias;          // 别名
    ImportType type;            // 导入类型
    std::string source_path;    // 源文件路径
    std::string target_path;    // 目标路径
    bool is_exported;           // 是否被导出
    
    ImportItem(const std::string& n, ImportType t, const std::string& src)
        : name(n), type(t), source_path(src), is_exported(false) {}
};

// 命名空间
class Namespace {
private:
    std::string name_;
    std::string parent_name_;
    std::unordered_map<std::string, std::shared_ptr<ImportItem>> items_;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> sub_namespaces_;
    
public:
    Namespace(const std::string& name, const std::string& parent = "");
    
    // 命名空间管理
    void add_item(const std::string& name, std::shared_ptr<ImportItem> item);
    std::shared_ptr<ImportItem> get_item(const std::string& name) const;
    bool has_item(const std::string& name) const;
    
    void add_sub_namespace(const std::string& name, std::shared_ptr<Namespace> ns);
    std::shared_ptr<Namespace> get_sub_namespace(const std::string& name) const;
    bool has_sub_namespace(const std::string& name) const;
    
    // 属性访问
    const std::string& get_name() const { return name_; }
    const std::string& get_parent_name() const { return parent_name_; }
    const std::unordered_map<std::string, std::shared_ptr<ImportItem>>& get_items() const { return items_; }
    const std::unordered_map<std::string, std::shared_ptr<Namespace>>& get_sub_namespaces() const { return sub_namespaces_; }
    
    // 合并命名空间
    void merge_with(std::shared_ptr<Namespace> other);
    
    // 调试
    void print_structure(int indent = 0) const;
};

// 路径解析器
class PathResolver {
private:
    std::string base_path_;
    std::vector<std::string> search_paths_;
    
public:
    PathResolver(const std::string& base_path);
    
    // 路径解析
    std::string resolve_path(const std::string& path) const;
    std::string normalize_path(const std::string& path) const;
    bool is_absolute_path(const std::string& path) const;
    bool is_relative_path(const std::string& path) const;
    
    // 路径等价处理
    std::string convert_dot_to_slash(const std::string& path) const;
    std::string convert_slash_to_dot(const std::string& path) const;
    
    // 搜索路径管理
    void add_search_path(const std::string& path);
    std::vector<std::string> find_files(const std::string& pattern) const;
    
    // 文件类型检测
    ImportType detect_file_type(const std::string& file_path) const;
    
    // 工具方法
    std::string get_file_extension(const std::string& file_path) const;
    std::string get_file_name(const std::string& file_path) const;
    std::string get_directory(const std::string& file_path) const;
};

// 导入管理器
class ImportManager {
private:
    std::string base_path_;
    std::shared_ptr<PathResolver> path_resolver_;
    std::shared_ptr<Namespace> global_namespace_;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> namespaces_;
    std::unordered_set<std::string> processed_files_;
    std::unordered_map<std::string, std::shared_ptr<ast::ASTNode>> imported_asts_;
    
    // 文件处理
    std::string read_file(const std::string& file_path) const;
    bool file_exists(const std::string& file_path) const;
    
    // 导入处理
    void process_import_statement(const std::string& import_stmt, const std::string& current_file);
    void process_precise_import(const std::string& name, const std::string& path, ImportType type);
    void process_type_import(const std::string& type, const std::string& path);
    void process_wildcard_import(const std::string& path);
    
    // 命名空间处理
    std::shared_ptr<Namespace> get_or_create_namespace(const std::string& name);
    void merge_namespaces(const std::string& name, std::shared_ptr<Namespace> ns);
    
    // 依赖解析
    void resolve_dependencies(const std::string& file_path);
    
public:
    ImportManager(const std::string& base_path);
    
    // 主要接口
    void process_file(const std::string& file_path);
    void process_imports(const std::string& content, const std::string& current_file);
    void process_import_node(std::shared_ptr<ast::ImportNode> node);
    
    // 导入管理
    void add_import(const std::string& name, const std::string& path, ImportType type, const std::string& alias = "");
    std::shared_ptr<ImportItem> get_import(const std::string& name, const std::string& namespace_name = "") const;
    std::vector<std::shared_ptr<ImportItem>> get_imports_by_type(ImportType type) const;
    
    // 命名空间管理
    std::shared_ptr<Namespace> get_namespace(const std::string& name) const;
    void set_default_namespace(const std::string& name);
    std::string get_default_namespace() const;
    
    // 路径管理
    void add_search_path(const std::string& path);
    std::string resolve_import_path(const std::string& import_path, const std::string& current_file) const;
    
    // 依赖管理
    std::vector<std::string> get_dependencies(const std::string& file_path) const;
    bool has_circular_dependency(const std::string& file_path) const;
    
    // 调试和诊断
    void print_imports() const;
    void print_namespaces() const;
    void print_dependencies() const;
    
    // 重置
    void clear();
    void reset();
};

} // namespace import
} // namespace chtl
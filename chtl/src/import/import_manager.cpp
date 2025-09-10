#include "import/import_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace import {

// Namespace 实现
Namespace::Namespace(const std::string& name, const std::string& parent)
    : name_(name), parent_name_(parent) {
}

void Namespace::add_item(const std::string& name, std::shared_ptr<ImportItem> item) {
    items_[name] = item;
}

std::shared_ptr<ImportItem> Namespace::get_item(const std::string& name) const {
    auto it = items_.find(name);
    return (it != items_.end()) ? it->second : nullptr;
}

bool Namespace::has_item(const std::string& name) const {
    return items_.find(name) != items_.end();
}

void Namespace::add_sub_namespace(const std::string& name, std::shared_ptr<Namespace> ns) {
    sub_namespaces_[name] = ns;
}

std::shared_ptr<Namespace> Namespace::get_sub_namespace(const std::string& name) const {
    auto it = sub_namespaces_.find(name);
    return (it != sub_namespaces_.end()) ? it->second : nullptr;
}

bool Namespace::has_sub_namespace(const std::string& name) const {
    return sub_namespaces_.find(name) != sub_namespaces_.end();
}

void Namespace::merge_with(std::shared_ptr<Namespace> other) {
    if (!other) return;
    
    // 合并项目
    for (const auto& pair : other->items_) {
        items_[pair.first] = pair.second;
    }
    
    // 合并子命名空间
    for (const auto& pair : other->sub_namespaces_) {
        if (has_sub_namespace(pair.first)) {
            get_sub_namespace(pair.first)->merge_with(pair.second);
        } else {
            add_sub_namespace(pair.first, pair.second);
        }
    }
}

void Namespace::print_structure(int indent) const {
    std::string indent_str(indent * 2, ' ');
    std::cout << indent_str << "Namespace: " << name_ << std::endl;
    
    for (const auto& pair : items_) {
        std::cout << indent_str << "  Item: " << pair.first << " (" << static_cast<int>(pair.second->type) << ")" << std::endl;
    }
    
    for (const auto& pair : sub_namespaces_) {
        pair.second->print_structure(indent + 1);
    }
}

// PathResolver 实现
PathResolver::PathResolver(const std::string& base_path) : base_path_(base_path) {
    search_paths_.push_back(base_path);
    search_paths_.push_back("modules"); // Add modules directory to search paths
}

std::string PathResolver::resolve_path(const std::string& path) const {
    if (is_absolute_path(path)) {
        return normalize_path(path);
    }
    
    // 相对路径，从基础路径开始解析
    std::string full_path = base_path_ + "/" + path;
    return normalize_path(full_path);
}

std::string PathResolver::normalize_path(const std::string& path) const {
    std::filesystem::path fs_path(path);
    return fs_path.lexically_normal().string();
}

bool PathResolver::is_absolute_path(const std::string& path) const {
    return std::filesystem::path(path).is_absolute();
}

bool PathResolver::is_relative_path(const std::string& path) const {
    return !is_absolute_path(path);
}

std::string PathResolver::convert_dot_to_slash(const std::string& path) const {
    std::string result = path;
    std::replace(result.begin(), result.end(), '.', '/');
    return result;
}

std::string PathResolver::convert_slash_to_dot(const std::string& path) const {
    std::string result = path;
    std::replace(result.begin(), result.end(), '/', '.');
    return result;
}

void PathResolver::add_search_path(const std::string& path) {
    search_paths_.push_back(path);
}

std::vector<std::string> PathResolver::find_files(const std::string& pattern) const {
    std::vector<std::string> results;
    
    for (const auto& search_path : search_paths_) {
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(search_path)) {
                if (entry.is_regular_file()) {
                    std::string file_path = entry.path().string();
                    std::string file_name = entry.path().filename().string();
                    
                    // 简单的模式匹配
                    if (file_name.find(pattern) != std::string::npos) {
                        results.push_back(file_path);
                    }
                }
            }
        } catch (const std::exception& e) {
            // 忽略无法访问的目录
        }
    }
    
    return results;
}

ImportType PathResolver::detect_file_type(const std::string& file_path) const {
    std::string ext = get_file_extension(file_path);
    
    if (ext == ".chtl") {
        return ImportType::CHTL;
    } else if (ext == ".html" || ext == ".htm") {
        return ImportType::HTML;
    } else if (ext == ".css") {
        return ImportType::CSS;
    } else if (ext == ".js") {
        return ImportType::JAVASCRIPT;
    } else if (ext == ".cjs") {
        return ImportType::CJMOD;
    }
    
    return ImportType::UNKNOWN;
}

std::string PathResolver::get_file_extension(const std::string& file_path) const {
    std::filesystem::path fs_path(file_path);
    std::string ext = fs_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

std::string PathResolver::get_file_name(const std::string& file_path) const {
    std::filesystem::path fs_path(file_path);
    return fs_path.stem().string();
}

std::string PathResolver::get_directory(const std::string& file_path) const {
    std::filesystem::path fs_path(file_path);
    return fs_path.parent_path().string();
}

// ImportManager 实现
ImportManager::ImportManager(const std::string& base_path) 
    : base_path_(base_path) {
    path_resolver_ = std::make_shared<PathResolver>(base_path);
    global_namespace_ = std::make_shared<Namespace>("global");
    namespaces_["global"] = global_namespace_;

    // --- Populate the virtual filesystem ---
    std::cout << "Initializing virtual filesystem with dummy modules..." << std::endl;
    auto chtholly_module = std::make_shared<cmod_cjmod::CMODModule>("Chtholly");
    cmod_cjmod::ModuleInfo info("Chtholly");
    info.version = "1.0.0-vfs";
    info.author = "VFS";
    info.exports.push_back("ChthollyComponent");
    chtholly_module->setInfo(info);
    chtholly_module->addSourceFile("src/Chtholly.chtl", "div { class: \"chtholly-component\"; text: \"Content from the VIRTUAL Chtholly module\"; }");

    std::string packed_chtholly = cmod_cjmod::ModulePackager::pack(*chtholly_module);
    virtual_filesystem_["modules/Chtholly.cmod"] = packed_chtholly;
    std::cout << "Packed 'Chtholly' module into virtual path: modules/Chtholly.cmod" << std::endl;
}

std::string ImportManager::read_file(const std::string& file_path) const {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + file_path);
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool ImportManager::file_exists(const std::string& file_path) const {
    return std::filesystem::exists(file_path);
}

void ImportManager::process_file(const std::string& file_path) {
    if (processed_files_.find(file_path) != processed_files_.end()) {
        return; // 已经处理过
    }
    
    processed_files_.insert(file_path);
    
    if (!file_exists(file_path)) {
        throw std::runtime_error("File not found: " + file_path);
    }
    
    std::string content = read_file(file_path);
    process_imports(content, file_path);
    resolve_dependencies(file_path);
}

void ImportManager::process_import_node(std::shared_ptr<ast::ImportNode> node, std::shared_ptr<ast::ASTNode> current_node) {
    if (!node || !current_node) {
        return;
    }

    // This function now only handles direct file imports. Module imports are handled by the resolver.
    if (node->import_type != ImportType::CHTL || !path_resolver_->get_file_extension(node->file_path).empty()) {
        std::string path_to_load = path_resolver_->resolve_path(node->file_path);

        if (processed_files_.count(path_to_load)) {
            return;
        }

        try {
            std::string content = read_file(path_to_load);
            processed_files_.insert(path_to_load);

            lexer::CHTLLexer lexer(content);
            auto tokens = lexer.tokenize();
            parser::CHTLParser parser(tokens);
            auto imported_ast = parser.parse();

            imported_asts_[path_to_load] = imported_ast;

            // Recursively resolve imports in the new AST
            ImportResolver resolver(*this);
            resolver.resolve_imports(imported_ast);

            // Replace the import node with the content of the imported file's AST
            if (imported_ast && current_node->parent) {
                auto parent = current_node->parent;
                // This is a simplified replacement. A real implementation would be more careful.
                // For now, we just add the children of the imported AST.
                for(const auto& child : imported_ast->children) {
                    parent->add_child(child);
                }
            }

        } catch (const std::exception& e) {
            std::cerr << "Error processing import for file '" << path_to_load << "': " << e.what() << std::endl;
        }
    }
}


// This method is now obsolete, the new resolver handles this.
void ImportManager::process_imports(const std::string& content, const std::string& current_file) {
    // This function is now handled by the ImportResolver and process_import_node
}

void ImportManager::process_precise_import(const std::string& name, const std::string& path, ImportType type) {
    auto item = std::make_shared<ImportItem>(name, type, path);
    global_namespace_->add_item(name, item);
}

void ImportManager::process_type_import(const std::string& type, const std::string& path) {
    // 类型导入处理
    std::string resolved_path = resolve_import_path(path, "");
    ImportType import_type = path_resolver_->detect_file_type(resolved_path);
    
    // 根据类型创建导入项
    std::string name = path_resolver_->get_file_name(resolved_path);
    auto item = std::make_shared<ImportItem>(name, import_type, resolved_path);
    global_namespace_->add_item(name, item);
}

void ImportManager::process_wildcard_import(const std::string& path) {
    // 通配符导入处理
    std::vector<std::string> files = path_resolver_->find_files("*");
    
    for (const auto& file : files) {
        std::string name = path_resolver_->get_file_name(file);
        ImportType type = path_resolver_->detect_file_type(file);
        
        auto item = std::make_shared<ImportItem>(name, type, file);
        global_namespace_->add_item(name, item);
    }
}

std::shared_ptr<Namespace> ImportManager::get_or_create_namespace(const std::string& name) {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        return it->second;
    }
    
    auto ns = std::make_shared<Namespace>(name);
    namespaces_[name] = ns;
    return ns;
}

void ImportManager::merge_namespaces(const std::string& name, std::shared_ptr<Namespace> ns) {
    auto existing = get_or_create_namespace(name);
    existing->merge_with(ns);
}

void ImportManager::resolve_dependencies(const std::string& file_path) {
    // 依赖解析逻辑
    // 这里可以实现更复杂的依赖解析
}

void ImportManager::add_import(const std::string& name, const std::string& path, ImportType type, const std::string& alias) {
    std::string resolved_path = resolve_import_path(path, "");
    auto item = std::make_shared<ImportItem>(name, type, resolved_path);
    if (!alias.empty()) {
        item->alias = alias;
    }
    global_namespace_->add_item(name, item);
}

std::shared_ptr<ImportItem> ImportManager::get_import(const std::string& name, const std::string& namespace_name) const {
    if (namespace_name.empty()) {
        return global_namespace_->get_item(name);
    }
    
    auto it = namespaces_.find(namespace_name);
    if (it != namespaces_.end()) {
        return it->second->get_item(name);
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<ImportItem>> ImportManager::get_imports_by_type(ImportType type) const {
    std::vector<std::shared_ptr<ImportItem>> result;
    
    for (const auto& pair : global_namespace_->get_items()) {
        if (pair.second->type == type) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::shared_ptr<Namespace> ImportManager::get_namespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    return (it != namespaces_.end()) ? it->second : nullptr;
}

void ImportManager::set_default_namespace(const std::string& name) {
    // 设置默认命名空间
}

std::string ImportManager::get_default_namespace() const {
    return "global";
}

void ImportManager::add_search_path(const std::string& path) {
    path_resolver_->add_search_path(path);
}

std::string ImportManager::resolve_import_path(const std::string& import_path, const std::string& current_file) const {
    if (path_resolver_->is_absolute_path(import_path)) {
        return path_resolver_->normalize_path(import_path);
    }
    
    // 相对路径解析
    std::string current_dir = path_resolver_->get_directory(current_file);
    std::string full_path = current_dir + "/" + import_path;
    return path_resolver_->normalize_path(full_path);
}

std::vector<std::string> ImportManager::get_dependencies(const std::string& file_path) const {
    // 返回依赖列表
    return {};
}

bool ImportManager::has_circular_dependency(const std::string& file_path) const {
    // 检查循环依赖
    return false;
}

void ImportManager::print_imports() const {
    std::cout << "=== Imports ===" << std::endl;
    for (const auto& pair : global_namespace_->get_items()) {
        std::cout << "Import: " << pair.first << " -> " << pair.second->source_path << std::endl;
    }
}

void ImportManager::print_namespaces() const {
    std::cout << "=== Namespaces ===" << std::endl;
    for (const auto& pair : namespaces_) {
        pair.second->print_structure();
    }
}

void ImportManager::print_dependencies() const {
    std::cout << "=== Dependencies ===" << std::endl;
    for (const auto& file : processed_files_) {
        std::cout << "Processed: " << file << std::endl;
    }
}

void ImportManager::clear() {
    namespaces_.clear();
    processed_files_.clear();
    global_namespace_ = std::make_shared<Namespace>("global");
    namespaces_["global"] = global_namespace_;
}

void ImportManager::reset() {
    clear();
}

} // namespace import
} // namespace chtl

#include "parser/chtl_parser.h"
#include "lexer/chtl_lexer.h"

std::shared_ptr<chtl::ast::ASTNode> chtl::import::ImportManager::load_module(const std::string& module_name) {
    if (imported_asts_.count(module_name)) {
        return imported_asts_[module_name];
    }

    std::string packed_content;
    bool loaded_from_vfs = false;

    // 1. Try to load from the real filesystem first
    std::string module_path = path_resolver_->resolve_path(module_name + ".cmod");
    try {
        packed_content = read_file(module_path);
        std::cout << "Loading module '" << module_name << "' from filesystem: " << module_path << std::endl;
    } catch (const std::exception& e) {
        // 2. Fallback to virtual filesystem if real file not found
        std::cout << "Could not find module '" << module_name << "' on disk. Falling back to virtual filesystem." << std::endl;
        module_path = "modules/" + module_name + ".cmod"; // VFS uses a simpler path
        if (virtual_filesystem_.count(module_path)) {
            packed_content = virtual_filesystem_[module_path];
            loaded_from_vfs = true;
        } else {
            std::cerr << "Error: Module '" << module_name << "' not found in virtual filesystem either." << std::endl;
            return nullptr;
        }
    }

    // 3. Unpack, parse info, and parse source from the loaded content
    auto unpacked_files = cmod_cjmod::ModulePackager::unpack(packed_content);

    std::string info_path = "info/" + module_name + ".chtl";
    if (unpacked_files.count(info_path) == 0) {
        std::cerr << "Warning: Module info file not found for " << module_name << std::endl;
        return nullptr;
    }
    cmod_cjmod::ModuleInfoParser info_parser(unpacked_files[info_path]);
    auto parsed_info = info_parser.parse();
    std::cout << "Successfully parsed info for module: " << parsed_info.name << std::endl;

    std::string source_path = "src/" + module_name + ".chtl";
    if (unpacked_files.count(source_path) == 0) {
        std::cerr << "Warning: Module source file not found for " << module_name << std::endl;
        return nullptr;
    }

    try {
        lexer::CHTLLexer lexer(unpacked_files[source_path]);
        auto tokens = lexer.tokenize();
        parser::CHTLParser parser(tokens);
        auto module_ast = parser.parse();

        if (module_ast) {
            imported_asts_[module_name] = module_ast;
            return module_ast;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing module source for " << module_name << ": " << e.what() << std::endl;
    }

    return nullptr;
}
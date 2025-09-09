#include "CHTL/CHTLModule.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// ModuleInfo 实现
ModuleInfo::ModuleInfo(const std::string& name, ModuleType type, const std::string& version)
    : m_name(name), m_type(type), m_version(version) {
}

void ModuleInfo::addDependency(const std::string& moduleName, const std::string& version) {
    m_dependencies.push_back({moduleName, version});
}

std::vector<std::pair<std::string, std::string>> ModuleInfo::getDependencies() const {
    return m_dependencies;
}

bool ModuleInfo::hasDependency(const std::string& moduleName) const {
    for (const auto& [name, version] : m_dependencies) {
        if (name == moduleName) {
            return true;
        }
    }
    return false;
}

void ModuleInfo::setMetadata(const std::string& key, const std::string& value) {
    m_metadata[key] = value;
}

std::string ModuleInfo::getMetadata(const std::string& key) const {
    auto it = m_metadata.find(key);
    return (it != m_metadata.end()) ? it->second : "";
}

bool ModuleInfo::hasMetadata(const std::string& key) const {
    return m_metadata.find(key) != m_metadata.end();
}

// CMODModule 实现
CMODModule::CMODModule(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void CMODModule::createModule(const std::string& name, const std::string& src, const std::string& info) {
    auto moduleInfo = parseModuleInfo(info);
    if (moduleInfo) {
        // 创建模块实例
        auto module = std::make_shared<CMODModule>(m_context);
        addModule(name, module);
    }
}

void CMODModule::createModuleFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot open CMOD file: " + filePath);
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // 解析模块内容
    auto module = parseCMODModule(content);
    if (module) {
        std::string moduleName = std::filesystem::path(filePath).stem().string();
        addModule(moduleName, module);
    }
}

std::shared_ptr<ModuleInfo> CMODModule::parseModuleInfo(const std::string& info) const {
    auto moduleInfo = std::make_shared<ModuleInfo>("", ModuleType::CMOD);
    
    // 解析 [Info] 块
    std::string infoBlock = parseInfoBlock(info);
    if (!infoBlock.empty()) {
        // 解析模块名称
        std::regex nameRegex(R"(name\s*:\s*([^\n]+))");
        std::smatch match;
        if (std::regex_search(infoBlock, match, nameRegex)) {
            std::string name = match[1].str();
            name.erase(0, name.find_first_not_of(" \t"));
            name.erase(name.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("name", name);
        }
        
        // 解析版本
        std::regex versionRegex(R"(version\s*:\s*([^\n]+))");
        if (std::regex_search(infoBlock, match, versionRegex)) {
            std::string version = match[1].str();
            version.erase(0, version.find_first_not_of(" \t"));
            version.erase(version.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("version", version);
        }
        
        // 解析描述
        std::regex descRegex(R"(description\s*:\s*([^\n]+))");
        if (std::regex_search(infoBlock, match, descRegex)) {
            std::string desc = match[1].str();
            desc.erase(0, desc.find_first_not_of(" \t"));
            desc.erase(desc.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("description", desc);
        }
        
        // 解析作者
        std::regex authorRegex(R"(author\s*:\s*([^\n]+))");
        if (std::regex_search(infoBlock, match, authorRegex)) {
            std::string author = match[1].str();
            author.erase(0, author.find_first_not_of(" \t"));
            author.erase(author.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("author", author);
        }
        
        // 解析许可证
        std::regex licenseRegex(R"(license\s*:\s*([^\n]+))");
        if (std::regex_search(infoBlock, match, licenseRegex)) {
            std::string license = match[1].str();
            license.erase(0, license.find_first_not_of(" \t"));
            license.erase(license.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("license", license);
        }
        
        // 解析依赖
        std::vector<std::string> deps = parseDependencies(infoBlock);
        for (const auto& dep : deps) {
            moduleInfo->addDependency(dep);
        }
    }
    
    return moduleInfo;
}

std::string CMODModule::parseModuleSource(const std::string& src) const {
    // 解析源文件内容
    std::string sourceBlock = parseSourceBlock(src);
    return sourceBlock;
}

std::string CMODModule::exportModule(const std::string& name) const {
    auto module = getModule(name);
    if (!module) {
        m_context->addError("Module not found: " + name);
        return "";
    }
    
    // 生成模块导出内容
    std::stringstream ss;
    ss << "// CMOD Module: " << name << "\n";
    ss << "[Info]\n";
    ss << "{\n";
    ss << "  name: " << name << ";\n";
    ss << "  version: 1.0.0;\n";
    ss << "}\n\n";
    ss << "[Export]\n";
    ss << "{\n";
    ss << "  // 导出内容\n";
    ss << "}\n";
    
    return ss.str();
}

std::string CMODModule::exportModuleToFile(const std::string& name, const std::string& filePath) const {
    std::string content = exportModule(name);
    if (content.empty()) {
        return "";
    }
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot create CMOD file: " + filePath);
        return "";
    }
    
    file << content;
    file.close();
    
    return content;
}

void CMODModule::addModule(const std::string& name, std::shared_ptr<CMODModule> module) {
    m_modules[name] = module;
}

std::shared_ptr<CMODModule> CMODModule::getModule(const std::string& name) const {
    auto it = m_modules.find(name);
    return (it != m_modules.end()) ? it->second : nullptr;
}

bool CMODModule::hasModule(const std::string& name) const {
    return m_modules.find(name) != m_modules.end();
}

bool CMODModule::validateModule(const std::string& name) const {
    auto module = getModule(name);
    if (!module) {
        return false;
    }
    
    // 验证模块内容
    return true;
}

bool CMODModule::validateModuleInfo(const std::string& info) const {
    // 验证模块信息格式
    std::regex infoRegex(R"(\[Info\]\s*\{[^}]*\})");
    return std::regex_search(info, infoRegex);
}

std::string CMODModule::parseInfoBlock(const std::string& info) const {
    std::regex infoRegex(R"(\[Info\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(info, match, infoRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string CMODModule::parseExportBlock(const std::string& info) const {
    std::regex exportRegex(R"(\[Export\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(info, match, exportRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::vector<std::string> CMODModule::parseDependencies(const std::string& info) const {
    std::vector<std::string> deps;
    
    std::regex depsRegex(R"(dependencies\s*:\s*\[([^\]]*)\])");
    std::smatch match;
    
    if (std::regex_search(info, match, depsRegex)) {
        std::string depsStr = match[1].str();
        std::stringstream ss(depsStr);
        std::string dep;
        
        while (std::getline(ss, dep, ',')) {
            dep.erase(0, dep.find_first_not_of(" \t"));
            dep.erase(dep.find_last_not_of(" \t") + 1);
            if (!dep.empty()) {
                deps.push_back(dep);
            }
        }
    }
    
    return deps;
}

// CJMODModule 实现
CJMODModule::CJMODModule(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void CJMODModule::createModule(const std::string& name, const std::string& src, const std::string& info) {
    auto moduleInfo = parseModuleInfo(info);
    if (moduleInfo) {
        // 创建模块实例
        auto module = std::make_shared<CJMODModule>(m_context);
        addModule(name, module);
    }
}

void CJMODModule::createModuleFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot open CJMOD file: " + filePath);
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // 解析模块内容
    auto module = parseCJMODModule(content);
    if (module) {
        std::string moduleName = std::filesystem::path(filePath).stem().string();
        addModule(moduleName, module);
    }
}

std::shared_ptr<ModuleInfo> CJMODModule::parseModuleInfo(const std::string& info) const {
    auto moduleInfo = std::make_shared<ModuleInfo>("", ModuleType::CJMOD);
    
    // 解析 [Info] 块
    std::string infoBlock = parseInfoBlock(info);
    if (!infoBlock.empty()) {
        // 解析模块名称
        std::regex nameRegex(R"(name\s*:\s*([^\n]+))");
        std::smatch match;
        if (std::regex_search(infoBlock, match, nameRegex)) {
            std::string name = match[1].str();
            name.erase(0, name.find_first_not_of(" \t"));
            name.erase(name.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("name", name);
        }
        
        // 解析版本
        std::regex versionRegex(R"(version\s*:\s*([^\n]+))");
        if (std::regex_search(infoBlock, match, versionRegex)) {
            std::string version = match[1].str();
            version.erase(0, version.find_first_not_of(" \t"));
            version.erase(version.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("version", version);
        }
        
        // 解析描述
        std::regex descRegex(R"(description\s*:\s*([^\n]+))");
        if (std::regex_search(infoBlock, match, descRegex)) {
            std::string desc = match[1].str();
            desc.erase(0, desc.find_first_not_of(" \t"));
            desc.erase(desc.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("description", desc);
        }
        
        // 解析作者
        std::regex authorRegex(R"(author\s*:\s*([^\n]+))");
        if (std::regex_search(infoBlock, match, authorRegex)) {
            std::string author = match[1].str();
            author.erase(0, author.find_first_not_of(" \t"));
            author.erase(author.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("author", author);
        }
        
        // 解析许可证
        std::regex licenseRegex(R"(license\s*:\s*([^\n]+))");
        if (std::regex_search(infoBlock, match, licenseRegex)) {
            std::string license = match[1].str();
            license.erase(0, license.find_first_not_of(" \t"));
            license.erase(license.find_last_not_of(" \t") + 1);
            moduleInfo->setMetadata("license", license);
        }
        
        // 解析依赖
        std::vector<std::string> deps = parseDependencies(infoBlock);
        for (const auto& dep : deps) {
            moduleInfo->addDependency(dep);
        }
    }
    
    return moduleInfo;
}

std::string CJMODModule::parseModuleSource(const std::string& src) const {
    // 解析源文件内容
    std::string sourceBlock = parseSourceBlock(src);
    return sourceBlock;
}

std::string CJMODModule::exportModule(const std::string& name) const {
    auto module = getModule(name);
    if (!module) {
        m_context->addError("Module not found: " + name);
        return "";
    }
    
    // 生成模块导出内容
    std::stringstream ss;
    ss << "// CJMOD Module: " << name << "\n";
    ss << "[Info]\n";
    ss << "{\n";
    ss << "  name: " << name << ";\n";
    ss << "  version: 1.0.0;\n";
    ss << "}\n\n";
    ss << "// CJMOD 源文件内容\n";
    
    return ss.str();
}

std::string CJMODModule::exportModuleToFile(const std::string& name, const std::string& filePath) const {
    std::string content = exportModule(name);
    if (content.empty()) {
        return "";
    }
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot create CJMOD file: " + filePath);
        return "";
    }
    
    file << content;
    file.close();
    
    return content;
}

void CJMODModule::addModule(const std::string& name, std::shared_ptr<CJMODModule> module) {
    m_modules[name] = module;
}

std::shared_ptr<CJMODModule> CJMODModule::getModule(const std::string& name) const {
    auto it = m_modules.find(name);
    return (it != m_modules.end()) ? it->second : nullptr;
}

bool CJMODModule::hasModule(const std::string& name) const {
    return m_modules.find(name) != m_modules.end();
}

bool CJMODModule::validateModule(const std::string& name) const {
    auto module = getModule(name);
    if (!module) {
        return false;
    }
    
    // 验证模块内容
    return true;
}

bool CJMODModule::validateModuleInfo(const std::string& info) const {
    // 验证模块信息格式
    std::regex infoRegex(R"(\[Info\]\s*\{[^}]*\})");
    return std::regex_search(info, infoRegex);
}

std::string CJMODModule::parseInfoBlock(const std::string& info) const {
    std::regex infoRegex(R"(\[Info\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(info, match, infoRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::vector<std::string> CJMODModule::parseDependencies(const std::string& info) const {
    std::vector<std::string> deps;
    
    std::regex depsRegex(R"(dependencies\s*:\s*\[([^\]]*)\])");
    std::smatch match;
    
    if (std::regex_search(info, match, depsRegex)) {
        std::string depsStr = match[1].str();
        std::stringstream ss(depsStr);
        std::string dep;
        
        while (std::getline(ss, dep, ',')) {
            dep.erase(0, dep.find_first_not_of(" \t"));
            dep.erase(dep.find_last_not_of(" \t") + 1);
            if (!dep.empty()) {
                deps.push_back(dep);
            }
        }
    }
    
    return deps;
}

} // namespace CHTL
#include "CHTL/CHTLVSCodeExtension.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <json/json.h>

namespace CHTL {

// ModuleUnpacker 实现
ModuleUnpacker::ModuleUnpacker(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void ModuleUnpacker::unpackModule(const std::string& modulePath, const std::string& outputPath) {
    if (!isModuleFile(modulePath)) {
        m_context->addError("Not a valid module file: " + modulePath);
        return;
    }
    
    try {
        std::filesystem::create_directories(outputPath);
        
        // 读取模块文件
        std::ifstream file(modulePath);
        if (!file.is_open()) {
            m_context->addError("Cannot open module file: " + modulePath);
            return;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string moduleContent = buffer.str();
        
        // 解析模块信息
        std::string moduleInfo = getModuleInfo(modulePath);
        
        // 创建模块目录结构
        std::string moduleName = extractModuleName(modulePath);
        std::string moduleDir = outputPath + "/" + moduleName;
        std::filesystem::create_directories(moduleDir);
        
        // 保存模块内容
        std::string moduleFile = moduleDir + "/" + moduleName + ".chtl";
        std::ofstream outputFile(moduleFile);
        if (outputFile.is_open()) {
            outputFile << moduleContent;
            outputFile.close();
        }
        
        // 保存模块信息
        std::string infoFile = moduleDir + "/module.json";
        std::ofstream infoOutput(infoFile);
        if (infoOutput.is_open()) {
            infoOutput << moduleInfo;
            infoOutput.close();
        }
        
        // 解包依赖
        unpackModuleDependencies(modulePath);
        
    } catch (const std::exception& e) {
        m_context->addError("Error unpacking module: " + std::string(e.what()));
    }
}

void ModuleUnpacker::unpackAllModules(const std::string& projectPath) {
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath)) {
            if (entry.is_regular_file() && isModuleFile(entry.path().string())) {
                std::string modulePath = entry.path().string();
                std::string outputPath = projectPath + "/unpacked";
                unpackModule(modulePath, outputPath);
            }
        }
    } catch (const std::exception& e) {
        m_context->addError("Error unpacking all modules: " + std::string(e.what()));
    }
}

void ModuleUnpacker::unpackModuleDependencies(const std::string& modulePath) {
    std::vector<std::string> dependencies = parseModuleDependencies(modulePath);
    
    for (const auto& dependency : dependencies) {
        // 查找依赖模块
        std::string dependencyPath = findModulePath(dependency);
        if (!dependencyPath.empty()) {
            std::string outputPath = std::filesystem::path(modulePath).parent_path().string() + "/dependencies";
            unpackModule(dependencyPath, outputPath);
        }
    }
}

std::vector<std::string> ModuleUnpacker::findModules(const std::string& projectPath) {
    std::vector<std::string> modules;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath)) {
            if (entry.is_regular_file() && isModuleFile(entry.path().string())) {
                modules.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        m_context->addError("Error finding modules: " + std::string(e.what()));
    }
    
    return modules;
}

std::vector<std::string> ModuleUnpacker::findModuleDependencies(const std::string& modulePath) {
    return parseModuleDependencies(modulePath);
}

std::string ModuleUnpacker::getModuleInfo(const std::string& modulePath) {
    std::ifstream file(modulePath);
    if (!file.is_open()) {
        return "{}";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // 解析模块信息
    Json::Value moduleInfo;
    moduleInfo["name"] = extractModuleName(modulePath);
    moduleInfo["version"] = getModuleVersion(modulePath);
    moduleInfo["path"] = modulePath;
    moduleInfo["dependencies"] = Json::Value(Json::arrayValue);
    
    // 查找依赖
    std::vector<std::string> dependencies = parseModuleDependencies(modulePath);
    for (const auto& dep : dependencies) {
        moduleInfo["dependencies"].append(dep);
    }
    
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, moduleInfo);
}

void ModuleUnpacker::installModule(const std::string& modulePath) {
    std::string moduleName = extractModuleName(modulePath);
    std::string installPath = "modules/" + moduleName;
    
    unpackModule(modulePath, installPath);
}

void ModuleUnpacker::uninstallModule(const std::string& moduleName) {
    std::string modulePath = "modules/" + moduleName;
    
    try {
        if (std::filesystem::exists(modulePath)) {
            std::filesystem::remove_all(modulePath);
        }
    } catch (const std::exception& e) {
        m_context->addError("Error uninstalling module: " + std::string(e.what()));
    }
}

void ModuleUnpacker::updateModule(const std::string& moduleName) {
    // 简化实现，实际应该检查更新并重新安装
    std::string modulePath = "modules/" + moduleName + "/" + moduleName + ".chtl";
    
    if (std::filesystem::exists(modulePath)) {
        installModule(modulePath);
    }
}

bool ModuleUnpacker::isModuleFile(const std::string& filePath) const {
    std::filesystem::path path(filePath);
    std::string extension = path.extension().string();
    return extension == ".cmod" || extension == ".cjmod" || extension == ".chtl";
}

std::string ModuleUnpacker::extractModuleName(const std::string& modulePath) const {
    std::filesystem::path path(modulePath);
    return path.stem().string();
}

std::string ModuleUnpacker::getModuleVersion(const std::string& modulePath) const {
    // 简化实现，实际应该解析模块文件中的版本信息
    return "1.0.0";
}

std::vector<std::string> ModuleUnpacker::parseModuleDependencies(const std::string& modulePath) const {
    std::vector<std::string> dependencies;
    
    std::ifstream file(modulePath);
    if (!file.is_open()) {
        return dependencies;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // 查找依赖声明
    std::regex depsRegex(R"(dependencies\s*:\s*\[([^\]]*)\])");
    std::smatch match;
    
    if (std::regex_search(content, match, depsRegex)) {
        std::string depsStr = match[1].str();
        std::stringstream ss(depsStr);
        std::string dep;
        
        while (std::getline(ss, dep, ',')) {
            dep.erase(0, dep.find_first_not_of(" \t"));
            dep.erase(dep.find_last_not_of(" \t") + 1);
            if (!dep.empty()) {
                dependencies.push_back(dep);
            }
        }
    }
    
    return dependencies;
}

std::string ModuleUnpacker::findModulePath(const std::string& moduleName) const {
    // 简化实现，实际应该在模块搜索路径中查找
    std::vector<std::string> searchPaths = {
        "modules/",
        "node_modules/",
        "lib/",
        "src/"
    };
    
    for (const auto& path : searchPaths) {
        std::string modulePath = path + moduleName + ".cmod";
        if (std::filesystem::exists(modulePath)) {
            return modulePath;
        }
        
        modulePath = path + moduleName + ".cjmod";
        if (std::filesystem::exists(modulePath)) {
            return modulePath;
        }
    }
    
    return "";
}

// JSONQueryTable 实现
JSONQueryTable::JSONQueryTable(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string JSONQueryTable::query(const std::string& queryString) {
    try {
        Json::Value query = parseQuery(queryString);
        Json::Value result = executeQuery(query);
        
        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, result);
    } catch (const std::exception& e) {
        m_context->addError("Query error: " + std::string(e.what()));
        return "{}";
    }
}

std::string JSONQueryTable::queryFile(const std::string& filePath, const std::string& queryString) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot open file: " + filePath);
        return "{}";
    }
    
    Json::Value root;
    file >> root;
    
    // 将文件数据添加到查询表
    addData("file", root);
    
    return query(queryString);
}

std::vector<std::string> JSONQueryTable::queryMultiple(const std::string& queryString) {
    std::vector<std::string> results;
    
    for (const auto& [key, value] : m_data) {
        addData("current", value);
        std::string result = query(queryString);
        results.push_back(result);
    }
    
    return results;
}

void JSONQueryTable::addData(const std::string& key, const Json::Value& value) {
    m_data[key] = value;
}

void JSONQueryTable::addDataFromFile(const std::string& key, const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot open file: " + filePath);
        return;
    }
    
    Json::Value root;
    file >> root;
    
    addData(key, root);
}

void JSONQueryTable::removeData(const std::string& key) {
    m_data.erase(key);
}

void JSONQueryTable::clearData() {
    m_data.clear();
}

void JSONQueryTable::createIndex(const std::string& field) {
    // 简化实现，实际应该创建更复杂的索引
    for (const auto& [key, value] : m_data) {
        if (value.isObject() && value.isMember(field)) {
            std::string fieldValue = value[field].asString();
            m_indexes[field][fieldValue].push_back(key);
        }
    }
}

void JSONQueryTable::removeIndex(const std::string& field) {
    m_indexes.erase(field);
}

void JSONQueryTable::optimizeQueries() {
    // 简化实现，实际应该优化查询性能
}

Json::Value JSONQueryTable::parseQuery(const std::string& queryString) const {
    Json::Value query;
    
    // 简化实现，实际应该解析更复杂的查询语法
    if (queryString == "all") {
        query["type"] = "select";
        query["fields"] = "*";
    } else if (queryString.starts_with("find ")) {
        query["type"] = "find";
        query["field"] = queryString.substr(5);
    } else {
        query["type"] = "select";
        query["fields"] = queryString;
    }
    
    return query;
}

Json::Value JSONQueryTable::executeQuery(const Json::Value& query) const {
    Json::Value result(Json::arrayValue);
    
    std::string queryType = query.get("type", "select").asString();
    
    if (queryType == "select" || queryType == "all") {
        for (const auto& [key, value] : m_data) {
            result.append(value);
        }
    } else if (queryType == "find") {
        std::string field = query.get("field", "").asString();
        std::string value = query.get("value", "").asString();
        
        for (const auto& [key, data] : m_data) {
            if (data.isObject() && data.isMember(field) && data[field].asString() == value) {
                result.append(data);
            }
        }
    }
    
    return result;
}

std::vector<std::string> JSONQueryTable::searchIndex(const std::string& field, const std::string& value) const {
    auto fieldIt = m_indexes.find(field);
    if (fieldIt == m_indexes.end()) {
        return {};
    }
    
    auto valueIt = fieldIt->second.find(value);
    if (valueIt == fieldIt->second.end()) {
        return {};
    }
    
    return valueIt->second;
}

// ModuleSyntaxHelper 实现
ModuleSyntaxHelper::ModuleSyntaxHelper(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    initializeSyntaxHints();
}

std::vector<std::string> ModuleSyntaxHelper::getModuleSyntaxHints(const std::string& moduleName) {
    std::vector<std::string> hints;
    
    // 基础模块语法提示
    hints.push_back("[Info] { name: \"" + moduleName + "\"; version: \"1.0.0\"; }");
    hints.push_back("[Export] { /* 导出内容 */ }");
    hints.push_back("dependencies: [\"module1\", \"module2\"]");
    hints.push_back("author: \"Your Name\"");
    hints.push_back("license: \"MIT\"");
    
    return hints;
}

std::vector<std::string> ModuleSyntaxHelper::getTemplateSyntaxHints(const std::string& templateName) {
    std::vector<std::string> hints;
    
    // 模板语法提示
    hints.push_back("[Template] @Style " + templateName + " { /* 样式定义 */ }");
    hints.push_back("[Template] @Element " + templateName + " { /* 元素定义 */ }");
    hints.push_back("[Template] @Var " + templateName + " { /* 变量定义 */ }");
    hints.push_back("inherit " + templateName + ";");
    hints.push_back("compositional " + templateName + ";");
    
    return hints;
}

std::vector<std::string> ModuleSyntaxHelper::getCustomSyntaxHints(const std::string& customName) {
    std::vector<std::string> hints;
    
    // 自定义语法提示
    hints.push_back("[Custom] @Style " + customName + " { /* 自定义样式 */ }");
    hints.push_back("[Custom] @Element " + customName + " { /* 自定义元素 */ }");
    hints.push_back("[Custom] @Var " + customName + " { /* 自定义变量 */ }");
    hints.push_back("delete property;");
    hints.push_back("insert property: value;");
    hints.push_back("modify property: value;");
    
    return hints;
}

std::vector<std::string> ModuleSyntaxHelper::getImportSyntaxHints(const std::string& importPath) {
    std::vector<std::string> hints;
    
    // 导入语法提示
    hints.push_back("[Import] \"" + importPath + "\";");
    hints.push_back("[Import] \"" + importPath + "\" as alias;");
    hints.push_back("[Import] \"" + importPath + "\"::Component;");
    hints.push_back("[Import] \"" + importPath + "\"::* as alias;");
    
    return hints;
}

bool ModuleSyntaxHelper::validateModuleSyntax(const std::string& moduleCode) {
    // 简化实现，实际应该进行完整的语法验证
    return moduleCode.find("[Info]") != std::string::npos;
}

bool ModuleSyntaxHelper::validateTemplateSyntax(const std::string& templateCode) {
    return templateCode.find("[Template]") != std::string::npos;
}

bool ModuleSyntaxHelper::validateCustomSyntax(const std::string& customCode) {
    return customCode.find("[Custom]") != std::string::npos;
}

bool ModuleSyntaxHelper::validateImportSyntax(const std::string& importCode) {
    return importCode.find("[Import]") != std::string::npos;
}

std::string ModuleSyntaxHelper::getSyntaxHelp(const std::string& syntaxType) {
    if (syntaxType == "module") {
        return "Module syntax: [Info] { name: \"moduleName\"; version: \"1.0.0\"; }";
    } else if (syntaxType == "template") {
        return "Template syntax: [Template] @Style TemplateName { /* styles */ }";
    } else if (syntaxType == "custom") {
        return "Custom syntax: [Custom] @Style CustomName { /* custom styles */ }";
    } else if (syntaxType == "import") {
        return "Import syntax: [Import] \"file.chtl\";";
    }
    
    return "Unknown syntax type: " + syntaxType;
}

std::string ModuleSyntaxHelper::getModuleHelp(const std::string& moduleName) {
    return "Help for module: " + moduleName;
}

std::string ModuleSyntaxHelper::getTemplateHelp(const std::string& templateName) {
    return "Help for template: " + templateName;
}

std::string ModuleSyntaxHelper::getCustomHelp(const std::string& customName) {
    return "Help for custom: " + customName;
}

void ModuleSyntaxHelper::initializeSyntaxHints() {
    // 初始化语法提示
}

std::vector<std::string> ModuleSyntaxHelper::parseModuleSyntax(const std::string& moduleCode) const {
    std::vector<std::string> hints;
    
    // 解析模块语法
    if (moduleCode.find("[Info]") != std::string::npos) {
        hints.push_back("Info block found");
    }
    
    if (moduleCode.find("[Export]") != std::string::npos) {
        hints.push_back("Export block found");
    }
    
    return hints;
}

std::vector<std::string> ModuleSyntaxHelper::parseTemplateSyntax(const std::string& templateCode) const {
    std::vector<std::string> hints;
    
    // 解析模板语法
    if (templateCode.find("@Style") != std::string::npos) {
        hints.push_back("Style template found");
    }
    
    if (templateCode.find("@Element") != std::string::npos) {
        hints.push_back("Element template found");
    }
    
    return hints;
}

std::vector<std::string> ModuleSyntaxHelper::parseCustomSyntax(const std::string& customCode) const {
    std::vector<std::string> hints;
    
    // 解析自定义语法
    if (customCode.find("delete") != std::string::npos) {
        hints.push_back("Delete operation found");
    }
    
    if (customCode.find("insert") != std::string::npos) {
        hints.push_back("Insert operation found");
    }
    
    return hints;
}

std::vector<std::string> ModuleSyntaxHelper::parseImportSyntax(const std::string& importCode) const {
    std::vector<std::string> hints;
    
    // 解析导入语法
    if (importCode.find("as") != std::string::npos) {
        hints.push_back("Alias import found");
    }
    
    if (importCode.find("::") != std::string::npos) {
        hints.push_back("Specific import found");
    }
    
    return hints;
}

} // namespace CHTL
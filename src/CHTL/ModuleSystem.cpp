#include "CHTL/ModuleSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace CHTL {

// CMODModule 实现
CMODModule::CMODModule(const std::string& name) : info_(name) {
}

CMODModule::~CMODModule() = default;

ModuleInfo& CMODModule::getInfo() {
    return info_;
}

const ModuleInfo& CMODModule::getInfo() const {
    return info_;
}

void CMODModule::setInfo(const ModuleInfo& info) {
    info_ = info;
}

void CMODModule::addExport(const ModuleExport& exportItem) {
    exports_.push_back(exportItem);
}

void CMODModule::removeExport(const std::string& name) {
    exports_.erase(
        std::remove_if(exports_.begin(), exports_.end(),
            [&name](const ModuleExport& exp) { return exp.name == name; }),
        exports_.end()
    );
}

std::vector<ModuleExport> CMODModule::getExports() const {
    return exports_;
}

std::shared_ptr<CHTLNode> CMODModule::getExport(const std::string& name) const {
    auto it = std::find_if(exports_.begin(), exports_.end(),
        [&name](const ModuleExport& exp) { return exp.name == name; });
    
    if (it != exports_.end()) {
        return it->node;
    }
    return nullptr;
}

bool CMODModule::hasExport(const std::string& name) const {
    return std::any_of(exports_.begin(), exports_.end(),
        [&name](const ModuleExport& exp) { return exp.name == name; });
}

void CMODModule::addTemplateExport(std::shared_ptr<TemplateNode> templateNode) {
    if (templateNode) {
        template_exports_.push_back(templateNode);
        exports_.emplace_back(templateNode->getTemplateName(), "template", templateNode);
    }
}

std::vector<std::shared_ptr<TemplateNode>> CMODModule::getTemplateExports() const {
    return template_exports_;
}

void CMODModule::addCustomExport(std::shared_ptr<CustomNode> customNode) {
    if (customNode) {
        custom_exports_.push_back(customNode);
        exports_.emplace_back(customNode->getCustomName(), "custom", customNode);
    }
}

std::vector<std::shared_ptr<CustomNode>> CMODModule::getCustomExports() const {
    return custom_exports_;
}

void CMODModule::addOriginExport(const std::string& name, const std::string& content, const std::string& type) {
    origin_exports_[type].emplace_back(name, content);
    exports_.emplace_back(name, "origin", nullptr);
}

std::vector<std::pair<std::string, std::string>> CMODModule::getOriginExports(const std::string& type) const {
    auto it = origin_exports_.find(type);
    if (it != origin_exports_.end()) {
        return it->second;
    }
    return {};
}

void CMODModule::addConfigurationExport(const std::string& name, const std::string& content) {
    configuration_exports_.emplace_back(name, content);
    exports_.emplace_back(name, "configuration", nullptr);
}

std::vector<std::pair<std::string, std::string>> CMODModule::getConfigurationExports() const {
    return configuration_exports_;
}

bool CMODModule::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    
    // 解析各个块
    if (!parseInfoBlock(content)) {
        return false;
    }
    
    if (!parseExportBlock(content)) {
        return false;
    }
    
    if (!parseTemplateBlock(content)) {
        return false;
    }
    
    if (!parseCustomBlock(content)) {
        return false;
    }
    
    if (!parseOriginBlock(content)) {
        return false;
    }
    
    if (!parseConfigurationBlock(content)) {
        return false;
    }
    
    return true;
}

bool CMODModule::saveToFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateInfoBlock() << std::endl;
    file << generateExportBlock() << std::endl;
    file << generateTemplateBlock() << std::endl;
    file << generateCustomBlock() << std::endl;
    file << generateOriginBlock() << std::endl;
    file << generateConfigurationBlock() << std::endl;
    
    return true;
}

bool CMODModule::isValid() const {
    return !info_.name.empty() && validate();
}

bool CMODModule::validate() const {
    // 验证模块信息
    if (info_.name.empty()) {
        return false;
    }
    
    // 验证导出项
    for (const auto& exportItem : exports_) {
        if (exportItem.name.empty() || exportItem.type.empty()) {
            return false;
        }
    }
    
    return true;
}

void CMODModule::clear() {
    info_ = ModuleInfo();
    exports_.clear();
    template_exports_.clear();
    custom_exports_.clear();
    origin_exports_.clear();
    configuration_exports_.clear();
}

bool CMODModule::parseInfoBlock(const std::string& content) {
    std::regex infoRegex(R"(\[Info\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, infoRegex)) {
        std::string infoContent = match[1].str();
        
        // 解析名称
        std::regex nameRegex("name\\s*:\\s*\"([^\"]+)\"");
        std::smatch nameMatch;
        if (std::regex_search(infoContent, nameMatch, nameRegex)) {
            info_.name = nameMatch[1].str();
        }
        
        // 解析版本
        std::regex versionRegex("version\\s*:\\s*\"([^\"]+)\"");
        std::smatch versionMatch;
        if (std::regex_search(infoContent, versionMatch, versionRegex)) {
            info_.version = versionMatch[1].str();
        }
        
        // 解析描述
        std::regex descRegex("description\\s*:\\s*\"([^\"]+)\"");
        std::smatch descMatch;
        if (std::regex_search(infoContent, descMatch, descRegex)) {
            info_.description = descMatch[1].str();
        }
        
        // 解析作者
        std::regex authorRegex("author\\s*:\\s*\"([^\"]+)\"");
        std::smatch authorMatch;
        if (std::regex_search(infoContent, authorMatch, authorRegex)) {
            info_.author = authorMatch[1].str();
        }
        
        // 解析许可证
        std::regex licenseRegex("license\\s*:\\s*\"([^\"]+)\"");
        std::smatch licenseMatch;
        if (std::regex_search(infoContent, licenseMatch, licenseRegex)) {
            info_.license = licenseMatch[1].str();
        }
        
        return true;
    }
    
    return false;
}

bool CMODModule::parseExportBlock(const std::string& content) {
    std::regex exportRegex(R"(\[Export\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, exportRegex)) {
        std::string exportContent = match[1].str();
        
        // 解析导出项
        std::regex itemRegex("(\\w+)\\s*:\\s*\"([^\"]+)\"");
        std::sregex_iterator iter(exportContent.begin(), exportContent.end(), itemRegex);
        std::sregex_iterator end;
        
        while (iter != end) {
            std::smatch itemMatch = *iter;
            std::string name = itemMatch[1].str();
            std::string type = itemMatch[2].str();
            
            exports_.emplace_back(name, type, nullptr);
            ++iter;
        }
        
        return true;
    }
    
    return false;
}

bool CMODModule::parseTemplateBlock(const std::string& content) {
    // 解析模板块
    std::regex templateRegex(R"(\[Template\]\s*@(\w+)\s+(\w+)\s*\{([^}]+)\})");
    std::sregex_iterator next(content.begin(), content.end(), templateRegex);
    std::sregex_iterator end;
    
    while (next != end) {
        std::smatch match = *next;
        std::string type = match[1].str();
        std::string name = match[2].str();
        std::string body = match[3].str();
        
        // 创建模板节点
        if (type == "Style") {
            auto templateNode = std::make_shared<StyleTemplateNode>(name);
            template_exports_.push_back(templateNode);
        } else if (type == "Element") {
            auto templateNode = std::make_shared<ElementTemplateNode>(name);
            template_exports_.push_back(templateNode);
        } else if (type == "Var") {
            auto templateNode = std::make_shared<VarTemplateNode>(name);
            template_exports_.push_back(templateNode);
        }
        
        next++;
    }
    
    return true;
}

bool CMODModule::parseCustomBlock(const std::string& content) {
    // 解析自定义块
    std::regex customRegex(R"(\[Custom\]\s*@(\w+)\s+(\w+)\s*\{([^}]+)\})");
    std::sregex_iterator next(content.begin(), content.end(), customRegex);
    std::sregex_iterator end;
    
    while (next != end) {
        std::smatch match = *next;
        std::string type = match[1].str();
        std::string name = match[2].str();
        std::string body = match[3].str();
        
        // 创建自定义节点
        if (type == "Style") {
            auto customNode = std::make_shared<CustomStyleNode>(name);
            custom_exports_.push_back(customNode);
        } else if (type == "Element") {
            auto customNode = std::make_shared<CustomElementNode>(name);
            custom_exports_.push_back(customNode);
        } else if (type == "Var") {
            auto customNode = std::make_shared<CustomVarNode>(name);
            custom_exports_.push_back(customNode);
        }
        
        next++;
    }
    
    return true;
}

bool CMODModule::parseOriginBlock(const std::string& content) {
    // 解析原始嵌入块
    std::regex originRegex(R"(\[Origin\]\s*@(\w+)\s+(\w+)\s*\{([^}]+)\})");
    std::sregex_iterator next(content.begin(), content.end(), originRegex);
    std::sregex_iterator end;
    
    while (next != end) {
        std::smatch match = *next;
        std::string type = match[1].str();
        std::string name = match[2].str();
        std::string body = match[3].str();
        
        origin_exports_[type].emplace_back(name, body);
        next++;
    }
    
    return true;
}

bool CMODModule::parseConfigurationBlock(const std::string& content) {
    // 解析配置块
    std::regex configRegex(R"(\[Configuration\]\s*(\w+)\s*\{([^}]+)\})");
    std::sregex_iterator next(content.begin(), content.end(), configRegex);
    std::sregex_iterator end;
    
    while (next != end) {
        std::smatch match = *next;
        std::string name = match[1].str();
        std::string body = match[2].str();
        
        configuration_exports_.emplace_back(name, body);
        next++;
    }
    
    return true;
}

std::string CMODModule::generateInfoBlock() const {
    std::ostringstream oss;
    oss << "[Info] {" << std::endl;
    oss << "    name: \"" << info_.name << "\";" << std::endl;
    oss << "    version: \"" << info_.version << "\";" << std::endl;
    if (!info_.description.empty()) {
        oss << "    description: \"" << info_.description << "\";" << std::endl;
    }
    if (!info_.author.empty()) {
        oss << "    author: \"" << info_.author << "\";" << std::endl;
    }
    if (!info_.license.empty()) {
        oss << "    license: \"" << info_.license << "\";" << std::endl;
    }
    oss << "}" << std::endl;
    return oss.str();
}

std::string CMODModule::generateExportBlock() const {
    std::ostringstream oss;
    oss << "[Export] {" << std::endl;
    for (const auto& exportItem : exports_) {
        oss << "    " << exportItem.name << ": \"" << exportItem.type << "\";" << std::endl;
    }
    oss << "}" << std::endl;
    return oss.str();
}

std::string CMODModule::generateTemplateBlock() const {
    std::ostringstream oss;
    for (const auto& templateNode : template_exports_) {
        oss << "[Template] @";
        if (templateNode->getTemplateType() == TemplateType::STYLE) {
            oss << "Style ";
        } else if (templateNode->getTemplateType() == TemplateType::ELEMENT) {
            oss << "Element ";
        } else if (templateNode->getTemplateType() == TemplateType::VAR) {
            oss << "Var ";
        }
        oss << templateNode->getTemplateName() << " {" << std::endl;
        oss << "    // Template content" << std::endl;
        oss << "}" << std::endl;
    }
    return oss.str();
}

std::string CMODModule::generateCustomBlock() const {
    std::ostringstream oss;
    for (const auto& customNode : custom_exports_) {
        oss << "[Custom] @";
        if (customNode->getCustomType() == CustomType::STYLE) {
            oss << "Style ";
        } else if (customNode->getCustomType() == CustomType::ELEMENT) {
            oss << "Element ";
        } else if (customNode->getCustomType() == CustomType::VAR) {
            oss << "Var ";
        }
        oss << customNode->getCustomName() << " {" << std::endl;
        oss << "    // Custom content" << std::endl;
        oss << "}" << std::endl;
    }
    return oss.str();
}

std::string CMODModule::generateOriginBlock() const {
    std::ostringstream oss;
    for (const auto& typePair : origin_exports_) {
        for (const auto& namePair : typePair.second) {
            oss << "[Origin] @" << typePair.first << " " << namePair.first << " {" << std::endl;
            oss << "    " << namePair.second << std::endl;
            oss << "}" << std::endl;
        }
    }
    return oss.str();
}

std::string CMODModule::generateConfigurationBlock() const {
    std::ostringstream oss;
    for (const auto& configPair : configuration_exports_) {
        oss << "[Configuration] " << configPair.first << " {" << std::endl;
        oss << "    " << configPair.second << std::endl;
        oss << "}" << std::endl;
    }
    return oss.str();
}

// CJMODModule 实现
CJMODModule::CJMODModule(const std::string& name) : info_(name) {
}

CJMODModule::~CJMODModule() = default;

ModuleInfo& CJMODModule::getInfo() {
    return info_;
}

const ModuleInfo& CJMODModule::getInfo() const {
    return info_;
}

void CJMODModule::setInfo(const ModuleInfo& info) {
    info_ = info;
}

void CJMODModule::addFunctionExport(const std::string& name, const std::string& syntax, const std::string& implementation) {
    function_exports_.emplace_back(name, syntax, implementation);
}

void CJMODModule::removeFunctionExport(const std::string& name) {
    function_exports_.erase(
        std::remove_if(function_exports_.begin(), function_exports_.end(),
            [&name](const std::tuple<std::string, std::string, std::string>& func) { 
                return std::get<0>(func) == name; 
            }),
        function_exports_.end()
    );
}

std::vector<std::tuple<std::string, std::string, std::string>> CJMODModule::getFunctionExports() const {
    return function_exports_;
}

bool CJMODModule::hasFunctionExport(const std::string& name) const {
    return std::any_of(function_exports_.begin(), function_exports_.end(),
        [&name](const std::tuple<std::string, std::string, std::string>& func) { 
            return std::get<0>(func) == name; 
        });
}

void CJMODModule::addVirtualObjectExport(const std::string& name, const std::string& implementation) {
    virtual_object_exports_.emplace_back(name, implementation);
}

void CJMODModule::removeVirtualObjectExport(const std::string& name) {
    virtual_object_exports_.erase(
        std::remove_if(virtual_object_exports_.begin(), virtual_object_exports_.end(),
            [&name](const std::pair<std::string, std::string>& obj) { 
                return obj.first == name; 
            }),
        virtual_object_exports_.end()
    );
}

std::vector<std::pair<std::string, std::string>> CJMODModule::getVirtualObjectExports() const {
    return virtual_object_exports_;
}

bool CJMODModule::hasVirtualObjectExport(const std::string& name) const {
    return std::any_of(virtual_object_exports_.begin(), virtual_object_exports_.end(),
        [&name](const std::pair<std::string, std::string>& obj) { 
            return obj.first == name; 
        });
}

bool CJMODModule::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    
    // 解析各个块
    if (!parseInfoBlock(content)) {
        return false;
    }
    
    if (!parseFunctionBlock(content)) {
        return false;
    }
    
    if (!parseVirtualObjectBlock(content)) {
        return false;
    }
    
    return true;
}

bool CJMODModule::saveToFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateInfoBlock() << std::endl;
    file << generateFunctionBlock() << std::endl;
    file << generateVirtualObjectBlock() << std::endl;
    
    return true;
}

bool CJMODModule::isValid() const {
    return !info_.name.empty() && validate();
}

bool CJMODModule::validate() const {
    // 验证模块信息
    if (info_.name.empty()) {
        return false;
    }
    
    // 验证函数导出
    for (const auto& func : function_exports_) {
        if (std::get<0>(func).empty() || std::get<1>(func).empty()) {
            return false;
        }
    }
    
    return true;
}

void CJMODModule::clear() {
    info_ = ModuleInfo();
    function_exports_.clear();
    virtual_object_exports_.clear();
}

bool CJMODModule::parseInfoBlock(const std::string& content) {
    std::regex infoRegex(R"(\[Info\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, infoRegex)) {
        std::string infoContent = match[1].str();
        
        // 解析名称
        std::regex nameRegex("name\\s*:\\s*\"([^\"]+)\"");
        std::smatch nameMatch;
        if (std::regex_search(infoContent, nameMatch, nameRegex)) {
            info_.name = nameMatch[1].str();
        }
        
        // 解析版本
        std::regex versionRegex("version\\s*:\\s*\"([^\"]+)\"");
        std::smatch versionMatch;
        if (std::regex_search(infoContent, versionMatch, versionRegex)) {
            info_.version = versionMatch[1].str();
        }
        
        return true;
    }
    
    return false;
}

bool CJMODModule::parseFunctionBlock(const std::string& content) {
    std::regex funcRegex(R"(\[Function\]\s*(\w+)\s*\{([^}]+)\})");
    std::sregex_iterator next(content.begin(), content.end(), funcRegex);
    std::sregex_iterator end;
    
    while (next != end) {
        std::smatch match = *next;
        std::string name = match[1].str();
        std::string body = match[2].str();
        
        // 解析语法和实现
        std::regex syntaxRegex("syntax\\s*:\\s*\"([^\"]+)\"");
        std::regex implRegex("implementation\\s*:\\s*\"([^\"]+)\"");
        
        std::smatch syntaxMatch, implMatch;
        std::string syntax, implementation;
        
        if (std::regex_search(body, syntaxMatch, syntaxRegex)) {
            syntax = syntaxMatch[1].str();
        }
        
        if (std::regex_search(body, implMatch, implRegex)) {
            implementation = implMatch[1].str();
        }
        
        function_exports_.emplace_back(name, syntax, implementation);
        next++;
    }
    
    return true;
}

bool CJMODModule::parseVirtualObjectBlock(const std::string& content) {
    std::regex objRegex(R"(\[VirtualObject\]\s*(\w+)\s*\{([^}]+)\})");
    std::sregex_iterator next(content.begin(), content.end(), objRegex);
    std::sregex_iterator end;
    
    while (next != end) {
        std::smatch match = *next;
        std::string name = match[1].str();
        std::string body = match[2].str();
        
        virtual_object_exports_.emplace_back(name, body);
        next++;
    }
    
    return true;
}

std::string CJMODModule::generateInfoBlock() const {
    std::ostringstream oss;
    oss << "[Info] {" << std::endl;
    oss << "    name: \"" << info_.name << "\";" << std::endl;
    oss << "    version: \"" << info_.version << "\";" << std::endl;
    if (!info_.description.empty()) {
        oss << "    description: \"" << info_.description << "\";" << std::endl;
    }
    oss << "}" << std::endl;
    return oss.str();
}

std::string CJMODModule::generateFunctionBlock() const {
    std::ostringstream oss;
    for (const auto& func : function_exports_) {
        oss << "[Function] " << std::get<0>(func) << " {" << std::endl;
        oss << "    syntax: \"" << std::get<1>(func) << "\";" << std::endl;
        oss << "    implementation: \"" << std::get<2>(func) << "\";" << std::endl;
        oss << "}" << std::endl;
    }
    return oss.str();
}

std::string CJMODModule::generateVirtualObjectBlock() const {
    std::ostringstream oss;
    for (const auto& obj : virtual_object_exports_) {
        oss << "[VirtualObject] " << obj.first << " {" << std::endl;
        oss << "    " << obj.second << std::endl;
        oss << "}" << std::endl;
    }
    return oss.str();
}

// HybridModule 实现
HybridModule::HybridModule(const std::string& name) : cmod_part_(name), cjmod_part_(name) {
}

HybridModule::~HybridModule() = default;

ModuleInfo& HybridModule::getInfo() {
    return info_;
}

const ModuleInfo& HybridModule::getInfo() const {
    return info_;
}

void HybridModule::setInfo(const ModuleInfo& info) {
    info_ = info;
}

CMODModule& HybridModule::getCMODPart() {
    return cmod_part_;
}

const CMODModule& HybridModule::getCMODPart() const {
    return cmod_part_;
}

CJMODModule& HybridModule::getCJMODPart() {
    return cjmod_part_;
}

const CJMODModule& HybridModule::getCJMODPart() const {
    return cjmod_part_;
}

bool HybridModule::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    
    return parseHybridFile(content);
}

bool HybridModule::saveToFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateHybridFile();
    return true;
}

bool HybridModule::isValid() const {
    return !info_.name.empty() && validate();
}

bool HybridModule::validate() const {
    return cmod_part_.validate() && cjmod_part_.validate();
}

void HybridModule::clear() {
    info_ = ModuleInfo();
    cmod_part_.clear();
    cjmod_part_.clear();
}

bool HybridModule::parseHybridFile(const std::string& content) {
    // 解析混合文件
    std::regex cmodRegex(R"(\[CMOD\]\s*\{([^}]+)\})");
    std::regex cjmodRegex(R"(\[CJMOD\]\s*\{([^}]+)\})");
    
    std::smatch cmodMatch, cjmodMatch;
    
    if (std::regex_search(content, cmodMatch, cmodRegex)) {
        if (!parseCMODSection(cmodMatch[1].str())) {
            return false;
        }
    }
    
    if (std::regex_search(content, cjmodMatch, cjmodRegex)) {
        if (!parseCJMODSection(cjmodMatch[1].str())) {
            return false;
        }
    }
    
    return true;
}

bool HybridModule::parseCMODSection(const std::string& content) {
    // 解析CMOD部分
    return cmod_part_.loadFromFile("temp_cmod.cmod");
}

bool HybridModule::parseCJMODSection(const std::string& content) {
    // 解析CJMOD部分
    return cjmod_part_.loadFromFile("temp_cjmod.cjmod");
}

std::string HybridModule::generateHybridFile() const {
    std::ostringstream oss;
    oss << "[CMOD] {" << std::endl;
    oss << cmod_part_.generateInfoBlock() << std::endl;
    oss << cmod_part_.generateExportBlock() << std::endl;
    oss << "}" << std::endl;
    
    oss << "[CJMOD] {" << std::endl;
    oss << cjmod_part_.generateInfoBlock() << std::endl;
    oss << cjmod_part_.generateFunctionBlock() << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

// ModuleManager 实现
ModuleManager::ModuleManager() {
    // 添加默认模块路径
    module_paths_.push_back(".");
    module_paths_.push_back("./module");
    module_paths_.push_back("../module");
}

ModuleManager::~ModuleManager() = default;

void ModuleManager::registerModule(std::shared_ptr<CMODModule> module) {
    if (module && !module->getInfo().name.empty()) {
        cmod_modules_[module->getInfo().name] = module;
    }
}

void ModuleManager::registerModule(std::shared_ptr<CJMODModule> module) {
    if (module && !module->getInfo().name.empty()) {
        cjmod_modules_[module->getInfo().name] = module;
    }
}

void ModuleManager::registerModule(std::shared_ptr<HybridModule> module) {
    if (module && !module->getInfo().name.empty()) {
        hybrid_modules_[module->getInfo().name] = module;
    }
}

std::shared_ptr<CMODModule> ModuleManager::getCMODModule(const std::string& name) const {
    auto it = cmod_modules_.find(name);
    if (it != cmod_modules_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<CJMODModule> ModuleManager::getCJMODModule(const std::string& name) const {
    auto it = cjmod_modules_.find(name);
    if (it != cjmod_modules_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<HybridModule> ModuleManager::getHybridModule(const std::string& name) const {
    auto it = hybrid_modules_.find(name);
    if (it != hybrid_modules_.end()) {
        return it->second;
    }
    return nullptr;
}

bool ModuleManager::loadModule(const std::string& name, const std::string& path) {
    std::string extension = path.substr(path.find_last_of('.') + 1);
    
    if (extension == "cmod") {
        return loadCMODModule(name, path);
    } else if (extension == "cjmod") {
        return loadCJMODModule(name, path);
    } else if (extension == "cmod") {
        return loadHybridModule(name, path);
    }
    
    return false;
}

bool ModuleManager::loadCMODModule(const std::string& name, const std::string& path) {
    auto module = std::make_shared<CMODModule>(name);
    if (module->loadFromFile(path)) {
        registerModule(module);
        return true;
    }
    return false;
}

bool ModuleManager::loadCJMODModule(const std::string& name, const std::string& path) {
    auto module = std::make_shared<CJMODModule>(name);
    if (module->loadFromFile(path)) {
        registerModule(module);
        return true;
    }
    return false;
}

bool ModuleManager::loadHybridModule(const std::string& name, const std::string& path) {
    auto module = std::make_shared<HybridModule>(name);
    if (module->loadFromFile(path)) {
        registerModule(module);
        return true;
    }
    return false;
}

std::vector<std::string> ModuleManager::findModules(const std::string& pattern) const {
    std::vector<std::string> modules;
    
    for (const auto& path : module_paths_) {
        std::vector<std::string> files = findFilesByPattern(path, pattern);
        modules.insert(modules.end(), files.begin(), files.end());
    }
    
    return modules;
}

std::vector<std::string> ModuleManager::findCMODModules() const {
    return findModules("*.cmod");
}

std::vector<std::string> ModuleManager::findCJMODModules() const {
    return findModules("*.cjmod");
}

std::vector<std::string> ModuleManager::findHybridModules() const {
    return findModules("*.cmod");
}

void ModuleManager::addModulePath(const std::string& path) {
    module_paths_.push_back(path);
}

void ModuleManager::removeModulePath(const std::string& path) {
    module_paths_.erase(
        std::remove(module_paths_.begin(), module_paths_.end(), path),
        module_paths_.end()
    );
}

std::vector<std::string> ModuleManager::getModulePaths() const {
    return module_paths_;
}

bool ModuleManager::resolveDependencies(const std::string& moduleName) {
    std::unordered_set<std::string> visited;
    return resolveModuleDependencies(moduleName, visited);
}

std::vector<std::string> ModuleManager::getDependencies(const std::string& moduleName) const {
    return getModuleDependencies(moduleName);
}

bool ModuleManager::validateModule(const std::string& moduleName) const {
    auto cmodModule = getCMODModule(moduleName);
    if (cmodModule) {
        return cmodModule->validate();
    }
    
    auto cjmodModule = getCJMODModule(moduleName);
    if (cjmodModule) {
        return cjmodModule->validate();
    }
    
    auto hybridModule = getHybridModule(moduleName);
    if (hybridModule) {
        return hybridModule->validate();
    }
    
    return false;
}

std::vector<std::string> ModuleManager::getModuleErrors(const std::string& moduleName) const {
    std::vector<std::string> errors;
    
    if (!validateModule(moduleName)) {
        errors.push_back("Module validation failed");
    }
    
    return errors;
}

void ModuleManager::clear() {
    cmod_modules_.clear();
    cjmod_modules_.clear();
    hybrid_modules_.clear();
}

void ModuleManager::unloadModule(const std::string& name) {
    cmod_modules_.erase(name);
    cjmod_modules_.erase(name);
    hybrid_modules_.erase(name);
}

std::string ModuleManager::findModuleFile(const std::string& name, const std::string& extension) const {
    for (const auto& path : module_paths_) {
        std::string fullPath = path + "/" + name + "." + extension;
        if (fileExists(fullPath)) {
            return fullPath;
        }
    }
    return "";
}

std::vector<std::string> ModuleManager::searchModuleFiles(const std::string& pattern) const {
    std::vector<std::string> files;
    
    for (const auto& path : module_paths_) {
        std::vector<std::string> foundFiles = findFilesByPattern(path, pattern);
        files.insert(files.end(), foundFiles.begin(), foundFiles.end());
    }
    
    return files;
}

bool ModuleManager::resolveModuleDependencies(const std::string& moduleName, std::unordered_set<std::string>& visited) {
    if (visited.find(moduleName) != visited.end()) {
        return true; // 循环依赖检测
    }
    
    visited.insert(moduleName);
    
    std::vector<std::string> dependencies = getModuleDependencies(moduleName);
    for (const auto& dep : dependencies) {
        if (!resolveModuleDependencies(dep, visited)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ModuleManager::getModuleDependencies(const std::string& moduleName) const {
    // 简化的依赖解析
    return {};
}

bool ModuleManager::fileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}

std::string ModuleManager::readFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

std::vector<std::string> ModuleManager::listFiles(const std::string& directory) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // 忽略文件系统错误
    }
    
    return files;
}

std::vector<std::string> ModuleManager::findFilesByPattern(const std::string& directory, const std::string& pattern) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find(pattern) != std::string::npos) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // 忽略文件系统错误
    }
    
    return files;
}

// ModulePackager 实现
ModulePackager::ModulePackager() = default;

ModulePackager::~ModulePackager() = default;

bool ModulePackager::packageModule(const std::string& moduleName, const std::string& outputPath) {
    // 简化的模块打包实现
    return true;
}

bool ModulePackager::packageCMODModule(const CMODModule& module, const std::string& outputPath) {
    return module.saveToFile(outputPath);
}

bool ModulePackager::packageCJMODModule(const CJMODModule& module, const std::string& outputPath) {
    return module.saveToFile(outputPath);
}

bool ModulePackager::packageHybridModule(const HybridModule& module, const std::string& outputPath) {
    return module.saveToFile(outputPath);
}

bool ModulePackager::unpackModule(const std::string& packagePath, const std::string& outputPath) {
    // 简化的模块解包实现
    return true;
}

bool ModulePackager::unpackCMODModule(const std::string& packagePath, const std::string& outputPath) {
    return true;
}

bool ModulePackager::unpackCJMODModule(const std::string& packagePath, const std::string& outputPath) {
    return true;
}

bool ModulePackager::unpackHybridModule(const std::string& packagePath, const std::string& outputPath) {
    return true;
}

bool ModulePackager::validatePackage(const std::string& packagePath) {
    return std::filesystem::exists(packagePath);
}

std::vector<std::string> ModulePackager::getPackageErrors(const std::string& packagePath) const {
    std::vector<std::string> errors;
    
    if (!std::filesystem::exists(packagePath)) {
        errors.push_back("Package file does not exist");
    }
    
    return errors;
}

ModuleInfo ModulePackager::getPackageInfo(const std::string& packagePath) const {
    ModuleInfo info;
    
    if (std::filesystem::exists(packagePath)) {
        std::ifstream file(packagePath);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            
            // 解析包信息
            std::regex nameRegex("name\\s*:\\s*\"([^\"]+)\"");
            std::smatch match;
            if (std::regex_search(content, match, nameRegex)) {
                info.name = match[1].str();
            }
        }
    }
    
    return info;
}

std::vector<std::string> ModulePackager::getPackageContents(const std::string& packagePath) const {
    std::vector<std::string> contents;
    
    if (std::filesystem::exists(packagePath)) {
        contents.push_back(packagePath);
    }
    
    return contents;
}

bool ModulePackager::createPackage(const std::string& packagePath, const std::string& content) {
    std::ofstream file(packagePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::string ModulePackager::readPackage(const std::string& packagePath) const {
    std::ifstream file(packagePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

bool ModulePackager::extractPackage(const std::string& packagePath, const std::string& outputPath) {
    // 简化的包提取实现
    return true;
}

std::string ModulePackager::compressData(const std::string& data) const {
    // 简化的数据压缩实现
    return data;
}

std::string ModulePackager::decompressData(const std::string& compressedData) const {
    // 简化的数据解压实现
    return compressedData;
}

std::string ModulePackager::generatePackageMetadata(const ModuleInfo& info) const {
    std::ostringstream oss;
    oss << "name: " << info.name << std::endl;
    oss << "version: " << info.version << std::endl;
    oss << "description: " << info.description << std::endl;
    oss << "author: " << info.author << std::endl;
    oss << "license: " << info.license << std::endl;
    return oss.str();
}

ModuleInfo ModulePackager::parsePackageMetadata(const std::string& metadata) const {
    ModuleInfo info;
    
    std::regex nameRegex(R"(name:\s*(.+))");
    std::regex versionRegex(R"(version:\s*(.+))");
    std::regex descRegex(R"(description:\s*(.+))");
    std::regex authorRegex(R"(author:\s*(.+))");
    std::regex licenseRegex(R"(license:\s*(.+))");
    
    std::smatch match;
    
    if (std::regex_search(metadata, match, nameRegex)) {
        info.name = match[1].str();
    }
    if (std::regex_search(metadata, match, versionRegex)) {
        info.version = match[1].str();
    }
    if (std::regex_search(metadata, match, descRegex)) {
        info.description = match[1].str();
    }
    if (std::regex_search(metadata, match, authorRegex)) {
        info.author = match[1].str();
    }
    if (std::regex_search(metadata, match, licenseRegex)) {
        info.license = match[1].str();
    }
    
    return info;
}

} // namespace CHTL
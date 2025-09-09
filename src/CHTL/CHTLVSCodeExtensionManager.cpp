#include "CHTL/CHTLVSCodeExtension.h"
#include <fstream>
#include <sstream>
#include <json/json.h>

namespace CHTL {

// VSCodeExtensionManager 实现
VSCodeExtensionManager::VSCodeExtensionManager(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    initializeComponents();
    loadDefaultConfiguration();
}

void VSCodeExtensionManager::initialize() {
    initializeComponents();
    loadDefaultConfiguration();
}

void VSCodeExtensionManager::shutdown() {
    // 清理资源
    if (m_pagePreviewer) {
        m_pagePreviewer->stopLivePreview();
    }
    
    // 保存配置
    saveDefaultConfiguration();
}

void VSCodeExtensionManager::reload() {
    shutdown();
    initialize();
}

std::shared_ptr<SyntaxHighlighter> VSCodeExtensionManager::getSyntaxHighlighter() const {
    return m_syntaxHighlighter;
}

std::shared_ptr<CodeFormatter> VSCodeExtensionManager::getCodeFormatter() const {
    return m_codeFormatter;
}

std::shared_ptr<CodeCompleter> VSCodeExtensionManager::getCodeCompleter() const {
    return m_codeCompleter;
}

std::shared_ptr<PagePreviewer> VSCodeExtensionManager::getPagePreviewer() const {
    return m_pagePreviewer;
}

std::shared_ptr<Exporter> VSCodeExtensionManager::getExporter() const {
    return m_exporter;
}

std::shared_ptr<BuiltinCompiler> VSCodeExtensionManager::getBuiltinCompiler() const {
    return m_builtinCompiler;
}

std::shared_ptr<ModuleUnpacker> VSCodeExtensionManager::getModuleUnpacker() const {
    return m_moduleUnpacker;
}

std::shared_ptr<JSONQueryTable> VSCodeExtensionManager::getJSONQueryTable() const {
    return m_jsonQueryTable;
}

std::shared_ptr<ModuleSyntaxHelper> VSCodeExtensionManager::getModuleSyntaxHelper() const {
    return m_moduleSyntaxHelper;
}

void VSCodeExtensionManager::loadConfiguration(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        m_context->addError("Cannot open configuration file: " + configFile);
        return;
    }
    
    Json::Value root;
    file >> root;
    
    // 解析配置
    if (root.isObject()) {
        for (const auto& key : root.getMemberNames()) {
            m_configuration[key] = root[key].asString();
        }
    }
}

void VSCodeExtensionManager::saveConfiguration(const std::string& configFile) {
    Json::Value root;
    
    for (const auto& [key, value] : m_configuration) {
        root[key] = value;
    }
    
    std::ofstream file(configFile);
    if (!file.is_open()) {
        m_context->addError("Cannot create configuration file: " + configFile);
        return;
    }
    
    Json::StreamWriterBuilder builder;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(root, &file);
}

void VSCodeExtensionManager::setConfiguration(const std::string& key, const std::string& value) {
    m_configuration[key] = value;
}

std::string VSCodeExtensionManager::getConfiguration(const std::string& key) const {
    auto it = m_configuration.find(key);
    return (it != m_configuration.end()) ? it->second : "";
}

void VSCodeExtensionManager::initializeComponents() {
    // 初始化所有组件
    m_syntaxHighlighter = std::make_shared<SyntaxHighlighter>(m_context);
    m_codeFormatter = std::make_shared<CodeFormatter>(m_context);
    m_codeCompleter = std::make_shared<CodeCompleter>(m_context);
    m_pagePreviewer = std::make_shared<PagePreviewer>(m_context);
    m_exporter = std::make_shared<Exporter>(m_context);
    m_builtinCompiler = std::make_shared<BuiltinCompiler>(m_context);
    m_moduleUnpacker = std::make_shared<ModuleUnpacker>(m_context);
    m_jsonQueryTable = std::make_shared<JSONQueryTable>(m_context);
    m_moduleSyntaxHelper = std::make_shared<ModuleSyntaxHelper>(m_context);
}

void VSCodeExtensionManager::loadDefaultConfiguration() {
    // 加载默认配置
    m_configuration["syntax.highlight.enabled"] = "true";
    m_configuration["syntax.highlight.theme"] = "default";
    m_configuration["formatting.enabled"] = "true";
    m_configuration["formatting.indent.size"] = "2";
    m_configuration["formatting.use.tabs"] = "false";
    m_configuration["completion.enabled"] = "true";
    m_configuration["preview.enabled"] = "true";
    m_configuration["preview.mode"] = "html";
    m_configuration["export.enabled"] = "true";
    m_configuration["export.format"] = "html";
    m_configuration["compilation.enabled"] = "true";
    m_configuration["compilation.mode"] = "release";
    m_configuration["modules.enabled"] = "true";
    m_configuration["query.enabled"] = "true";
    m_configuration["syntax.help.enabled"] = "true";
}

void VSCodeExtensionManager::saveDefaultConfiguration() {
    // 保存默认配置到文件
    std::string configFile = "chtl-vscode-config.json";
    saveConfiguration(configFile);
}

} // namespace CHTL
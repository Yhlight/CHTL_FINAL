#include "CHTLJSManage.h"
#include "CHTLJSLoader.h"
#include "CHTLJSGenerator.h"
#include <iostream>
#include <filesystem>

namespace CHTL {

CHTLJSManage::CHTLJSManage() {}

CHTLJSManage::~CHTLJSManage() {}

bool CHTLJSManage::compileFile(const std::string& filePath, const std::string& outputDir) {
    clearErrors();
    
    if (!validateOptions()) {
        addError("Invalid compilation options");
        return false;
    }
    
    // 创建加载器
    CHTLJSLoader loader;
    auto rootNode = loader.loadFile(filePath, context);
    
    if (!rootNode) {
        addError("Failed to load file: " + filePath);
        return false;
    }
    
    // 设置输出目录
    context.setOutputDir(outputDir);
    
    // 处理编译
    return processCompilation(rootNode.get(), outputDir);
}

bool CHTLJSManage::compileString(const std::string& content, const std::string& outputDir) {
    clearErrors();
    
    if (!validateOptions()) {
        addError("Invalid compilation options");
        return false;
    }
    
    // 创建加载器
    CHTLJSLoader loader;
    auto rootNode = loader.loadString(content, context);
    
    if (!rootNode) {
        addError("Failed to parse content");
        return false;
    }
    
    // 设置输出目录
    context.setOutputDir(outputDir);
    
    // 处理编译
    return processCompilation(rootNode.get(), outputDir);
}

void CHTLJSManage::setOption(const std::string& key, const std::string& value) {
    options[key] = value;
    context.setOption(key, value);
}

std::string CHTLJSManage::getOption(const std::string& key) const {
    auto it = options.find(key);
    return it != options.end() ? it->second : "";
}

const std::vector<std::string>& CHTLJSManage::getErrors() const {
    return errors;
}

void CHTLJSManage::clearErrors() {
    errors.clear();
}

void CHTLJSManage::addError(const std::string& error) {
    errors.push_back(error);
    std::cerr << "Error: " << error << std::endl;
}

bool CHTLJSManage::validateOptions() const {
    // 验证编译选项
    return true;
}

bool CHTLJSManage::processCompilation(CHTLJSBaseNode* rootNode, const std::string& outputDir) {
    if (!rootNode) {
        addError("No root node to process");
        return false;
    }
    
    try {
        // 创建输出目录
        std::filesystem::create_directories(outputDir);
        
        // 创建代码生成器
        CHTLJSGenerator generator;
        
        // 生成JavaScript
        std::string js = generator.generateJavaScript(rootNode);
        if (js.empty()) {
            addError("Failed to generate JavaScript");
            return false;
        }
        
        // 写入文件
        std::string baseName = std::filesystem::path(context.getSourcePath()).stem().string();
        
        // 写入JavaScript文件
        std::ofstream jsFile(outputDir + "/" + baseName + ".js");
        if (jsFile.is_open()) {
            jsFile << js;
            jsFile.close();
        } else {
            addError("Failed to write JavaScript file");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        addError("Compilation error: " + std::string(e.what()));
        return false;
    }
}

} // namespace CHTL
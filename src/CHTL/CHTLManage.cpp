#include "CHTLManage.h"
#include "CHTLLoader.h"
#include "CHTLGenerator.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace CHTL {

CHTLManage::CHTLManage() {}

CHTLManage::~CHTLManage() {}

bool CHTLManage::compileFile(const std::string& filePath, const std::string& outputDir) {
    clearErrors();
    
    if (!validateOptions()) {
        addError("Invalid compilation options");
        return false;
    }
    
    // 创建加载器
    CHTLLoader loader;
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

bool CHTLManage::compileString(const std::string& content, const std::string& outputDir) {
    clearErrors();
    
    if (!validateOptions()) {
        addError("Invalid compilation options");
        return false;
    }
    
    // 创建加载器
    CHTLLoader loader;
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

void CHTLManage::setOption(const std::string& key, const std::string& value) {
    options[key] = value;
    context.setOption(key, value);
}

std::string CHTLManage::getOption(const std::string& key) const {
    auto it = options.find(key);
    return it != options.end() ? it->second : "";
}

const std::vector<std::string>& CHTLManage::getErrors() const {
    return errors;
}

void CHTLManage::clearErrors() {
    errors.clear();
}

void CHTLManage::addError(const std::string& error) {
    errors.push_back(error);
    std::cerr << "Error: " << error << std::endl;
}

bool CHTLManage::validateOptions() const {
    // 验证编译选项
    return true;
}

bool CHTLManage::processCompilation(BaseNode* rootNode, const std::string& outputDir) {
    if (!rootNode) {
        addError("No root node to process");
        return false;
    }
    
    try {
        // 创建输出目录
        std::filesystem::create_directories(outputDir);
        
        // 创建代码生成器
        CHTLGenerator generator;
        
        // 生成HTML
        // 转换为ASTNode
        auto astNode = std::make_unique<ASTNode>(ASTNode::NodeType::ROOT);
        // 这里需要将BaseNode转换为ASTNode，简化处理
        std::string html = "<html><body>Generated HTML</body></html>";
        std::string css = "/* Generated CSS */";
        std::string js = "// Generated JavaScript";
        
        // 写入文件
        std::string baseName = std::filesystem::path(context.getSourcePath()).stem().string();
        
        // 写入HTML文件
        std::ofstream htmlFile(outputDir + "/" + baseName + ".html");
        if (htmlFile.is_open()) {
            htmlFile << html;
            htmlFile.close();
        } else {
            addError("Failed to write HTML file");
            return false;
        }
        
        // 写入CSS文件
        if (!css.empty()) {
            std::ofstream cssFile(outputDir + "/" + baseName + ".css");
            if (cssFile.is_open()) {
                cssFile << css;
                cssFile.close();
            }
        }
        
        // 写入JavaScript文件
        if (!js.empty()) {
            std::ofstream jsFile(outputDir + "/" + baseName + ".js");
            if (jsFile.is_open()) {
                jsFile << js;
                jsFile.close();
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        addError("Compilation error: " + std::string(e.what()));
        return false;
    }
}

} // namespace CHTL
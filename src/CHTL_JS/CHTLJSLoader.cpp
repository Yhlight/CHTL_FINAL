#include "CHTLJSLoader.h"
#include "CHTLJSLexer.h"
#include "CHTLJSParser.h"
#include <fstream>
#include <iostream>

namespace CHTL {

CHTLJSLoader::CHTLJSLoader() {}

CHTLJSLoader::~CHTLJSLoader() {}

std::unique_ptr<CHTLJSBaseNode> CHTLJSLoader::loadFile(const std::string& filePath, 
                                                      CHTLJSContext& context) {
    if (!fileExists(filePath)) {
        std::cerr << "Error: File not found: " << filePath << std::endl;
        return nullptr;
    }
    
    std::string content = readFile(filePath);
    if (content.empty()) {
        std::cerr << "Error: Failed to read file: " << filePath << std::endl;
        return nullptr;
    }
    
    context.setSourcePath(filePath);
    return loadString(content, context);
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSLoader::loadString(const std::string& content, 
                                                        CHTLJSContext& context) {
    return parseContent(content, context);
}

bool CHTLJSLoader::fileExists(const std::string& filePath) const {
    std::ifstream file(filePath);
    return file.good();
}

std::string CHTLJSLoader::readFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    return content;
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSLoader::parseContent(const std::string& content, 
                                                          CHTLJSContext& context) {
    // 创建词法分析器
    CHTLJSLexer lexer;
    auto tokens = lexer.tokenize(content);
    
    if (tokens.empty()) {
        std::cerr << "Error: No tokens generated" << std::endl;
        return nullptr;
    }
    
    // 创建语法分析器
    CHTLJSParser parser;
    parser.setTokens(tokens);
    auto rootNode = parser.parse();
    
    if (!rootNode) {
        std::cerr << "Error: Failed to parse content" << std::endl;
        return nullptr;
    }
    
    // 处理导入、模板、自定义等
    processNode(rootNode.get(), context);
    
    return std::unique_ptr<CHTLJSBaseNode>(rootNode.release());
}

void CHTLJSLoader::processNode(CHTLJSBaseNode* node, CHTLJSContext& context) {
    if (!node) return;
    
    // 处理不同类型的节点
    switch (node->getType()) {
        case CHTLJSNodeType::IMPORT: {
            auto importNode = static_cast<CHTLJSImportNode*>(node);
            processImport(importNode, context);
            break;
        }
        case CHTLJSNodeType::TEMPLATE: {
            auto templateNode = static_cast<CHTLJSTemplateNode*>(node);
            processTemplate(templateNode, context);
            break;
        }
        case CHTLJSNodeType::CUSTOM: {
            auto customNode = static_cast<CHTLJSCustomNode*>(node);
            processCustom(customNode, context);
            break;
        }
        default:
            // 递归处理子节点
            for (auto& child : node->getChildren()) {
                processNode(child.get(), context);
            }
            break;
    }
}

void CHTLJSLoader::processImport(CHTLJSImportNode* importNode, CHTLJSContext& context) {
    if (!importNode) return;
    
    // 处理导入逻辑
    std::string importPath = importNode->getPath();
    std::string sourcePath = context.getSourcePath();
    
    // 构建完整路径
    std::string fullPath = sourcePath.substr(0, sourcePath.find_last_of("/\\")) + "/" + importPath;
    
    // 加载导入的文件
    CHTLJSLoader loader;
    auto importedNode = loader.loadFile(fullPath, context);
    
    if (importedNode) {
        // 将导入的内容添加到当前节点
        importNode->addChild(std::move(importedNode));
    }
}

void CHTLJSLoader::processTemplate(CHTLJSTemplateNode* templateNode, CHTLJSContext& context) {
    if (!templateNode) return;
    
    // 处理模板逻辑
    std::string templateName = templateNode->getName();
    context.addGlobalVariable("template_" + templateName, templateName);
}

void CHTLJSLoader::processCustom(CHTLJSCustomNode* customNode, CHTLJSContext& context) {
    if (!customNode) return;
    
    // 处理自定义逻辑
    std::string customName = customNode->getName();
    context.addGlobalVariable("custom_" + customName, customName);
}

} // namespace CHTL
#include "CHTLLoader.h"
#include "CHTLLexer.h"
#include "CHTLParser.h"
#include <fstream>
#include <iostream>

namespace CHTL {

CHTLLoader::CHTLLoader() {}

CHTLLoader::~CHTLLoader() {}

std::unique_ptr<BaseNode> CHTLLoader::loadFile(const std::string& filePath, 
                                              CHTLContext& context) {
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

std::unique_ptr<BaseNode> CHTLLoader::loadString(const std::string& content, 
                                                CHTLContext& context) {
    return parseContent(content, context);
}

bool CHTLLoader::fileExists(const std::string& filePath) const {
    std::ifstream file(filePath);
    return file.good();
}

std::string CHTLLoader::readFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    return content;
}

std::unique_ptr<BaseNode> CHTLLoader::parseContent(const std::string& content, 
                                                  CHTLContext& context) {
    // 创建词法分析器
    CHTLLexer lexer;
    lexer.setSource(content);
    auto tokens = lexer.tokenize();
    
    if (tokens.empty()) {
        std::cerr << "Error: No tokens generated" << std::endl;
        return nullptr;
    }
    
    // 创建语法分析器
    CHTLParser parser;
    auto astNode = parser.parse();
    
    // 转换为BaseNode（简化处理）
    auto rootNode = std::make_unique<BaseNode>(NodeType::ELEMENT);
    rootNode->setName("root");
    
    if (!rootNode) {
        std::cerr << "Error: Failed to parse content" << std::endl;
        return nullptr;
    }
    
    // 处理导入、模板、自定义等
    processNode(rootNode.get(), context);
    
    return rootNode;
}

void CHTLLoader::processNode(BaseNode* node, CHTLContext& context) {
    if (!node) return;
    
    // 处理不同类型的节点
    switch (node->getType()) {
        case NodeType::IMPORT: {
            auto importNode = static_cast<ImportNode*>(node);
            processImport(importNode, context);
            break;
        }
        case NodeType::TEMPLATE: {
            auto templateNode = static_cast<TemplateNode*>(node);
            processTemplate(templateNode, context);
            break;
        }
        case NodeType::CUSTOM: {
            auto customNode = static_cast<CustomNode*>(node);
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

void CHTLLoader::processImport(ImportNode* importNode, CHTLContext& context) {
    if (!importNode) return;
    
    // 处理导入逻辑
    std::string importPath = importNode->getPath();
    std::string sourcePath = context.getSourcePath();
    
    // 构建完整路径
    std::string fullPath = sourcePath.substr(0, sourcePath.find_last_of("/\\")) + "/" + importPath;
    
    // 加载导入的文件
    CHTLLoader loader;
    auto importedNode = loader.loadFile(fullPath, context);
    
    if (importedNode) {
        // 将导入的内容添加到当前节点
        importNode->addChild(std::move(importedNode));
    }
}

void CHTLLoader::processTemplate(TemplateNode* templateNode, CHTLContext& context) {
    if (!templateNode) return;
    
    // 处理模板逻辑
    std::string templateName = templateNode->getName();
    context.addGlobalVariable("template_" + templateName, templateName);
}

void CHTLLoader::processCustom(CustomNode* customNode, CHTLContext& context) {
    if (!customNode) return;
    
    // 处理自定义逻辑
    std::string customName = customNode->getName();
    context.addGlobalVariable("custom_" + customName, customName);
}

} // namespace CHTL
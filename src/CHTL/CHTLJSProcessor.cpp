#include "CHTL/CHTLJS.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// CHTLJSProcessor 实现
CHTLJSProcessor::CHTLJSProcessor(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    m_compiler = std::make_unique<CHTLJSCompiler>(context);
}

void CHTLJSProcessor::processCHTLJS(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 递归处理所有节点
    processNode(root);
}

void CHTLJSProcessor::processCHTLJSFile(const std::string& filePath) {
    std::string content = m_compiler->compileFile(filePath);
    if (!content.empty()) {
        // 将编译后的内容写入文件
        std::string outputPath = filePath;
        size_t lastDot = outputPath.find_last_of('.');
        if (lastDot != std::string::npos) {
            outputPath = outputPath.substr(0, lastDot) + ".js";
        } else {
            outputPath += ".js";
        }
        
        std::ofstream outputFile(outputPath);
        if (outputFile.is_open()) {
            outputFile << content;
            outputFile.close();
        }
    }
}

void CHTLJSProcessor::processLocalScripts(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 递归查找脚本节点
    std::function<void(std::shared_ptr<BaseNode>)> findScripts = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::ELEMENT) {
                auto elementNode = std::static_pointer_cast<ElementNode>(node);
                if (elementNode->getTagName() == "script") {
                    processScriptNode(node);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findScripts(child);
            }
        };
    
    findScripts(root);
}

void CHTLJSProcessor::processScriptNode(std::shared_ptr<BaseNode> scriptNode) {
    if (!scriptNode || scriptNode->getType() != NodeType::ELEMENT) {
        return;
    }
    
    auto elementNode = std::static_pointer_cast<ElementNode>(scriptNode);
    if (elementNode->getTagName() != "script") {
        return;
    }
    
    // 处理脚本内容
    std::string scriptContent;
    for (const auto& child : elementNode->getChildren()) {
        if (child->getType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            scriptContent += textNode->getContent();
        }
    }
    
    if (!scriptContent.empty()) {
        // 编译 CHTL JS 代码
        std::string compiledCode = m_compiler->compileCHTLJS(scriptContent);
        
        // 更新脚本内容
        elementNode->clearChildren();
        auto newTextNode = std::make_shared<TextNode>(compiledCode);
        elementNode->addChild(newTextNode);
    }
}

void CHTLJSProcessor::processEnhancedSelectors(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 递归查找选择器节点
    std::function<void(std::shared_ptr<BaseNode>)> findSelectors = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::ELEMENT) {
                auto elementNode = std::static_pointer_cast<ElementNode>(node);
                if (elementNode->getTagName() == "script") {
                    processSelectorNode(node);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findSelectors(child);
            }
        };
    
    findSelectors(root);
}

void CHTLJSProcessor::processSelectorNode(std::shared_ptr<BaseNode> selectorNode) {
    if (!selectorNode || selectorNode->getType() != NodeType::ELEMENT) {
        return;
    }
    
    auto elementNode = std::static_pointer_cast<ElementNode>(selectorNode);
    if (elementNode->getTagName() != "script") {
        return;
    }
    
    // 处理选择器内容
    std::string scriptContent;
    for (const auto& child : elementNode->getChildren()) {
        if (child->getType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            scriptContent += textNode->getContent();
        }
    }
    
    if (!scriptContent.empty()) {
        // 处理增强选择器
        std::string processedContent = m_compiler->processSelectors(scriptContent);
        
        // 更新脚本内容
        elementNode->clearChildren();
        auto newTextNode = std::make_shared<TextNode>(processedContent);
        elementNode->addChild(newTextNode);
    }
}

void CHTLJSProcessor::processEventListeners(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 递归查找事件监听器节点
    std::function<void(std::shared_ptr<BaseNode>)> findListeners = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::ELEMENT) {
                auto elementNode = std::static_pointer_cast<ElementNode>(node);
                if (elementNode->getTagName() == "script") {
                    processEventListenerNode(node);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findListeners(child);
            }
        };
    
    findListeners(root);
}

void CHTLJSProcessor::processEventListenerNode(std::shared_ptr<BaseNode> listenerNode) {
    if (!listenerNode || listenerNode->getType() != NodeType::ELEMENT) {
        return;
    }
    
    auto elementNode = std::static_pointer_cast<ElementNode>(listenerNode);
    if (elementNode->getTagName() != "script") {
        return;
    }
    
    // 处理事件监听器内容
    std::string scriptContent;
    for (const auto& child : elementNode->getChildren()) {
        if (child->getType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            scriptContent += textNode->getContent();
        }
    }
    
    if (!scriptContent.empty()) {
        // 处理事件监听器
        std::string processedContent = m_compiler->processEventListeners(scriptContent);
        
        // 更新脚本内容
        elementNode->clearChildren();
        auto newTextNode = std::make_shared<TextNode>(processedContent);
        elementNode->addChild(newTextNode);
    }
}

void CHTLJSProcessor::processAnimations(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 递归查找动画节点
    std::function<void(std::shared_ptr<BaseNode>)> findAnimations = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::ELEMENT) {
                auto elementNode = std::static_pointer_cast<ElementNode>(node);
                if (elementNode->getTagName() == "script") {
                    processAnimationNode(node);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findAnimations(child);
            }
        };
    
    findAnimations(root);
}

void CHTLJSProcessor::processAnimationNode(std::shared_ptr<BaseNode> animationNode) {
    if (!animationNode || animationNode->getType() != NodeType::ELEMENT) {
        return;
    }
    
    auto elementNode = std::static_pointer_cast<ElementNode>(animationNode);
    if (elementNode->getTagName() != "script") {
        return;
    }
    
    // 处理动画内容
    std::string scriptContent;
    for (const auto& child : elementNode->getChildren()) {
        if (child->getType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            scriptContent += textNode->getContent();
        }
    }
    
    if (!scriptContent.empty()) {
        // 处理动画
        std::string processedContent = m_compiler->processAnimations(scriptContent);
        
        // 更新脚本内容
        elementNode->clearChildren();
        auto newTextNode = std::make_shared<TextNode>(processedContent);
        elementNode->addChild(newTextNode);
    }
}

void CHTLJSProcessor::processVirtualObjects(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 递归查找虚对象节点
    std::function<void(std::shared_ptr<BaseNode>)> findVirtualObjects = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::ELEMENT) {
                auto elementNode = std::static_pointer_cast<ElementNode>(node);
                if (elementNode->getTagName() == "script") {
                    processVirtualObjectNode(node);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findVirtualObjects(child);
            }
        };
    
    findVirtualObjects(root);
}

void CHTLJSProcessor::processVirtualObjectNode(std::shared_ptr<BaseNode> virtualObjectNode) {
    if (!virtualObjectNode || virtualObjectNode->getType() != NodeType::ELEMENT) {
        return;
    }
    
    auto elementNode = std::static_pointer_cast<ElementNode>(virtualObjectNode);
    if (elementNode->getTagName() != "script") {
        return;
    }
    
    // 处理虚对象内容
    std::string scriptContent;
    for (const auto& child : elementNode->getChildren()) {
        if (child->getType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            scriptContent += textNode->getContent();
        }
    }
    
    if (!scriptContent.empty()) {
        // 处理虚对象
        std::string processedContent = m_compiler->processVirtualObjects(scriptContent);
        
        // 更新脚本内容
        elementNode->clearChildren();
        auto newTextNode = std::make_shared<TextNode>(processedContent);
        elementNode->addChild(newTextNode);
    }
}

void CHTLJSProcessor::processRouter(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 递归查找路由节点
    std::function<void(std::shared_ptr<BaseNode>)> findRouter = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::ELEMENT) {
                auto elementNode = std::static_pointer_cast<ElementNode>(node);
                if (elementNode->getTagName() == "script") {
                    processRouterNode(node);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findRouter(child);
            }
        };
    
    findRouter(root);
}

void CHTLJSProcessor::processRouterNode(std::shared_ptr<BaseNode> routerNode) {
    if (!routerNode || routerNode->getType() != NodeType::ELEMENT) {
        return;
    }
    
    auto elementNode = std::static_pointer_cast<ElementNode>(routerNode);
    if (elementNode->getTagName() != "script") {
        return;
    }
    
    // 处理路由内容
    std::string scriptContent;
    for (const auto& child : elementNode->getChildren()) {
        if (child->getType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            scriptContent += textNode->getContent();
        }
    }
    
    if (!scriptContent.empty()) {
        // 处理路由
        std::string processedContent = m_compiler->processRouter(scriptContent);
        
        // 更新脚本内容
        elementNode->clearChildren();
        auto newTextNode = std::make_shared<TextNode>(processedContent);
        elementNode->addChild(newTextNode);
    }
}

void CHTLJSProcessor::processNode(std::shared_ptr<BaseNode> node) {
    if (!node) {
        return;
    }
    
    // 根据节点类型处理
    switch (node->getType()) {
        case NodeType::ELEMENT:
            processElementNode(std::static_pointer_cast<ElementNode>(node));
            break;
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                processNode(child);
            }
            break;
    }
}

void CHTLJSProcessor::processElementNode(std::shared_ptr<ElementNode> elementNode) {
    if (!elementNode) {
        return;
    }
    
    std::string tagName = elementNode->getTagName();
    
    if (tagName == "script") {
        processScriptNode(elementNode);
    } else if (tagName == "style") {
        processStyleNode(elementNode);
    }
    
    // 递归处理子节点
    for (const auto& child : elementNode->getChildren()) {
        processNode(child);
    }
}

void CHTLJSProcessor::processScriptNode(std::shared_ptr<ElementNode> scriptNode) {
    if (!scriptNode) {
        return;
    }
    
    // 处理脚本内容
    std::string scriptContent;
    for (const auto& child : scriptNode->getChildren()) {
        if (child->getType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            scriptContent += textNode->getContent();
        }
    }
    
    if (!scriptContent.empty()) {
        // 编译 CHTL JS 代码
        std::string compiledCode = m_compiler->compileCHTLJS(scriptContent);
        
        // 更新脚本内容
        scriptNode->clearChildren();
        auto newTextNode = std::make_shared<TextNode>(compiledCode);
        scriptNode->addChild(newTextNode);
    }
}

void CHTLJSProcessor::processStyleNode(std::shared_ptr<ElementNode> styleNode) {
    if (!styleNode) {
        return;
    }
    
    // 处理样式内容
    std::string styleContent;
    for (const auto& child : styleNode->getChildren()) {
        if (child->getType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            styleContent += textNode->getContent();
        }
    }
    
    if (!styleContent.empty()) {
        // 处理样式中的 CHTL JS 代码
        std::string processedContent = m_compiler->compileCHTLJS(styleContent);
        
        // 更新样式内容
        styleNode->clearChildren();
        auto newTextNode = std::make_shared<TextNode>(processedContent);
        styleNode->addChild(newTextNode);
    }
}

} // namespace CHTL
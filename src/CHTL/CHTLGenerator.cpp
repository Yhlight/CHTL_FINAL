#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include <sstream>

namespace CHTL {

CHTLGenerator::CHTLGenerator() {}

std::string CHTLGenerator::generateHTML(std::unique_ptr<BaseNode> root) {
    if (!root) {
        return "";
    }
    
    std::stringstream ss;
    
    for (size_t i = 0; i < root->getChildCount(); ++i) {
        BaseNode* child = root->getChild(i);
        if (!child) continue;
        
        switch (child->getType()) {
            case NodeType::ELEMENT: {
                auto* element = static_cast<ElementNode*>(child);
                ss << element->toHTML();
                break;
            }
            case NodeType::TEXT: {
                auto* text = static_cast<TextNode*>(child);
                ss << text->toHTML();
                break;
            }
            case NodeType::COMMENT: {
                auto* comment = static_cast<CommentNode*>(child);
                ss << comment->toHTML();
                break;
            }
            case NodeType::STYLE: {
                auto* style = static_cast<StyleNode*>(child);
                ss << style->toHTML();
                break;
            }
            case NodeType::SCRIPT: {
                auto* script = static_cast<ScriptNode*>(child);
                ss << script->toHTML();
                break;
            }
            default:
                // 其他节点类型暂时跳过
                break;
        }
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateCSS(std::unique_ptr<BaseNode> root) {
    if (!root) {
        return "";
    }
    
    std::stringstream ss;
    
    for (size_t i = 0; i < root->getChildCount(); ++i) {
        BaseNode* child = root->getChild(i);
        if (!child) continue;
        
        if (child->getType() == NodeType::STYLE) {
            auto* style = static_cast<StyleNode*>(child);
            ss << style->toCSS();
        }
        
        // 递归处理子节点
        for (size_t j = 0; j < child->getChildCount(); ++j) {
            BaseNode* grandChild = child->getChild(j);
            if (grandChild && grandChild->getType() == NodeType::STYLE) {
                auto* style = static_cast<StyleNode*>(grandChild);
                ss << style->toCSS();
            }
        }
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateJS(std::unique_ptr<BaseNode> root) {
    if (!root) {
        return "";
    }
    
    std::stringstream ss;
    
    for (size_t i = 0; i < root->getChildCount(); ++i) {
        BaseNode* child = root->getChild(i);
        if (!child) continue;
        
        if (child->getType() == NodeType::SCRIPT) {
            auto* script = static_cast<ScriptNode*>(child);
            ss << script->toJS();
        }
        
        // 递归处理子节点
        for (size_t j = 0; j < child->getChildCount(); ++j) {
            BaseNode* grandChild = child->getChild(j);
            if (grandChild && grandChild->getType() == NodeType::SCRIPT) {
                auto* script = static_cast<ScriptNode*>(grandChild);
                ss << script->toJS();
            }
        }
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateCompleteHTML(std::unique_ptr<BaseNode> root) {
    if (!root) {
        return "";
    }
    
    std::stringstream ss;
    
    // HTML5文档声明
    ss << "<!DOCTYPE html>\n";
    ss << "<html lang=\"zh-CN\">\n";
    
    // 生成HTML内容
    std::string htmlContent = generateHTML(std::move(root));
    ss << htmlContent;
    
    // 生成CSS样式
    std::string cssContent = generateCSS(root->clone());
    if (!cssContent.empty()) {
        ss << "<style>\n" << cssContent << "\n</style>\n";
    }
    
    // 生成JavaScript代码
    std::string jsContent = generateJS(root->clone());
    if (!jsContent.empty()) {
        ss << "<script>\n" << jsContent << "\n</script>\n";
    }
    
    ss << "</html>\n";
    
    return ss.str();
}

std::string CHTLGenerator::generateElementHTML(BaseNode* node) {
    if (!node || node->getType() != NodeType::ELEMENT) {
        return "";
    }
    
    auto* element = static_cast<ElementNode*>(node);
    return element->toHTML();
}

std::string CHTLGenerator::generateTextHTML(BaseNode* node) {
    if (!node || node->getType() != NodeType::TEXT) {
        return "";
    }
    
    auto* text = static_cast<TextNode*>(node);
    return text->toHTML();
}

std::string CHTLGenerator::generateStyleCSS(BaseNode* node) {
    if (!node || node->getType() != NodeType::STYLE) {
        return "";
    }
    
    auto* style = static_cast<StyleNode*>(node);
    return style->toCSS();
}

std::string CHTLGenerator::generateScriptJS(BaseNode* node) {
    if (!node || node->getType() != NodeType::SCRIPT) {
        return "";
    }
    
    auto* script = static_cast<ScriptNode*>(node);
    return script->toJS();
}

std::string CHTLGenerator::generateTemplateHTML(BaseNode* node) {
    // 模板HTML生成尚未实现
    return "";
}

std::string CHTLGenerator::generateCustomHTML(BaseNode* node) {
    // 自定义HTML生成尚未实现
    return "";
}

std::string CHTLGenerator::generateOriginHTML(BaseNode* node) {
    // 原始嵌入HTML生成尚未实现
    return "";
}

std::string CHTLGenerator::generateImportHTML(BaseNode* node) {
    // 导入HTML生成尚未实现
    return "";
}

std::string CHTLGenerator::generateConfigurationHTML(BaseNode* node) {
    // 配置HTML生成尚未实现
    return "";
}

std::string CHTLGenerator::generateNamespaceHTML(BaseNode* node) {
    // 命名空间HTML生成尚未实现
    return "";
}

} // namespace CHTL
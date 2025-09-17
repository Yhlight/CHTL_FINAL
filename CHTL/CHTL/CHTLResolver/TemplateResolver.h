#pragma once

#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/PropertyNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLCloner/AstCloner.h"
#include <memory>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>

class TemplateResolver {
public:
    TemplateResolver() = default;

    std::shared_ptr<RootNode> resolve(std::shared_ptr<RootNode> rawAst) {
        collectTemplates(rawAst);
        return std::dynamic_pointer_cast<RootNode>(expandNode(rawAst));
    }

private:
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> templateRegistry;

    void collectTemplates(std::shared_ptr<BaseNode> node) {
        if (!node) return;
        if (auto rootNode = std::dynamic_pointer_cast<RootNode>(node)) {
            for (auto it = rootNode->children.begin(); it != rootNode->children.end(); ) {
                if (auto defNode = std::dynamic_pointer_cast<TemplateDefinitionNode>(*it)) {
                    templateRegistry[defNode->name] = defNode;
                    it = rootNode->children.erase(it);
                } else {
                    collectTemplates(*it);
                    ++it;
                }
            }
        }
    }

    std::shared_ptr<BaseNode> expandNode(std::shared_ptr<BaseNode> node) {
        if (!node) return nullptr;

        if (auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node)) {
            if (templateRegistry.count(usageNode->name)) {
                auto templateDef = templateRegistry[usageNode->name];
                AstCloner cloner;
                std::vector<std::shared_ptr<BaseNode>> clonedBody;
                for (const auto& bodyNode : templateDef->body) {
                    clonedBody.push_back(cloner.clone(bodyNode));
                }

                if (!usageNode->specializationBody.empty()) {
                    for (const auto& specNode : usageNode->specializationBody) {
                        if (auto specProp = std::dynamic_pointer_cast<PropertyNode>(specNode)) {
                            bool propertyFoundAndSet = false;
                            for (auto& clonedNode : clonedBody) {
                                if (auto clonedProp = std::dynamic_pointer_cast<PropertyNode>(clonedNode)) {
                                    if (clonedProp->key == specProp->key) {
                                        clonedProp->value = specProp->value;
                                        propertyFoundAndSet = true;
                                        break;
                                    }
                                }
                            }
                            if (!propertyFoundAndSet) {
                                clonedBody.push_back(specNode);
                            }
                        }
                    }
                }

                auto listWrapper = std::make_shared<RootNode>();
                listWrapper->children = clonedBody;
                return listWrapper;
            } else {
                throw std::runtime_error("Template not found: " + usageNode->name);
            }
        }

        // THIS IS THE FIX. The recursion must start at the root.
        if (auto rootNode = std::dynamic_pointer_cast<RootNode>(node)) {
            std::vector<std::shared_ptr<BaseNode>> newChildren;
            for (const auto& child : rootNode->children) {
                auto expandedChild = expandNode(child);
                if (auto expandedList = std::dynamic_pointer_cast<RootNode>(expandedChild)) {
                     newChildren.insert(newChildren.end(), expandedList->children.begin(), expandedList->children.end());
                } else if (expandedChild) {
                    newChildren.push_back(expandedChild);
                }
            }
            rootNode->children = newChildren;
        }
        else if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
            std::vector<std::shared_ptr<BaseNode>> newChildren;
            for (const auto& child : elementNode->children) {
                auto expandedChild = expandNode(child);
                if (auto expandedList = std::dynamic_pointer_cast<RootNode>(expandedChild)) {
                     newChildren.insert(newChildren.end(), expandedList->children.begin(), expandedList->children.end());
                } else if (expandedChild) {
                    newChildren.push_back(expandedChild);
                }
            }
            elementNode->children = newChildren;
        } else if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(node)) {
            std::vector<std::shared_ptr<BaseNode>> newChildren;
            for (const auto& child : styleNode->children) {
                 auto expandedChild = expandNode(child);
                 if (auto expandedList = std::dynamic_pointer_cast<RootNode>(expandedChild)) {
                     newChildren.insert(newChildren.end(), expandedList->children.begin(), expandedList->children.end());
                } else if (expandedChild) {
                    newChildren.push_back(expandedChild);
                }
            }
            styleNode->children = newChildren;
        }

        return node;
    }
};

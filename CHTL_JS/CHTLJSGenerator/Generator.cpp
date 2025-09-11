#include "Generator.h"
#include "../CHTLJSNode/Node.h" // For DelegateNode

// Helper to trim leading/trailing whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string CHTLJSGenerator::generate(const CHTLJSNodeList& ast) {
    for (const auto& node : ast) {
        node->accept(*this);
    }
    return output.str();
}

void CHTLJSGenerator::visit(FileLoaderNode* node) {
    output << "// CHTL JS fileloader statement\n";
    for (const auto& file : node->files) {
        output << "// load: " << file << "\n";
    }
    output << "\n";
}

void CHTLJSGenerator::visit(ListenNode* node) {
    output << "{\n";
    for (size_t i = 0; i < node->eventHandlers.size(); ++i) {
        node->eventHandlers[i]->accept(*this);
        if (i < node->eventHandlers.size() - 1) {
            output << ",";
        }
        output << "\n";
    }
    output << "}";
}

void CHTLJSGenerator::visit(AnimateNode* node) {
    std::string target_str;
    std::string options_str = "{";
    std::string keyframes_str = "[";

    // The parser logic for animate is very basic, it just gives us key-value pairs.
    // We have to do more work here to generate correct JS.
    for (const auto& prop : node->properties) {
        std::string key = prop->key;
        std::string value = trim(prop->value);

        if (key == "target") {
            // The value of target is CHTL JS code, e.g. {{selector}}
            // We need a sub-compiler/generator to handle this.
            // This is a limitation of the current setup. For now, we'll assume it's a simple selector.
             if (value.find("{{") != std::string::npos) {
                size_t start = value.find("{{");
                size_t end = value.find("}}");
                std::string selector = value.substr(start + 2, end - start - 2);
                target_str = "document.querySelector('" + trim(selector) + "')";
            } else {
                target_str = value; // Assume it's already a JS variable
            }
        } else if (key == "duration" || key == "delay" || key == "loop") {
            options_str += key + ": " + value + ", ";
        } else if (key == "easing" || key == "direction") {
            options_str += key + ": '" + value + "', ";
        } else if (key == "begin") {
            keyframes_str += "{ \"offset\": 0, " + value + " }, ";
        } else if (key == "end") {
            keyframes_str += "{ \"offset\": 1, " + value + " }, ";
        } else if (key == "when") {
             // This is a simplified handler for 'when'. A real one would parse the array.
             keyframes_str += value + ", ";
        }
    }

    // Clean up trailing commas
    if (options_str.length() > 1) options_str.pop_back(); options_str.pop_back();
    if (keyframes_str.length() > 1) keyframes_str.pop_back(); keyframes_str.pop_back();

    options_str += "}";
    keyframes_str += "]";

    if (target_str.empty()) {
        output << "/* CHTL JS Error: 'animate' block requires a 'target' property. */";
        return;
    }

    output << target_str << ".animate(" << keyframes_str << ", " << options_str << ")";
}

void CHTLJSGenerator::visit(VirDeclNode* node) {
    output << "const " << node->name << " = ";
    node->value->accept(*this);
    output << ";\n";
}

void CHTLJSGenerator::visit(EnhancedSelectorNode* node) {
    // Basic implementation: assumes querySelector. A more advanced version
    // could check for multiple results and use querySelectorAll.
    output << "document.querySelector('" << node->selector << "')";
}

void CHTLJSGenerator::visit(CHTLJSPropertyNode* node) {
    output << "  " << node->key << ": " << trim(node->value);
}

void CHTLJSGenerator::visit(MethodCallNode* node) {
    if (node->methodName == "listen") {
        if (auto* listenBlock = dynamic_cast<ListenNode*>(node->arguments.get())) {
            for (const auto& handler : listenBlock->eventHandlers) {
                node->callee->accept(*this);
                output << ".addEventListener('" << handler->key << "', " << trim(handler->value) << ");\n";
            }
        }
    } else if (node->methodName == "delegate") {
        if (auto* delegateBlock = dynamic_cast<DelegateNode*>(node->arguments.get())) {
            // Extract target and event handlers from the delegate block
            std::string target_selector_str;
            std::map<std::string, std::string> event_handlers;
            for (const auto& prop : delegateBlock->properties) {
                if (prop->key == "target") {
                    target_selector_str = trim(prop->value);
                } else {
                    event_handlers[prop->key] = trim(prop->value);
                }
            }

            if (target_selector_str.empty()) {
                output << "/* CHTL JS Error: 'delegate' block requires a 'target' property. */\n";
                return;
            }

            // Generate the parent selector string by temporarily redirecting the output
            std::stringstream parent_ss;
            std::swap(parent_ss, output);
            node->callee->accept(*this);
            std::swap(parent_ss, output);
            std::string parent_selector_str = parent_ss.str();

            output << "(function() {\n";
            output << "  window.__chtl_delegates = window.__chtl_delegates || {};\n";
            output << "  const parentSelector = " << parent_selector_str << ";\n";
            output << "  const parentId = parentSelector.id || parentSelector.tagName;\n"; // Simplified ID
            output << "  if (!window.__chtl_delegates[parentId]) {\n";
            output << "    window.__chtl_delegates[parentId] = { handlers: {}, listener: function(event) {\n";
            output << "      const parentHandlers = window.__chtl_delegates[parentId].handlers;\n";
            output << "      for (const childSelector in parentHandlers) {\n";
            output << "        if (event.target.matches(childSelector)) {\n";
            output << "          if (parentHandlers[childSelector][event.type]) {\n";
            output << "            parentHandlers[childSelector][event.type].call(event.target, event);\n";
            output << "          }\n        }\n      }\n    }};\n";
            // Add listeners for all event types
            for(const auto& pair : event_handlers) {
                 output << "    parentSelector.addEventListener('" << pair.first << "', window.__chtl_delegates[parentId].listener);\n";
            }
            output << "  }\n";

            // Add the specific handlers for this call
            output << "  const childSelector = \"" << target_selector_str << "\";\n";
            output << "  window.__chtl_delegates[parentId].handlers[childSelector] = window.__chtl_delegates[parentId].handlers[childSelector] || {};\n";
            for(const auto& pair : event_handlers) {
                 output << "  window.__chtl_delegates[parentId].handlers[childSelector]['" << pair.first << "'] = " << pair.second << ";\n";
            }
            output << "})();\n";
        }
    }
}

void CHTLJSGenerator::visit(DelegateNode* node) {
    // This node is handled by the MethodCallNode visitor
}

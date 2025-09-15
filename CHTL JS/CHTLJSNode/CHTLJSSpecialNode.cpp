#include "CHTLJSSpecialNode.h"
#include <sstream>

namespace CHTLJS {

// ScriptLoaderNode implementation
ScriptLoaderNode::ScriptLoaderNode()
    : CHTLJSBaseNode(NodeType::SCRIPT_LOADER) {
}

ScriptLoaderNode::~ScriptLoaderNode() {
}

void ScriptLoaderNode::setModuleName(const std::string& name) {
    module_name_ = name;
}

const std::string& ScriptLoaderNode::getModuleName() const {
    return module_name_;
}

void ScriptLoaderNode::setModulePath(const std::string& path) {
    module_path_ = path;
}

const std::string& ScriptLoaderNode::getModulePath() const {
    return module_path_;
}

void ScriptLoaderNode::setCallback(std::shared_ptr<CHTLJSBaseNode> callback) {
    callback_ = callback;
}

std::shared_ptr<CHTLJSBaseNode> ScriptLoaderNode::getCallback() const {
    return callback_;
}

std::string ScriptLoaderNode::generateCode() const {
    std::stringstream ss;
    ss << "ScriptLoader {\n";
    ss << "    module: \"" << module_name_ << "\",\n";
    ss << "    path: \"" << module_path_ << "\"";
    if (callback_) {
        ss << ",\n    callback: " << callback_->generateCode();
    }
    ss << "\n}";
    return ss.str();
}

std::string ScriptLoaderNode::toString() const {
    return "ScriptLoaderNode(" + module_name_ + ")";
}

// ListenNode implementation
ListenNode::ListenNode()
    : CHTLJSBaseNode(NodeType::LISTEN) {
}

ListenNode::~ListenNode() {
}

void ListenNode::setSelector(const std::string& selector) {
    selector_ = selector;
}

const std::string& ListenNode::getSelector() const {
    return selector_;
}

void ListenNode::addEvent(const std::string& event, std::shared_ptr<CHTLJSBaseNode> handler) {
    events_[event] = handler;
}

const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& ListenNode::getEvents() const {
    return events_;
}

std::string ListenNode::generateCode() const {
    std::stringstream ss;
    ss << "Listen {\n";
    ss << "    selector: \"" << selector_ << "\",\n";
    ss << "    events: {\n";
    for (const auto& event : events_) {
        ss << "        " << event.first << ": " << event.second->generateCode() << ",\n";
    }
    ss << "    }\n";
    ss << "}";
    return ss.str();
}

std::string ListenNode::toString() const {
    return "ListenNode(" + selector_ + ")";
}

// AnimateNode implementation
AnimateNode::AnimateNode()
    : CHTLJSBaseNode(NodeType::ANIMATE) {
}

AnimateNode::~AnimateNode() {
}

void AnimateNode::setTarget(const std::string& target) {
    target_ = target;
}

const std::string& AnimateNode::getTarget() const {
    return target_;
}

void AnimateNode::setDuration(const std::string& duration) {
    duration_ = duration;
}

const std::string& AnimateNode::getDuration() const {
    return duration_;
}

void AnimateNode::setEasing(const std::string& easing) {
    easing_ = easing;
}

const std::string& AnimateNode::getEasing() const {
    return easing_;
}

void AnimateNode::addProperty(const std::string& property, const std::string& value) {
    properties_[property] = value;
}

const std::map<std::string, std::string>& AnimateNode::getProperties() const {
    return properties_;
}

void AnimateNode::setCallback(std::shared_ptr<CHTLJSBaseNode> callback) {
    callback_ = callback;
}

std::shared_ptr<CHTLJSBaseNode> AnimateNode::getCallback() const {
    return callback_;
}

std::string AnimateNode::generateCode() const {
    std::stringstream ss;
    ss << "Animate {\n";
    ss << "    target: \"" << target_ << "\",\n";
    ss << "    duration: \"" << duration_ << "\",\n";
    ss << "    easing: \"" << easing_ << "\",\n";
    ss << "    properties: {\n";
    for (const auto& prop : properties_) {
        ss << "        " << prop.first << ": \"" << prop.second << "\",\n";
    }
    ss << "    }";
    if (callback_) {
        ss << ",\n    callback: " << callback_->generateCode();
    }
    ss << "\n}";
    return ss.str();
}

std::string AnimateNode::toString() const {
    return "AnimateNode(" + target_ + ")";
}

// RouterNode implementation
RouterNode::RouterNode()
    : CHTLJSBaseNode(NodeType::ROUTER) {
}

RouterNode::~RouterNode() {
}

void RouterNode::addRoute(const std::string& path, std::shared_ptr<CHTLJSBaseNode> handler) {
    routes_[path] = handler;
}

const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& RouterNode::getRoutes() const {
    return routes_;
}

void RouterNode::setDefaultRoute(std::shared_ptr<CHTLJSBaseNode> handler) {
    default_route_ = handler;
}

std::shared_ptr<CHTLJSBaseNode> RouterNode::getDefaultRoute() const {
    return default_route_;
}

std::string RouterNode::generateCode() const {
    std::stringstream ss;
    ss << "Router {\n";
    ss << "    routes: {\n";
    for (const auto& route : routes_) {
        ss << "        \"" << route.first << "\": " << route.second->generateCode() << ",\n";
    }
    ss << "    }";
    if (default_route_) {
        ss << ",\n    default: " << default_route_->generateCode();
    }
    ss << "\n}";
    return ss.str();
}

std::string RouterNode::toString() const {
    return "RouterNode(" + std::to_string(routes_.size()) + " routes)";
}

// VirNode implementation
VirNode::VirNode()
    : CHTLJSBaseNode(NodeType::VIR) {
}

VirNode::~VirNode() {
}

void VirNode::setObjectName(const std::string& name) {
    object_name_ = name;
}

const std::string& VirNode::getObjectName() const {
    return object_name_;
}

void VirNode::addProperty(const std::string& name, std::shared_ptr<CHTLJSBaseNode> value) {
    properties_[name] = value;
}

const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& VirNode::getProperties() const {
    return properties_;
}

void VirNode::addMethod(const std::string& name, std::shared_ptr<CHTLJSBaseNode> method) {
    methods_[name] = method;
}

const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& VirNode::getMethods() const {
    return methods_;
}

std::string VirNode::generateCode() const {
    std::stringstream ss;
    ss << "Vir " << object_name_ << " {\n";
    
    if (!properties_.empty()) {
        ss << "    properties: {\n";
        for (const auto& prop : properties_) {
            ss << "        " << prop.first << ": " << prop.second->generateCode() << ",\n";
        }
        ss << "    },\n";
    }
    
    if (!methods_.empty()) {
        ss << "    methods: {\n";
        for (const auto& method : methods_) {
            ss << "        " << method.first << ": " << method.second->generateCode() << ",\n";
        }
        ss << "    }";
    }
    
    ss << "\n}";
    return ss.str();
}

std::string VirNode::toString() const {
    return "VirNode(" + object_name_ + ")";
}

// UtilThenNode implementation
UtilThenNode::UtilThenNode()
    : CHTLJSBaseNode(NodeType::UTIL_THEN) {
}

UtilThenNode::~UtilThenNode() {
}

void UtilThenNode::setTarget(std::shared_ptr<CHTLJSBaseNode> target) {
    target_ = target;
}

std::shared_ptr<CHTLJSBaseNode> UtilThenNode::getTarget() const {
    return target_;
}

void UtilThenNode::addOperation(const std::string& operation, std::shared_ptr<CHTLJSBaseNode> value) {
    operations_[operation] = value;
}

const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& UtilThenNode::getOperations() const {
    return operations_;
}

std::string UtilThenNode::generateCode() const {
    std::stringstream ss;
    if (target_) {
        ss << target_->generateCode();
    }
    ss << ".util.then {\n";
    for (const auto& op : operations_) {
        ss << "    " << op.first << ": " << op.second->generateCode() << ",\n";
    }
    ss << "}";
    return ss.str();
}

std::string UtilThenNode::toString() const {
    return "UtilThenNode";
}

// PrintMyloveNode implementation
PrintMyloveNode::PrintMyloveNode()
    : CHTLJSBaseNode(NodeType::PRINTMYLOVE), width_(80), height_(24) {
}

PrintMyloveNode::~PrintMyloveNode() {
}

void PrintMyloveNode::setImagePath(const std::string& path) {
    image_path_ = path;
}

const std::string& PrintMyloveNode::getImagePath() const {
    return image_path_;
}

void PrintMyloveNode::setWidth(int width) {
    width_ = width;
}

int PrintMyloveNode::getWidth() const {
    return width_;
}

void PrintMyloveNode::setHeight(int height) {
    height_ = height;
}

int PrintMyloveNode::getHeight() const {
    return height_;
}

void PrintMyloveNode::setCharSet(const std::string& charset) {
    charset_ = charset;
}

const std::string& PrintMyloveNode::getCharSet() const {
    return charset_;
}

std::string PrintMyloveNode::generateCode() const {
    std::stringstream ss;
    ss << "printMylove {\n";
    ss << "    image: \"" << image_path_ << "\",\n";
    ss << "    width: " << width_ << ",\n";
    ss << "    height: " << height_ << ",\n";
    ss << "    charset: \"" << charset_ << "\"\n";
    ss << "}";
    return ss.str();
}

std::string PrintMyloveNode::toString() const {
    return "PrintMyloveNode(" + image_path_ + ")";
}

// INeverAwayNode implementation
INeverAwayNode::INeverAwayNode()
    : CHTLJSBaseNode(NodeType::INEVERAWAY) {
}

INeverAwayNode::~INeverAwayNode() {
}

void INeverAwayNode::setFunctionName(const std::string& name) {
    function_name_ = name;
}

const std::string& INeverAwayNode::getFunctionName() const {
    return function_name_;
}

void INeverAwayNode::addOverload(const std::vector<std::string>& parameters, std::shared_ptr<CHTLJSBaseNode> body) {
    overloads_.push_back({parameters, body});
}

const std::vector<std::pair<std::vector<std::string>, std::shared_ptr<CHTLJSBaseNode>>>& INeverAwayNode::getOverloads() const {
    return overloads_;
}

std::string INeverAwayNode::generateCode() const {
    std::stringstream ss;
    ss << "iNeverAway " << function_name_ << " {\n";
    for (size_t i = 0; i < overloads_.size(); ++i) {
        ss << "    overload " << i << ": (";
        for (size_t j = 0; j < overloads_[i].first.size(); ++j) {
            if (j > 0) ss << ", ";
            ss << overloads_[i].first[j];
        }
        ss << ") -> " << overloads_[i].second->generateCode() << ",\n";
    }
    ss << "}";
    return ss.str();
}

std::string INeverAwayNode::toString() const {
    return "INeverAwayNode(" + function_name_ + ")";
}

// ResponsiveGetNode implementation
ResponsiveGetNode::ResponsiveGetNode(const std::string& variableName)
    : CHTLJSBaseNode(NodeType::RESPONSIVE_GET), variable_name_(variableName) {
}

ResponsiveGetNode::~ResponsiveGetNode() {
}

void ResponsiveGetNode::setVariableName(const std::string& name) {
    variable_name_ = name;
}

const std::string& ResponsiveGetNode::getVariableName() const {
    return variable_name_;
}

std::string ResponsiveGetNode::generateCode() const {
    return "$" + variable_name_ + "$";
}

std::string ResponsiveGetNode::toString() const {
    return "ResponsiveGetNode(" + variable_name_ + ")";
}

// ResponsiveSetNode implementation
ResponsiveSetNode::ResponsiveSetNode(const std::string& variableName, std::shared_ptr<CHTLJSBaseNode> value)
    : CHTLJSBaseNode(NodeType::RESPONSIVE_SET), variable_name_(variableName), value_(value) {
}

ResponsiveSetNode::~ResponsiveSetNode() {
}

void ResponsiveSetNode::setVariableName(const std::string& name) {
    variable_name_ = name;
}

const std::string& ResponsiveSetNode::getVariableName() const {
    return variable_name_;
}

void ResponsiveSetNode::setValue(std::shared_ptr<CHTLJSBaseNode> value) {
    value_ = value;
}

std::shared_ptr<CHTLJSBaseNode> ResponsiveSetNode::getValue() const {
    return value_;
}

std::string ResponsiveSetNode::generateCode() const {
    std::stringstream ss;
    ss << "$" << variable_name_ << "$ = ";
    if (value_) {
        ss << value_->generateCode();
    }
    return ss.str();
}

std::string ResponsiveSetNode::toString() const {
    return "ResponsiveSetNode(" + variable_name_ + ")";
}

// CHTLSelectorNode implementation
CHTLSelectorNode::CHTLSelectorNode(const std::string& selector)
    : CHTLJSBaseNode(NodeType::CHTL_SELECTOR), selector_(selector) {
}

CHTLSelectorNode::~CHTLSelectorNode() {
}

void CHTLSelectorNode::setSelector(const std::string& selector) {
    selector_ = selector;
}

const std::string& CHTLSelectorNode::getSelector() const {
    return selector_;
}

std::string CHTLSelectorNode::generateCode() const {
    return "{{" + selector_ + "}}";
}

std::string CHTLSelectorNode::toString() const {
    return "CHTLSelectorNode(" + selector_ + ")";
}

// CHTLResponsiveNode implementation
CHTLResponsiveNode::CHTLResponsiveNode(const std::string& variableName)
    : CHTLJSBaseNode(NodeType::CHTL_RESPONSIVE), variable_name_(variableName) {
}

CHTLResponsiveNode::~CHTLResponsiveNode() {
}

void CHTLResponsiveNode::setVariableName(const std::string& name) {
    variable_name_ = name;
}

const std::string& CHTLResponsiveNode::getVariableName() const {
    return variable_name_;
}

std::string CHTLResponsiveNode::generateCode() const {
    return "$" + variable_name_ + "$";
}

std::string CHTLResponsiveNode::toString() const {
    return "CHTLResponsiveNode(" + variable_name_ + ")";
}

} // namespace CHTLJS
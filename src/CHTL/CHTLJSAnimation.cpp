#include "CHTL/CHTLJS.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// CHTLJSAnimation 实现
CHTLJSAnimation::CHTLJSAnimation(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string CHTLJSAnimation::parseAnimation(const std::string& animationConfig) const {
    auto properties = parseAnimationProperties(animationConfig);
    return generateAnimationFunction(properties);
}

std::unordered_map<std::string, std::string> CHTLJSAnimation::parseAnimationProperties(const std::string& config) const {
    std::unordered_map<std::string, std::string> properties;
    
    // 解析动画配置
    std::regex propertyRegex(R"(\s*(\w+)\s*:\s*([^,;]+)(?:[,;]|$))");
    std::sregex_iterator iter(config.begin(), config.end(), propertyRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string key = match[1].str();
        std::string value = match[2].str();
        
        // 去除前后空格
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        properties[key] = value;
    }
    
    return properties;
}

std::string CHTLJSAnimation::generateAnimationCode(const std::string& animationConfig) const {
    auto properties = parseAnimationProperties(animationConfig);
    return generateAnimationFunction(properties);
}

std::string CHTLJSAnimation::generateAnimationFunction(const std::unordered_map<std::string, std::string>& properties) const {
    std::stringstream ss;
    
    ss << "function animate() {\n";
    ss << "  const startTime = performance.now();\n";
    ss << "  const duration = " << (properties.count("duration") ? properties.at("duration") : "1000") << ";\n";
    ss << "  const easing = '" << (properties.count("easing") ? properties.at("easing") : "ease") << "';\n";
    ss << "  const loop = " << (properties.count("loop") ? properties.at("loop") : "false") << ";\n";
    ss << "  const direction = '" << (properties.count("direction") ? properties.at("direction") : "normal") << "';\n";
    ss << "  const delay = " << (properties.count("delay") ? properties.at("delay") : "0") << ";\n\n";
    
    ss << "  function animationFrame(currentTime) {\n";
    ss << "    const elapsed = currentTime - startTime - delay;\n";
    ss << "    const progress = Math.min(elapsed / duration, 1);\n";
    ss << "    const easedProgress = " << generateEasingFunction(properties.count("easing") ? properties.at("easing") : "ease") << "(progress);\n\n";
    
    ss << "    // 应用动画属性\n";
    if (properties.count("begin")) {
        ss << "    if (progress === 0) {\n";
        ss << "      " << properties.at("begin") << "\n";
        ss << "    }\n";
    }
    
    ss << "    // 动画执行\n";
    ss << "    " << (properties.count("when") ? properties.at("when") : "// 动画逻辑") << "\n";
    
    if (properties.count("end")) {
        ss << "    if (progress === 1) {\n";
        ss << "      " << properties.at("end") << "\n";
        ss << "    }\n";
    }
    
    ss << "    if (progress < 1) {\n";
    ss << "      requestAnimationFrame(animationFrame);\n";
    ss << "    } else if (loop) {\n";
    ss << "      // 循环动画\n";
    ss << "      setTimeout(() => {\n";
    ss << "        animate();\n";
    ss << "      }, 100);\n";
    ss << "    }\n";
    ss << "  }\n\n";
    
    ss << "  if (delay > 0) {\n";
    ss << "    setTimeout(() => {\n";
    ss << "      requestAnimationFrame(animationFrame);\n";
    ss << "    }, delay);\n";
    ss << "  } else {\n";
    ss << "    requestAnimationFrame(animationFrame);\n";
    ss << "  }\n";
    ss << "}\n";
    
    return ss.str();
}

std::string CHTLJSAnimation::generateEasingFunction(const std::string& easing) const {
    if (!isValidEasing(easing)) {
        m_context->addWarning("Invalid easing function: " + easing);
        return "function(t) { return t; }";
    }
    
    return generateEasingCode(easing);
}

std::string CHTLJSAnimation::parseEasing(const std::string& easing) const {
    return generateEasingFunction(easing);
}

std::string CHTLJSAnimation::generateAnimationControl(const std::string& controlType) const {
    if (controlType == "play") {
        return "animate();";
    } else if (controlType == "pause") {
        return "// 暂停动画逻辑";
    } else if (controlType == "stop") {
        return "// 停止动画逻辑";
    } else if (controlType == "reverse") {
        return "// 反向动画逻辑";
    }
    
    return "// 未知控制类型: " + controlType;
}

std::string CHTLJSAnimation::generateLoopControl(const std::string& loopConfig) const {
    std::stringstream ss;
    
    ss << "const loopConfig = " << loopConfig << ";\n";
    ss << "if (loopConfig) {\n";
    ss << "  // 循环动画配置\n";
    ss << "  const loopCount = loopConfig.count || Infinity;\n";
    ss << "  const loopDelay = loopConfig.delay || 0;\n";
    ss << "  // 实现循环逻辑\n";
    ss << "}\n";
    
    return ss.str();
}

bool CHTLJSAnimation::isValidEasing(const std::string& easing) const {
    static const std::set<std::string> validEasings = {
        "linear", "ease", "ease-in", "ease-out", "ease-in-out",
        "cubic-bezier", "steps", "spring"
    };
    
    return validEasings.find(easing) != validEasings.end();
}

std::string CHTLJSAnimation::generateEasingCode(const std::string& easing) const {
    if (easing == "linear") {
        return "function(t) { return t; }";
    } else if (easing == "ease") {
        return "function(t) { return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t; }";
    } else if (easing == "ease-in") {
        return "function(t) { return t * t; }";
    } else if (easing == "ease-out") {
        return "function(t) { return t * (2 - t); }";
    } else if (easing == "ease-in-out") {
        return "function(t) { return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t; }";
    } else if (easing.starts_with("cubic-bezier")) {
        return "function(t) { return " + easing + "; }";
    } else if (easing.starts_with("steps")) {
        return "function(t) { return " + easing + "; }";
    } else if (easing == "spring") {
        return "function(t) { return 1 - Math.pow(1 - t, 3); }";
    }
    
    return "function(t) { return t; }";
}

std::string CHTLJSAnimation::generateTimingFunction(const std::string& timing) const {
    return generateEasingCode(timing);
}

} // namespace CHTL
/**
 * @file CompilerManager.cpp
 * @brief 编译器管理器实现
 * 
 * 根据 CHTL.md 文档实现
 * 统一管理 CSS 和 JavaScript 编译器
 */

#include "CompilerManager.hpp"

namespace CHTL {

CompilerManager::CompilerManager() 
    : cssCompiler_(std::make_unique<CSSCompiler>())
    , jsCompiler_(std::make_unique<JSCompiler>()) {
}

CompilerManager::~CompilerManager() = default;

bool CompilerManager::compileCSS(const std::string& input, std::string& output) {
    return cssCompiler_->compile(input, output);
}

bool CompilerManager::compileJS(const std::string& input, std::string& output) {
    return jsCompiler_->compile(input, output);
}

bool CompilerManager::validateCSS(const std::string& input) {
    return cssCompiler_->validate(input);
}

bool CompilerManager::validateJS(const std::string& input) {
    return jsCompiler_->validate(input);
}

void CompilerManager::setMinify(bool minify) {
    cssCompiler_->setMinify(minify);
    jsCompiler_->setMinify(minify);
}

void CompilerManager::setOptimize(bool optimize) {
    cssCompiler_->setOptimize(optimize);
    jsCompiler_->setOptimize(optimize);
}

void CompilerManager::addCSSVariable(const std::string& name, const std::string& value) {
    cssCompiler_->addVariable(name, value);
}

void CompilerManager::addJSGlobalVariable(const std::string& name, const std::string& value) {
    jsCompiler_->addGlobalVariable(name, value);
}

void CompilerManager::addJSGlobalFunction(const std::string& name, const std::string& code) {
    jsCompiler_->addGlobalFunction(name, code);
}

void CompilerManager::clearAll() {
    cssCompiler_->clearVariables();
    jsCompiler_->clearGlobals();
}

} // namespace CHTL
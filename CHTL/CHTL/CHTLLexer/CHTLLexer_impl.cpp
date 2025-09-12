#include "CHTLLexer.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace CHTL {

// 初始化关键字映射
void CHTLLexer::initializeKeywords() {
    // CHTL关键字
    keywords["text"] = CHTLTokenType::TEXT;
    keywords["style"] = CHTLTokenType::STYLE;
    keywords["script"] = CHTLTokenType::SCRIPT;
    keywords["template"] = CHTLTokenType::TEMPLATE;
    keywords["custom"] = CHTLTokenType::CUSTOM;
    keywords["import"] = CHTLTokenType::IMPORT;
    keywords["namespace"] = CHTLTokenType::NAMESPACE;
    keywords["constraint"] = CHTLTokenType::CONSTRAINT;
    keywords["configuration"] = CHTLTokenType::CONFIGURATION;
    keywords["use"] = CHTLTokenType::USE;
    
    // 模板关键字
    keywords["@Style"] = CHTLTokenType::TEMPLATE_STYLE;
    keywords["@Element"] = CHTLTokenType::TEMPLATE_ELEMENT;
    keywords["@Var"] = CHTLTokenType::TEMPLATE_VAR;
    keywords["inherit"] = CHTLTokenType::INHERIT;
    keywords["delete"] = CHTLTokenType::DELETE;
    keywords["insert"] = CHTLTokenType::INSERT;
    
    // CHTL JS关键字
    keywords["fileloader"] = CHTLTokenType::FILELOADER;
    keywords["listen"] = CHTLTokenType::LISTEN;
    keywords["delegate"] = CHTLTokenType::DELEGATE;
    keywords["animate"] = CHTLTokenType::ANIMATE;
    keywords["vir"] = CHTLTokenType::VIR;
    keywords["router"] = CHTLTokenType::ROUTER;
}

// 初始化HTML元素
void CHTLLexer::initializeHTMLElements() {
    htmlElements = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "textarea", "select", "option",
        "header", "footer", "nav", "main", "section", "article", "aside",
        "br", "hr", "strong", "em", "code", "pre", "blockquote"
    };
}

CHTLLexer::CHTLLexer() : position(0), line(1), column(1) {
    initializeKeywords();
    initializeHTMLElements();
}

CHTLLexer::CHTLLexer(const std::string& source) : source(source), position(0), line(1), column(1) {
    initializeKeywords();
    initializeHTMLElements();
}

char CHTLLexer::current() const {
    if (position >= source.length()) return '\0';
    return source[position];
}

char CHTLLexer::peek(size_t offset) const {
    if (position + offset >= source.length()) return '\0';
    return source[position + offset];
}

void CHTLLexer::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void CHTLLexer::skipWhitespace() {
    while (position < source.length() && isWhitespace(current())) {
        advance();
    }
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_' || c == '-';
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLLexer::isNewline(char c) const {
    return c == '\n';
}

} // namespace CHTL
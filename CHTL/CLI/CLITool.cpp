#include "CLITool.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <regex>

namespace CHTL {
namespace CLI {

// ColorOutput 实现
std::string ColorOutput::colorize(const std::string& text, Color color) {
    if (!std::getenv("NO_COLOR")) {
        switch (color) {
            case RED: return "\033[31m" + text + "\033[0m";
            case GREEN: return "\033[32m" + text + "\033[0m";
            case YELLOW: return "\033[33m" + text + "\033[0m";
            case BLUE: return "\033[34m" + text + "\033[0m";
            case MAGENTA: return "\033[35m" + text + "\033[0m";
            case CYAN: return "\033[36m" + text + "\033[0m";
            case WHITE: return "\033[37m" + text + "\033[0m";
            case RESET: return "\033[0m";
            default: return text;
        }
    }
    return text;
}

void ColorOutput::printColored(const std::string& text, Color color, std::ostream& os) {
    os << colorize(text, color);
}

void ColorOutput::printError(const std::string& text, std::ostream& os) {
    printColored("错误: " + text, RED, os);
}

void ColorOutput::printSuccess(const std::string& text, std::ostream& os) {
    printColored("成功: " + text, GREEN, os);
}

void ColorOutput::printWarning(const std::string& text, std::ostream& os) {
    printColored("警告: " + text, YELLOW, os);
}

void ColorOutput::printInfo(const std::string& text, std::ostream& os) {
    printColored("信息: " + text, BLUE, os);
}

// ProgressBar 实现
ProgressBar::ProgressBar(int total, int width, const std::string& prefix, const std::string& suffix)
    : total(total), current(0), width(width), prefix(prefix), suffix(suffix), fill('#'), empty('-') {}

void ProgressBar::update(int current) {
    this->current = current;
    updateDisplay();
}

void ProgressBar::setTotal(int total) {
    this->total = total;
}

void ProgressBar::setPrefix(const std::string& prefix) {
    this->prefix = prefix;
}

void ProgressBar::setSuffix(const std::string& suffix) {
    this->suffix = suffix;
}

void ProgressBar::setFill(char fill) {
    this->fill = fill;
}

void ProgressBar::setEmpty(char empty) {
    this->empty = empty;
}

void ProgressBar::display() const {
    updateDisplay();
}

void ProgressBar::finish() {
    current = total;
    updateDisplay();
    std::cout << std::endl;
}

void ProgressBar::updateDisplay() const {
    if (total == 0) return;
    
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(width * progress);
    
    std::cout << "\r" << prefix << "[";
    for (int i = 0; i < width; ++i) {
        if (i < pos) {
            std::cout << fill;
        } else {
            std::cout << empty;
        }
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "% " << suffix;
    std::cout.flush();
}

} // namespace CLI
} // namespace CHTL
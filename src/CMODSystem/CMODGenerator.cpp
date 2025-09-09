#include "CMODSystem/CMODGenerator.h"
#include <fstream>
#include <iostream>

namespace CHTL {

CMODGenerator::CMODGenerator() {
}

CMODGenerator::~CMODGenerator() = default;

bool CMODGenerator::generateModule(const std::string& outputPath) {
    // 生成CMOD模块文件
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        return false;
    }
    
    // 生成模块头部
    file << "[ModuleInfo]" << std::endl;
    file << "name: " << m_moduleName << std::endl;
    file << "version: " << m_moduleVersion << std::endl;
    file << "description: " << m_moduleDescription << std::endl;
    file << "author: " << m_moduleAuthor << std::endl;
    file << "license: " << m_moduleLicense << std::endl;
    file << "[/ModuleInfo]" << std::endl;
    file << std::endl;
    
    // 生成模块内容
    file << "[ModuleContent]" << std::endl;
    file << m_moduleContent << std::endl;
    file << "[/ModuleContent]" << std::endl;
    file << std::endl;
    
    // 生成模块导出
    file << "[ModuleExport]" << std::endl;
    for (const auto& exportItem : m_moduleExports) {
        file << "export: " << exportItem << std::endl;
    }
    file << "[/ModuleExport]" << std::endl;
    
    file.close();
    return true;
}

std::string CMODGenerator::getGeneratedModule() const {
    return m_generatedModule;
}

} // namespace CHTL
#include "CJMODSystem/CJMODGenerator.h"
#include <fstream>
#include <iostream>

namespace CHTL {

CJMODGenerator::CJMODGenerator() {
}

CJMODGenerator::~CJMODGenerator() = default;

bool CJMODGenerator::generateModule(const std::string& outputPath) {
    // 生成CJMOD模块文件
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        return false;
    }
    
    // 生成模块头部
    file << "[CJMODInfo]" << std::endl;
    file << "name: " << m_moduleName << std::endl;
    file << "version: " << m_moduleVersion << std::endl;
    file << "description: " << m_moduleDescription << std::endl;
    file << "author: " << m_moduleAuthor << std::endl;
    file << "license: " << m_moduleLicense << std::endl;
    file << "[/CJMODInfo]" << std::endl;
    file << std::endl;
    
    // 生成模块内容
    file << "[CJMODContent]" << std::endl;
    file << m_moduleContent << std::endl;
    file << "[/CJMODContent]" << std::endl;
    file << std::endl;
    
    // 生成C++ API
    file << "[CppAPI]" << std::endl;
    file << m_cppAPI << std::endl;
    file << "[/CppAPI]" << std::endl;
    file << std::endl;
    
    // 生成模块导出
    file << "[CJMODExport]" << std::endl;
    for (const auto& exportItem : m_moduleExports) {
        file << "export: " << exportItem << std::endl;
    }
    file << "[/CJMODExport]" << std::endl;
    
    file.close();
    return true;
}

} // namespace CHTL
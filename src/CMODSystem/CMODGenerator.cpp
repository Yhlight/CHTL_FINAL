#include "CMODSystem/CMODGenerator.h"

namespace CHTL {

CMODGenerator::CMODGenerator() {
}

CMODGenerator::~CMODGenerator() = default;

bool CMODGenerator::generateModule(const std::string& outputPath) {
    // TODO: 实现CMOD模块生成
    return false;
}

std::string CMODGenerator::getGeneratedModule() const {
    return m_generatedModule;
}

} // namespace CHTL
#include "CHTLIOStream.h"

namespace CHTL {

CHTLIOStream::CHTLIOStream() {
    // Constructor
}

CHTLIOStream::~CHTLIOStream() {
    // Destructor
}

void CHTLIOStream::log(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
}

void CHTLIOStream::error(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

} // namespace CHTL

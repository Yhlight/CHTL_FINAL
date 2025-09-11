#pragma once

#include "../CHTLNode/ProgramNode.h"
#include <string>
#include <vector>
#include <set>
#include <memory>

namespace CHTL {

class CHTLLoader {
public:
    CHTLLoader() = default;

    std::shared_ptr<ProgramNode> Load(const std::string& entry_filepath);

private:
    std::shared_ptr<ProgramNode> parseFile(const std::string& filepath);

    std::set<std::string> m_processed_files;
    std::vector<std::string> m_error_messages;
};

} // namespace CHTL

#pragma once
#include "Token.h"
#include <string>
namespace CHTL {
class Lexer {
public:
    Lexer(const std::string& input); Token next_token();
private:
    void read_char(); Token read_identifier(); Token read_string(); bool is_letter(char ch) const;
    std::string m_input; size_t m_position = 0; size_t m_read_position = 0; char m_char = 0;
};
}

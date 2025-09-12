#include "Lexer.h"
namespace CHTL {
bool Lexer::is_letter(char ch) const { return ('a'<=ch&&ch<='z')||('A'<=ch&&ch<='Z')||ch=='_'; }
Lexer::Lexer(const std::string& input) : m_input(input) { read_char(); }
void Lexer::read_char() {
    if (m_read_position>=m_input.length()){ m_char=0; } else { m_char=m_input[m_read_position]; }
    m_position=m_read_position; m_read_position+=1;
}
Token Lexer::read_identifier() {
    size_t start=m_position;
    while(is_letter(m_char) || (m_position > start && (('0'<=m_char&&m_char<='9')||m_char=='-'))){ read_char(); }
    return {TokenType::IDENTIFIER, m_input.substr(start, m_position-start)};
}
Token Lexer::read_string() {
    char quote = m_char; size_t start=m_position+1; read_char();
    while(m_char!=quote && m_char!=0){ read_char(); }
    std::string literal = m_input.substr(start, m_position-start);
    read_char(); return {TokenType::STRING_LITERAL, literal};
}
Token Lexer::next_token() {
    while(m_char==' '||m_char=='\t'||m_char=='\n'||m_char=='\r'){ read_char(); }
    if ((m_char == '/' && m_input[m_read_position] == '/') || (m_char == '-' && m_input[m_read_position] == '-')) {
        while (m_char != '\n' && m_char != 0) { read_char(); }
        while(m_char==' '||m_char=='\t'||m_char=='\n'||m_char=='\r'){ read_char(); }
    }
    Token tok;
    switch(m_char){
        case '{': tok={TokenType::LEFT_BRACE,"{"}; read_char(); break;
        case '}': tok={TokenType::RIGHT_BRACE,"}"}; read_char(); break;
        case ':': tok={TokenType::COLON,":"}; read_char(); break;
        case '=': tok={TokenType::EQUALS,"="}; read_char(); break;
        case ';': tok={TokenType::SEMICOLON,";"}; read_char(); break;
        case '.': tok={TokenType::DOT,"."}; read_char(); break;
        case '#': tok={TokenType::HASH,"#"}; read_char(); break;
        case '&': tok={TokenType::AMPERSAND,"&"}; read_char(); break;
        case '"': case '\'': return read_string();
        case 0: tok={TokenType::END_OF_FILE,""}; break;
        default:
            if(is_letter(m_char)){ return read_identifier(); }
            else { tok={TokenType::ILLEGAL, std::string(1,m_char)}; read_char(); }
            break;
    }
    return tok;
}
}

#ifndef CHTL_IO_STREAM_H
#define CHTL_IO_STREAM_H

#include <iostream>
#include <string>

namespace CHTL {

// A wrapper or custom implementation for I/O operations within the compiler
class CHTLIOStream {
public:
    CHTLIOStream();
    ~CHTLIOStream();

    void log(const std::string& message);
    void error(const std::string& message);
};

} // namespace CHTL

#endif // CHTL_IO_STREAM_H

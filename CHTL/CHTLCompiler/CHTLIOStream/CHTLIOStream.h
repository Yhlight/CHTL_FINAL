#ifndef CHTL_IO_STREAM_H
#define CHTL_IO_STREAM_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

// Stream types
enum class StreamType {
    File,
    Memory,
    String,
    Network,
    Pipe
};

// Stream mode
enum class StreamMode {
    Read,
    Write,
    Append,
    ReadWrite
};

// Stream status
enum class StreamStatus {
    Open,
    Closed,
    Error,
    EndOfFile
};

// Stream configuration
struct StreamConfig {
    StreamType type;
    StreamMode mode;
    bool binary;
    bool buffered;
    size_t buffer_size;
    std::string encoding;
    
    StreamConfig() : type(StreamType::File), mode(StreamMode::Read), 
                    binary(false), buffered(true), buffer_size(4096), encoding("utf-8") {}
};

// Stream result
struct StreamResult {
    bool success;
    std::string data;
    std::string error_message;
    size_t bytes_read;
    size_t bytes_written;
    
    StreamResult() : success(false), bytes_read(0), bytes_written(0) {}
};

// Base stream interface
class IStream {
public:
    virtual ~IStream() = default;
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool isOpen() const = 0;
    virtual StreamStatus getStatus() const = 0;
    virtual std::string read(size_t size = 0) = 0;
    virtual bool write(const std::string& data) = 0;
    virtual bool seek(size_t position) = 0;
    virtual size_t tell() const = 0;
    virtual size_t size() const = 0;
    virtual bool eof() const = 0;
    virtual void flush() = 0;
    virtual std::string getError() const = 0;
};

// File stream
class FileStream : public IStream {
private:
    std::string file_path;
    StreamMode mode;
    std::unique_ptr<std::fstream> file;
    StreamStatus status;
    std::string error_message;
    
public:
    FileStream(const std::string& path, StreamMode mode = StreamMode::Read);
    ~FileStream();
    
    bool open() override;
    bool close() override;
    bool isOpen() const override;
    StreamStatus getStatus() const override;
    std::string read(size_t size = 0) override;
    bool write(const std::string& data) override;
    bool seek(size_t position) override;
    size_t tell() const override;
    size_t size() const override;
    bool eof() const override;
    void flush() override;
    std::string getError() const override;
    
    const std::string& getFilePath() const;
    void setFilePath(const std::string& path);
};

// Memory stream
class MemoryStream : public IStream {
private:
    std::string data;
    size_t position;
    StreamMode mode;
    StreamStatus status;
    std::string error_message;
    
public:
    MemoryStream(const std::string& initial_data = "", StreamMode mode = StreamMode::ReadWrite);
    ~MemoryStream();
    
    bool open() override;
    bool close() override;
    bool isOpen() const override;
    StreamStatus getStatus() const override;
    std::string read(size_t size = 0) override;
    bool write(const std::string& data) override;
    bool seek(size_t position) override;
    size_t tell() const override;
    size_t size() const override;
    bool eof() const override;
    void flush() override;
    std::string getError() const override;
    
    const std::string& getData() const;
    void setData(const std::string& data);
    void clear();
};

// String stream
class StringStream : public IStream {
private:
    std::stringstream stream;
    StreamMode mode;
    StreamStatus status;
    std::string error_message;
    
public:
    StringStream(const std::string& initial_data = "", StreamMode mode = StreamMode::ReadWrite);
    ~StringStream();
    
    bool open() override;
    bool close() override;
    bool isOpen() const override;
    StreamStatus getStatus() const override;
    std::string read(size_t size = 0) override;
    bool write(const std::string& data) override;
    bool seek(size_t position) override;
    size_t tell() const override;
    size_t size() const override;
    bool eof() const override;
    void flush() override;
    std::string getError() const override;
    
    std::string str() const;
    void str(const std::string& data);
};

// Stream manager
class CHTLIOStream {
private:
    std::vector<std::unique_ptr<IStream>> streams;
    bool debug_mode;
    bool verbose;
    
public:
    CHTLIOStream();
    ~CHTLIOStream();
    
    // Stream management
    std::unique_ptr<IStream> createStream(StreamType type, const std::string& source, StreamMode mode = StreamMode::Read);
    std::unique_ptr<IStream> createFileStream(const std::string& file_path, StreamMode mode = StreamMode::Read);
    std::unique_ptr<IStream> createMemoryStream(const std::string& initial_data = "", StreamMode mode = StreamMode::ReadWrite);
    std::unique_ptr<IStream> createStringStream(const std::string& initial_data = "", StreamMode mode = StreamMode::ReadWrite);
    
    // File operations
    StreamResult readFile(const std::string& file_path);
    bool writeFile(const std::string& file_path, const std::string& data);
    bool appendFile(const std::string& file_path, const std::string& data);
    bool copyFile(const std::string& source, const std::string& destination);
    bool moveFile(const std::string& source, const std::string& destination);
    bool deleteFile(const std::string& file_path);
    bool fileExists(const std::string& file_path);
    size_t getFileSize(const std::string& file_path);
    
    // Directory operations
    bool createDirectory(const std::string& path);
    bool deleteDirectory(const std::string& path);
    bool directoryExists(const std::string& path);
    std::vector<std::string> listFiles(const std::string& directory);
    std::vector<std::string> listDirectories(const std::string& directory);
    std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern);
    
    // Stream operations
    StreamResult readStream(IStream& stream, size_t size = 0);
    bool writeStream(IStream& stream, const std::string& data);
    bool copyStream(IStream& source, IStream& destination);
    std::string readAll(IStream& stream);
    bool writeAll(IStream& stream, const std::string& data);
    
    // Utility
    std::string getFileExtension(const std::string& file_path) const;
    std::string getFileName(const std::string& file_path) const;
    std::string getDirectory(const std::string& file_path) const;
    std::string getAbsolutePath(const std::string& path) const;
    std::string getRelativePath(const std::string& path, const std::string& base) const;
    bool isAbsolutePath(const std::string& path) const;
    std::string normalizePath(const std::string& path) const;
    
    // Configuration
    void setDebugMode(bool enabled);
    void setVerboseMode(bool enabled);
    
    // Error handling
    std::string getLastError() const;
    void clearError();
    
private:
    std::string last_error;
    
    void setError(const std::string& error);
    std::string getStreamModeString(StreamMode mode) const;
    std::ios::openmode getOpenMode(StreamMode mode) const;
};

} // namespace CHTL

#endif // CHTL_IO_STREAM_H
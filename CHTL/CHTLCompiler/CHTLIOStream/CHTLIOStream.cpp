#include "CHTLIOStream.h"

#include <filesystem>
#include <algorithm>
#include <regex>

namespace CHTL {

// FileStream implementation
FileStream::FileStream(const std::string& path, StreamMode mode) 
    : file_path(path), mode(mode), status(StreamStatus::Closed) {
    file = std::make_unique<std::fstream>();
}

FileStream::~FileStream() {
    close();
}

bool FileStream::open() {
    if (isOpen()) {
        return true;
    }
    
    std::ios::openmode open_mode = std::ios::in;
    switch (mode) {
        case StreamMode::Read:
            open_mode = std::ios::in;
            break;
        case StreamMode::Write:
            open_mode = std::ios::out;
            break;
        case StreamMode::Append:
            open_mode = std::ios::out | std::ios::app;
            break;
        case StreamMode::ReadWrite:
            open_mode = std::ios::in | std::ios::out;
            break;
    }
    file->open(file_path, open_mode);
    
    if (file->is_open()) {
        status = StreamStatus::Open;
        return true;
    } else {
        status = StreamStatus::Error;
        error_message = "Failed to open file: " + file_path;
        return false;
    }
}

bool FileStream::close() {
    if (!isOpen()) {
        return true;
    }
    
    file->close();
    status = StreamStatus::Closed;
    return true;
}

bool FileStream::isOpen() const {
    return file && file->is_open();
}

StreamStatus FileStream::getStatus() const {
    return status;
}

std::string FileStream::read(size_t size) {
    if (!isOpen()) {
        error_message = "Stream is not open";
        return "";
    }
    
    if (mode == StreamMode::Write || mode == StreamMode::Append) {
        error_message = "Stream is not open for reading";
        return "";
    }
    
    if (size == 0) {
        // Read all
        std::stringstream buffer;
        buffer << file->rdbuf();
        return buffer.str();
    } else {
        // Read specific size
        std::string result(size, '\0');
        file->read(&result[0], size);
        result.resize(file->gcount());
        return result;
    }
}

bool FileStream::write(const std::string& data) {
    if (!isOpen()) {
        error_message = "Stream is not open";
        return false;
    }
    
    if (mode == StreamMode::Read) {
        error_message = "Stream is not open for writing";
        return false;
    }
    
    file->write(data.c_str(), data.size());
    if (file->fail()) {
        error_message = "Failed to write to file";
        return false;
    }
    
    return true;
}

bool FileStream::seek(size_t position) {
    if (!isOpen()) {
        error_message = "Stream is not open";
        return false;
    }
    
    file->seekg(position);
    return !file->fail();
}

size_t FileStream::tell() const {
    if (!isOpen()) {
        return 0;
    }
    
    return file->tellg();
}

size_t FileStream::size() const {
    if (!isOpen()) {
        return 0;
    }
    
    size_t current_pos = file->tellg();
    file->seekg(0, std::ios::end);
    size_t size = file->tellg();
    file->seekg(current_pos);
    return size;
}

bool FileStream::eof() const {
    return file && file->eof();
}

void FileStream::flush() {
    if (isOpen()) {
        file->flush();
    }
}

std::string FileStream::getError() const {
    return error_message;
}

const std::string& FileStream::getFilePath() const {
    return file_path;
}

void FileStream::setFilePath(const std::string& path) {
    file_path = path;
}

// MemoryStream implementation
MemoryStream::MemoryStream(const std::string& initial_data, StreamMode mode) 
    : data(initial_data), position(0), mode(mode), status(StreamStatus::Closed) {
}

MemoryStream::~MemoryStream() {
    close();
}

bool MemoryStream::open() {
    status = StreamStatus::Open;
    return true;
}

bool MemoryStream::close() {
    status = StreamStatus::Closed;
    return true;
}

bool MemoryStream::isOpen() const {
    return status == StreamStatus::Open;
}

StreamStatus MemoryStream::getStatus() const {
    return status;
}

std::string MemoryStream::read(size_t size) {
    if (!isOpen()) {
        error_message = "Stream is not open";
        return "";
    }
    
    if (mode == StreamMode::Write || mode == StreamMode::Append) {
        error_message = "Stream is not open for reading";
        return "";
    }
    
    if (position >= data.size()) {
        status = StreamStatus::EndOfFile;
        return "";
    }
    
    size_t read_size = (size == 0) ? data.size() - position : std::min(size, data.size() - position);
    std::string result = data.substr(position, read_size);
    position += read_size;
    
    if (position >= data.size()) {
        status = StreamStatus::EndOfFile;
    }
    
    return result;
}

bool MemoryStream::write(const std::string& data) {
    if (!isOpen()) {
        error_message = "Stream is not open";
        return false;
    }
    
    if (mode == StreamMode::Read) {
        error_message = "Stream is not open for writing";
        return false;
    }
    
    if (mode == StreamMode::Append) {
        this->data += data;
    } else {
        if (position + data.size() > this->data.size()) {
            this->data.resize(position + data.size());
        }
        this->data.replace(position, data.size(), data);
        position += data.size();
    }
    
    return true;
}

bool MemoryStream::seek(size_t position) {
    if (!isOpen()) {
        error_message = "Stream is not open";
        return false;
    }
    
    if (position > data.size()) {
        error_message = "Position out of bounds";
        return false;
    }
    
    this->position = position;
    status = StreamStatus::Open;
    return true;
}

size_t MemoryStream::tell() const {
    return position;
}

size_t MemoryStream::size() const {
    return data.size();
}

bool MemoryStream::eof() const {
    return position >= data.size();
}

void MemoryStream::flush() {
    // Memory stream doesn't need flushing
}

std::string MemoryStream::getError() const {
    return error_message;
}

const std::string& MemoryStream::getData() const {
    return data;
}

void MemoryStream::setData(const std::string& data) {
    this->data = data;
    position = 0;
}

void MemoryStream::clear() {
    data.clear();
    position = 0;
}

// StringStream implementation
StringStream::StringStream(const std::string& initial_data, StreamMode mode) 
    : mode(mode), status(StreamStatus::Closed) {
    stream.str(initial_data);
}

StringStream::~StringStream() {
    close();
}

bool StringStream::open() {
    status = StreamStatus::Open;
    return true;
}

bool StringStream::close() {
    status = StreamStatus::Closed;
    return true;
}

bool StringStream::isOpen() const {
    return status == StreamStatus::Open;
}

StreamStatus StringStream::getStatus() const {
    return status;
}

std::string StringStream::read(size_t size) {
    if (!isOpen()) {
        error_message = "Stream is not open";
        return "";
    }
    
    if (mode == StreamMode::Write || mode == StreamMode::Append) {
        error_message = "Stream is not open for reading";
        return "";
    }
    
    if (size == 0) {
        // Read all
        std::string result = stream.str();
        stream.str("");
        return result;
    } else {
        // Read specific size
        std::string result(size, '\0');
        stream.read(&result[0], size);
        result.resize(stream.gcount());
        return result;
    }
}

bool StringStream::write(const std::string& data) {
    if (!isOpen()) {
        error_message = "Stream is not open";
        return false;
    }
    
    if (mode == StreamMode::Read) {
        error_message = "Stream is not open for writing";
        return false;
    }
    
    stream << data;
    return !stream.fail();
}

bool StringStream::seek(size_t position) {
    if (!isOpen()) {
        error_message = "Stream is not open";
        return false;
    }
    
    stream.seekg(position);
    return !stream.fail();
}

size_t StringStream::tell() const {
    return const_cast<StringStream*>(this)->stream.tellg();
}

size_t StringStream::size() const {
    auto* non_const_this = const_cast<StringStream*>(this);
    size_t current_pos = non_const_this->stream.tellg();
    non_const_this->stream.seekg(0, std::ios::end);
    size_t size = non_const_this->stream.tellg();
    non_const_this->stream.seekg(current_pos);
    return size;
}

bool StringStream::eof() const {
    return stream.eof();
}

void StringStream::flush() {
    stream.flush();
}

std::string StringStream::getError() const {
    return error_message;
}

std::string StringStream::str() const {
    return stream.str();
}

void StringStream::str(const std::string& data) {
    stream.str(data);
}

// CHTLIOStream implementation
CHTLIOStream::CHTLIOStream() : debug_mode(false), verbose(false) {
}

CHTLIOStream::~CHTLIOStream() {
    // Clean up streams
    streams.clear();
}

std::unique_ptr<IStream> CHTLIOStream::createStream(StreamType type, const std::string& source, StreamMode mode) {
    switch (type) {
        case StreamType::File:
            return createFileStream(source, mode);
        case StreamType::Memory:
            return createMemoryStream(source, mode);
        case StreamType::String:
            return createStringStream(source, mode);
        default:
            return nullptr;
    }
}

std::unique_ptr<IStream> CHTLIOStream::createFileStream(const std::string& file_path, StreamMode mode) {
    return std::make_unique<FileStream>(file_path, mode);
}

std::unique_ptr<IStream> CHTLIOStream::createMemoryStream(const std::string& initial_data, StreamMode mode) {
    return std::make_unique<MemoryStream>(initial_data, mode);
}

std::unique_ptr<IStream> CHTLIOStream::createStringStream(const std::string& initial_data, StreamMode mode) {
    return std::make_unique<StringStream>(initial_data, mode);
}

StreamResult CHTLIOStream::readFile(const std::string& file_path) {
    StreamResult result;
    
    try {
        auto stream = createFileStream(file_path, StreamMode::Read);
        if (!stream || !stream->open()) {
            result.success = false;
            result.error_message = "Failed to open file: " + file_path;
            return result;
        }
        
        result.data = stream->read();
        result.bytes_read = result.data.size();
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Error reading file: " + std::string(e.what());
    }
    
    return result;
}

bool CHTLIOStream::writeFile(const std::string& file_path, const std::string& data) {
    try {
        auto stream = createFileStream(file_path, StreamMode::Write);
        if (!stream || !stream->open()) {
            setError("Failed to open file for writing: " + file_path);
            return false;
        }
        
        bool success = stream->write(data);
        if (success) {
            stream->flush();
        } else {
            setError("Failed to write to file: " + file_path);
        }
        
        return success;
        
    } catch (const std::exception& e) {
        setError("Error writing file: " + std::string(e.what()));
        return false;
    }
}

bool CHTLIOStream::appendFile(const std::string& file_path, const std::string& data) {
    try {
        auto stream = createFileStream(file_path, StreamMode::Append);
        if (!stream || !stream->open()) {
            setError("Failed to open file for appending: " + file_path);
            return false;
        }
        
        bool success = stream->write(data);
        if (success) {
            stream->flush();
        } else {
            setError("Failed to append to file: " + file_path);
        }
        
        return success;
        
    } catch (const std::exception& e) {
        setError("Error appending to file: " + std::string(e.what()));
        return false;
    }
}

bool CHTLIOStream::copyFile(const std::string& source, const std::string& destination) {
    try {
        auto source_stream = createFileStream(source, StreamMode::Read);
        auto dest_stream = createFileStream(destination, StreamMode::Write);
        
        if (!source_stream || !source_stream->open()) {
            setError("Failed to open source file: " + source);
            return false;
        }
        
        if (!dest_stream || !dest_stream->open()) {
            setError("Failed to open destination file: " + destination);
            return false;
        }
        
        return copyStream(*source_stream, *dest_stream);
        
    } catch (const std::exception& e) {
        setError("Error copying file: " + std::string(e.what()));
        return false;
    }
}

bool CHTLIOStream::moveFile(const std::string& source, const std::string& destination) {
    if (!copyFile(source, destination)) {
        return false;
    }
    
    return deleteFile(source);
}

bool CHTLIOStream::deleteFile(const std::string& file_path) {
    try {
        return std::filesystem::remove(file_path);
    } catch (const std::exception& e) {
        setError("Error deleting file: " + std::string(e.what()));
        return false;
    }
}

bool CHTLIOStream::fileExists(const std::string& file_path) {
    try {
        return std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path);
    } catch (const std::exception& e) {
        setError("Error checking file existence: " + std::string(e.what()));
        return false;
    }
}

size_t CHTLIOStream::getFileSize(const std::string& file_path) {
    try {
        if (fileExists(file_path)) {
            return std::filesystem::file_size(file_path);
        }
        return 0;
    } catch (const std::exception& e) {
        setError("Error getting file size: " + std::string(e.what()));
        return 0;
    }
}

bool CHTLIOStream::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception& e) {
        setError("Error creating directory: " + std::string(e.what()));
        return false;
    }
}

bool CHTLIOStream::deleteDirectory(const std::string& path) {
    try {
        return std::filesystem::remove_all(path) > 0;
    } catch (const std::exception& e) {
        setError("Error deleting directory: " + std::string(e.what()));
        return false;
    }
}

bool CHTLIOStream::directoryExists(const std::string& path) {
    try {
        return std::filesystem::exists(path) && std::filesystem::is_directory(path);
    } catch (const std::exception& e) {
        setError("Error checking directory existence: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> CHTLIOStream::listFiles(const std::string& directory) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        setError("Error listing files: " + std::string(e.what()));
    }
    
    return files;
}

std::vector<std::string> CHTLIOStream::listDirectories(const std::string& directory) {
    std::vector<std::string> directories;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        setError("Error listing directories: " + std::string(e.what()));
    }
    
    return directories;
}

std::vector<std::string> CHTLIOStream::findFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> files;
    
    try {
        std::regex regex_pattern(pattern);
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (std::regex_match(filename, regex_pattern)) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        setError("Error finding files: " + std::string(e.what()));
    }
    
    return files;
}

StreamResult CHTLIOStream::readStream(IStream& stream, size_t size) {
    StreamResult result;
    
    try {
        result.data = stream.read(size);
        result.bytes_read = result.data.size();
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Error reading stream: " + std::string(e.what());
    }
    
    return result;
}

bool CHTLIOStream::writeStream(IStream& stream, const std::string& data) {
    try {
        return stream.write(data);
    } catch (const std::exception& e) {
        setError("Error writing to stream: " + std::string(e.what()));
        return false;
    }
}

bool CHTLIOStream::copyStream(IStream& source, IStream& destination) {
    try {
        std::string data = source.read();
        return destination.write(data);
    } catch (const std::exception& e) {
        setError("Error copying stream: " + std::string(e.what()));
        return false;
    }
}

std::string CHTLIOStream::readAll(IStream& stream) {
    return stream.read();
}

bool CHTLIOStream::writeAll(IStream& stream, const std::string& data) {
    return stream.write(data);
}

std::string CHTLIOStream::getFileExtension(const std::string& file_path) const {
    size_t pos = file_path.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return file_path.substr(pos);
}

std::string CHTLIOStream::getFileName(const std::string& file_path) const {
    size_t pos = file_path.find_last_of('/');
    if (pos == std::string::npos) {
        return file_path;
    }
    return file_path.substr(pos + 1);
}

std::string CHTLIOStream::getDirectory(const std::string& file_path) const {
    size_t pos = file_path.find_last_of('/');
    if (pos == std::string::npos) {
        return "";
    }
    return file_path.substr(0, pos);
}

std::string CHTLIOStream::getAbsolutePath(const std::string& path) const {
    try {
        return std::filesystem::absolute(path).string();
    } catch (const std::exception& e) {
        const_cast<CHTLIOStream*>(this)->setError("Error getting absolute path: " + std::string(e.what()));
        return path;
    }
}

std::string CHTLIOStream::getRelativePath(const std::string& path, const std::string& base) const {
    try {
        return std::filesystem::relative(path, base).string();
    } catch (const std::exception& e) {
        const_cast<CHTLIOStream*>(this)->setError("Error getting relative path: " + std::string(e.what()));
        return path;
    }
}

bool CHTLIOStream::isAbsolutePath(const std::string& path) const {
    return std::filesystem::path(path).is_absolute();
}

std::string CHTLIOStream::normalizePath(const std::string& path) const {
    try {
        return std::filesystem::canonical(path).string();
    } catch (const std::exception& e) {
        const_cast<CHTLIOStream*>(this)->setError("Error normalizing path: " + std::string(e.what()));
        return path;
    }
}

void CHTLIOStream::setDebugMode(bool enabled) {
    debug_mode = enabled;
}

void CHTLIOStream::setVerboseMode(bool enabled) {
    verbose = enabled;
}

std::string CHTLIOStream::getLastError() const {
    return last_error;
}

void CHTLIOStream::clearError() {
    last_error.clear();
}

// Private methods
void CHTLIOStream::setError(const std::string& error) {
    last_error = error;
    if (debug_mode) {
        std::cerr << "CHTLIOStream Error: " << error << std::endl;
    }
}

std::string CHTLIOStream::getStreamModeString(StreamMode mode) const {
    switch (mode) {
        case StreamMode::Read: return "read";
        case StreamMode::Write: return "write";
        case StreamMode::Append: return "append";
        case StreamMode::ReadWrite: return "readwrite";
        default: return "unknown";
    }
}

std::ios::openmode CHTLIOStream::getOpenMode(StreamMode mode) const {
    switch (mode) {
        case StreamMode::Read:
            return std::ios::in;
        case StreamMode::Write:
            return std::ios::out;
        case StreamMode::Append:
            return std::ios::out | std::ios::app;
        case StreamMode::ReadWrite:
            return std::ios::in | std::ios::out;
        default:
            return std::ios::in;
    }
}

} // namespace CHTL
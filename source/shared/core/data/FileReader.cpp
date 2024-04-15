#include "core/data/FileReader.hpp"
#include "core/data/IFileReader.hpp"

#include <fstream>
#include <sstream>

namespace Soldank
{
std::expected<std::string, FileReaderError> FileReader::Read(const std::string& file_path,
                                                             std::ios_base::openmode mode) const
{
    std::ifstream file_to_read(file_path, mode);
    if (!file_to_read.is_open()) {
        return std::unexpected(FileReaderError::FileNotFound);
    }

    std::stringstream buffer;
    buffer << file_to_read.rdbuf();

    if (buffer.bad()) {
        return std::unexpected(FileReaderError::BufferError);
    }

    return buffer.str();
}
} // namespace Soldank

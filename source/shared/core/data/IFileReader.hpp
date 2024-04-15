#ifndef __DATA_READER_HPP__
#define __DATA_READER_HPP__

#include <expected>
#include <string>
#include <ios>

namespace Soldank
{
enum class FileReaderError
{
    FileNotFound = 0,
    BufferError
};

class IFileReader
{
public:
    virtual ~IFileReader() = default;
    virtual std::expected<std::string, FileReaderError> Read(
      const std::string& file_path,
      std::ios_base::openmode mode = std::ios_base::in) const = 0;
};
} // namespace Soldank

#endif

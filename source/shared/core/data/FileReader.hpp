#ifndef __FILE_READER_HPP__
#define __FILE_READER_HPP__

#include "core/data/IFileReader.hpp"

namespace Soldank
{
class FileReader : public IFileReader
{
public:
    std::expected<std::string, FileReaderError> Read(
      const std::string& file_path,
      std::ios_base::openmode mode = std::ios_base::in) const override;
};
} // namespace Soldank

#endif

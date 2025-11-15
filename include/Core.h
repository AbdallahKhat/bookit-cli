#ifndef BOOKITCORE_H
#define BOOKITCORE_H

#include "Books.h"
#include "CommandParser.h"
#include <filesystem>

namespace Bookit::Core
{
void initializeWorkspace(const std::filesystem::path& wsDir);

void addBook(const std::filesystem::path& wsDir, const std::filesystem::path& filePath,
             const CommandParser::Options& options);

void saveBookToMetadata(const std::filesystem::path& wsDir, const Book& book);

} // namespace Bookit::Core

#endif // BOOKITCORE_H

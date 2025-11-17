#ifndef BOOKITCORE_H
#define BOOKITCORE_H

#include "Books.h"
#include "CommandParser.h"
#include <filesystem>
#include <string_view>

namespace Bookit::Core
{
void initializeWorkspace(const std::filesystem::path& wsDir);

void addBook(const std::filesystem::path& wsDir, const std::filesystem::path& filePath,
             const CommandParser::Options& options);

void removeBook(const std::filesystem::path& wsDir, std::string_view bookFileName);

void listBooks(const std::filesystem::path& wsDir);

void openBook(const std::filesystem::path& wsDir, std::string_view bookFileName);

void saveBookToMetadata(const std::filesystem::path& wsDir, const Book& book);

} // namespace Bookit::Core

#endif // BOOKITCORE_H

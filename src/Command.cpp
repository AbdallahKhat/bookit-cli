#include "Command.h"
#include "Core.h"
#include <filesystem>

namespace fs = std::filesystem;

// Class Command
// Shared validation helpers
bool Command::validateDirectory(const fs::path& path) const
{
    return fs::exists(path) && fs::is_directory(path);
}
bool Command::validateFile(const fs::path& path) const
{
    return fs::exists(path) && fs::is_regular_file(path);
}

// Class InitCommand
bool InitCommand::validate() const
{
    return validateDirectory(m_path) && fs::is_empty(m_path);
}

void InitCommand::execute() const
{
    Bookit::Core::initializeWorkspace(m_path);
}

// Class AddBookCommand
bool AddBookCommand::validate() const
{
    return validateDirectory(m_wsDir) && validateFile(m_filePath);
}

void AddBookCommand::execute() const
{
    Bookit::Core::addBook(m_wsDir, m_filePath, m_options);
}

// Class RemoveBookCommand
bool RemoveBookCommand::validate() const
{
    return validateDirectory(m_wsDir) && !m_bookFileName.empty();
}

void RemoveBookCommand::execute() const
{
    Bookit::Core::removeBook(m_wsDir, m_bookFileName);
}

// Class ListBooksCommand
bool ListBooksCommand::validate() const
{
    return validateDirectory(m_wsDir);
}

void ListBooksCommand::execute() const
{
    Bookit::Core::listBooks(m_wsDir);
}

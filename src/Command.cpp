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
    // Check for an empty directory
    if (validateDirectory(m_path))
    {
        if (fs::is_empty(m_path)) return true;
    }

    return false;
}

void InitCommand::execute() const
{
    Bookit::Core::initializeWorkspace(m_path);
}

// Class AddBookCommand
bool AddBookCommand::validate() const
{
    // Validate that the provided file is a real file
    return validateFile(m_filePath);
}

void AddBookCommand::execute() const
{
    // Call the addBook function from Bookit::Core
    Bookit::Core::addBook(m_wsDir, m_filePath, m_options);
}

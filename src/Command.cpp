#include "Command.h"
#include "BookitCore.h"
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
    BookitCore::initializeWorkspace(m_path);
}

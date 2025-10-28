#include "LibraryManager.h"
#include "CommandInfo.h"
#include "CommandParser.h"
#include <filesystem>

namespace fs = std::filesystem;

// static helper function prototypes
static bool verifyDirectoryReady(const fs::path& path);

/**
 *
 * Public Interface Function implementations
 *
 */

bool LibraryManager::ExecuteCmd()
{
    switch (m_parsedCmd.commandType())
    {
    case CommandInfo::Init:
        InitWorspaceEnv();
        break;

    // case CommandInfo::AddBook:
    default:
        break;
    }

    return true;
}

/**
 *
 * Private facade command function implementations
 *
 */

// Initialize a new Workspace Enironment for library use
bool LibraryManager::InitWorspaceEnv()
{
    const auto& path = m_parsedCmd.path();

    if (!verifyDirectoryReady(path)) { return false; }

    // TODO: Execute Concrete Command (CommandInit)

    return true;
}

/**
 *
 * Static helper functions for the command functions
 *
 */

/* Helpers for path verifications */

// checks if path refers to real directory
static bool verifyDirectoryReady(const fs::path& path)
{
    if (!fs::is_directory(path))
    {
        // TODO: Error: does not refer to a real directory
        return false;
    }

    if (!fs::is_empty(path))
    {
        // TODO: Error: directory must be empty to initialize
        return false;
    }

    return true;
}

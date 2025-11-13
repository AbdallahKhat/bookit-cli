#include "LibraryManager.h"
#include "CommandInfo.h"
#include "CommandParser.h"
#include "Workspaces.h"
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

/**
 *
 * Public Interface Function implementations
 *
 */

bool LibraryManager::executeCmd()
{
    switch (m_parsedCmd.commandType())
    {
    case CommandInfo::Init:
        initWorspaceEnv();
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
bool LibraryManager::initWorspaceEnv()
{

    Bookit::Workspaces workspace;

    auto command = createCommand();
    if (!command) { return false; }

    if (!command->validate()) { return false; }

    command->execute();

    // save workspace to workspace.json
    workspace.addWorkspace(m_parsedCmd.path());

    return true;
}

std::unique_ptr<Command> LibraryManager::createCommand()
{
    switch (m_parsedCmd.commandType())
    {
    case CommandInfo::Init:
        return std::make_unique<InitCommand>(m_parsedCmd.path());

    // case CommandInfo::AddBook:
    default:
        return {};
    }
}

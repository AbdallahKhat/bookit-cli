#include "LibraryManager.h"
#include "CommandInfo.h"
#include "CommandParser.h"
#include "Workspaces.h"
#include <filesystem>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

/**
 *
 * Public Interface Function implementations
 *
 */

bool LibraryManager::executeCmd()
{
    Bookit::Workspaces workspace;

    auto command = createCommand(workspace);
    if (!command) { return false; }

    if (!command->validate()) { return false; }

    command->execute();

    // For Init command, add workspace to persistence
    if (m_parsedCmd.commandType() == CommandInfo::Init)
    {
        workspace.addWorkspace(m_parsedCmd.path());
    }

    return true;
}

/**
 *
 * Private function implementations
 *
 */

std::unique_ptr<Command> LibraryManager::createCommand(const Bookit::Workspaces& workspace)
{
    switch (m_parsedCmd.commandType())
    {
    case CommandInfo::Init:
        return std::make_unique<InitCommand>(m_parsedCmd.path());

    case CommandInfo::AddBook:
    {
        const auto& currentWs = workspace.getCurrentWorkspace();
        if (currentWs.empty())
        {
            std::cerr << "Error: No active workspace. Please initialize a workspace first with "
                         "'bookit init <dir>'\n";
            return {};
        }
        return std::make_unique<AddBookCommand>(m_parsedCmd.path(), m_parsedCmd.options(),
                                                currentWs);
    }

    default:
        return {};
    }
}

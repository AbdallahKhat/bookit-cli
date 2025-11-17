#include "LibraryManager.h"
#include "CommandInfo.h"
#include "CommandParser.h"
#include "Workspaces.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>

namespace fs = std::filesystem;

/**
 *
 * Public Interface Function implementations
 *
 */

bool LibraryManager::executeCmd()
{
    Bookit::Workspaces workspace;

    if (handleWorkspaceCommand(workspace)) { return true; }

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

std::optional<fs::path>
LibraryManager::getActiveWorkspace(const Bookit::Workspaces& workspace) const
{
    const auto& currentWs = workspace.getCurrentWorkspace();
    if (currentWs.empty())
    {
        std::cerr << "Error: No active workspace. Please initialize a workspace first with "
                     "'bookit init <dir>'\n";
        return std::nullopt;
    }

    return currentWs;
}

std::unique_ptr<Command> LibraryManager::createCommand(const Bookit::Workspaces& workspace)
{
    const auto type = m_parsedCmd.commandType();

    std::optional<fs::path> currentWs{};
    if (type != CommandInfo::Init)
    {
        currentWs = getActiveWorkspace(workspace);
        if (!currentWs) { return {}; }
    }

    switch (type)
    {
    case CommandInfo::Init:
        return std::make_unique<InitCommand>(m_parsedCmd.path());
    case CommandInfo::AddBook:
        return std::make_unique<AddBookCommand>(m_parsedCmd.path(), m_parsedCmd.options(),
                                                *currentWs);
    case CommandInfo::RemoveBook:
    {
        const auto bookFileName = m_parsedCmd.path().filename().string();
        return std::make_unique<RemoveBookCommand>(bookFileName, *currentWs);
    }
    case CommandInfo::ListBooks:
        return std::make_unique<ListBooksCommand>(*currentWs);
    case CommandInfo::OpenBook:
    {
        const auto bookFileName = m_parsedCmd.path().filename().string();
        return std::make_unique<OpenBookCommand>(bookFileName, *currentWs);
    }

    default:
        return {};
    }
}

bool LibraryManager::handleWorkspaceCommand(Bookit::Workspaces& workspace) const
{
    switch (m_parsedCmd.commandType())
    {
    case CommandInfo::ListWs:
        workspace.listWorkspaces();
        return true;
    case CommandInfo::SwitchWs:
        workspace.switchWorkspace(m_parsedCmd.path());
        return true;
    default:
        return false;
    }
}

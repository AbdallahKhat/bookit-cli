#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "Command.h"
#include "CommandParser.h"
#include "Workspaces.h"

// LibraryManager is the coordinator class for all features of the library.
// It serves as the main interface for library operations.
class LibraryManager
{
public:
    // Constructors
    LibraryManager(const CommandParser& parsedCmd) noexcept : m_parsedCmd{parsedCmd} {}

    // Public Interfaces
    bool executeCmd();

private:
    // Private functions
    std::unique_ptr<Command> createCommand(const Bookit::Workspaces& workspace);

    // Member variables
    const CommandParser& m_parsedCmd;
};

#endif // LIBRARYMANAGER_H

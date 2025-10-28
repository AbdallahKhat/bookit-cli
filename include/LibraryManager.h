#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "CommandParser.h"

// LibraryManager is the coordinator class for all features of the library.
// It serves as the main interface for library operations.
class LibraryManager
{
public:
    // Constructors
    LibraryManager(const CommandParser& parsedCmd) noexcept : m_parsedCmd{parsedCmd} {}

    // Public Interfaces
    bool ExecuteCmd();

private:
    // Private facade command functions
    bool InitWorspaceEnv();

    // Member variables
    const CommandParser& m_parsedCmd;
};

#endif // LIBRARYMANAGER_H

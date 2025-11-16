#include "CommandInfo.h"
#include "CommandParser.h"
#include "LibraryManager.h"
#include <iostream>
#include <string_view>

int main(int argc, char* argv[])
{
    CommandParser parsedCmd{argc, argv};

    if (!parsedCmd.isValid())
    {
        const auto [errorType, arg] = parsedCmd.error();
        CommandInfo::printError(errorType, arg);
        return errorType;
    }
    if (parsedCmd.isHelp())
    {
        CommandInfo::printHelp();
        return 0;
    }

    LibraryManager libraryManager{parsedCmd};
    libraryManager.executeCmd();

    return 0;
}

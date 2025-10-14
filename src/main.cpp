#include "CommandInfo.h"
#include "CommandParser.h"
#include <iostream>
#include <string_view>

int main(int argc, char* argv[])
{
    CommandParser parsedCmd{argc, argv};

    if (!parsedCmd.isValid())
    {
        auto [errorType, arg] = parsedCmd.error(); // get error code
        CommandInfo::printError(errorType, arg);
        return errorType;
    }
    if (parsedCmd.isHelp())
    {
        CommandInfo::printHelp();
        return 0;
    }

    std::cout << "The command given is: " << CommandInfo::toString(parsedCmd.commandType()) << '\n';
    if (parsedCmd.commandType() == CommandInfo::Init)
    {
        std::cout << "The Directory given is: " << parsedCmd.path() << '\n';
    }
    else { std::cout << "The path to file given is: " << parsedCmd.path() << '\n'; }

    for (const auto& [option, value] : parsedCmd.options())
    {
        std::cout << "Option: " << option << " | Value: " << value << '\n';
    }

    return 0;
}

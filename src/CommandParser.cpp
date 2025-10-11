#include "CommandParser.h"
#include <algorithm>
#include <string_view>

using CommandInfo::commandNames;
using CommandInfo::ParseError;

std::string_view CommandParser::getCmdStr()
{
    return CommandInfo::commandNames[static_cast<std::size_t>(m_commandType)];
}

CommandParser::CommandParser(int argc, char* argv[])
{
    if (argc == 1) // only one argument given (only argv[0])
    {
        m_error = ParseError::MissingCommand;
        return;
    }

    // get command type (only in argv[1]) e.g: bookit addbook ...
    std::string_view inputCommand{argv[1]};

    auto it = std::find(commandNames.begin(), commandNames.end(), inputCommand);

    if (it == commandNames.end()) // command not found!
    {
        m_error = ParseError::UnknownCommand;
        return;
    }

    auto typeIndex = it - commandNames.begin();

    m_commandType = static_cast<CommandInfo::Type>(typeIndex);

    // set found command type
}

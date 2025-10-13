#include "CommandParser.h"
#include <algorithm>
#include <string_view>

using CommandInfo::commandNames;
using CommandInfo::ParseError;

// Constructor: parses the main command, file/dir location, options (e.g: --author: "Martin")
CommandParser::CommandParser(int argc, char* argv[]) : m_argc{argc}, m_argv{argv}
{
    if (m_argc == 1) // only one argument given (only argv[0])
    {
        m_error = ParseError::MissingCommand;
        return;
    }

    if (!parseCmd()) { return; } // failed to parse main command
}

// Returns a string of the command provided by the user
std::string_view CommandParser::getCmdStr()
{
    return CommandInfo::commandNames[static_cast<std::size_t>(m_commandType)];
}

/*
    helper functions
*/

// parses main command group (e.g: init, addbook, etc.)
bool CommandParser::parseCmd()
{
    // get command type (only in m_argv[1]) e.g: bookit addbook ...
    std::string_view inputCommand{m_argv[1]};

    auto it = std::find(commandNames.begin(), commandNames.end(), inputCommand);

    if (it == commandNames.end()) // command not found!
    {
        m_error = ParseError::UnknownCommand;
        return false;
    }

    auto typeIndex = it - commandNames.begin();
    // set found command type
    m_commandType = static_cast<CommandInfo::Type>(typeIndex);

    return true;
}

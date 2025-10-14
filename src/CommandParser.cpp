#include "CommandParser.h"
#include "CommandInfo.h"
#include <algorithm>
#include <string_view>

using CommandInfo::commandNames;
using CommandInfo::ParseError;
using CommandInfo::Type;
using Error = CommandParser::Error;

// Constructor: parses the main command, file/dir location, options (e.g: --author: "Martin")
CommandParser::CommandParser(int argc, char* argv[]) : m_argc{argc}, m_argv{argv}
{
    if (m_argc == 1) // only one argument given (only argv[0])
    {
        m_error.emplace(Error{ParseError::MissingCommand});
        return;
    }

    if (!parseCmd()) { return; }  // return on failure to parse main command
    if (!parsePath()) { return; } // return on failure to parse path
}

/*
    Parsing helper functions
*/

// Parses main command group (e.g: init, addbook, etc.)
bool CommandParser::parseCmd()
{
    // get command type (only in m_argv[1]) e.g: bookit addbook ...
    std::string_view inputCommand{m_argv[1]};

    auto it = std::find(commandNames.begin(), commandNames.end(), inputCommand);

    if (it == commandNames.end()) // command not found!
    {
        m_error.emplace(Error{ParseError::UnknownCommand, m_argv[1]});
        return false;
    }

    auto typeIndex = it - commandNames.begin();
    // set found command type
    m_commandType = static_cast<CommandInfo::Type>(typeIndex);

    return true;
}

// Parses user provided path argument (file - direcory)
bool CommandParser::parsePath()
{
    if (m_argc < 3) // no path argument to (file/directory) given
    {
        m_error.emplace(Error{ParseError::MissingArgument});
        return false;
    }

    // get path argument (only in m_argv[2]) e.g: bookit addbook ./path/to/file
    std::string_view inputPath{m_argv[2]};

    // check if argument is an option, not a path
    if (inputPath.substr(0, 2).compare("--") == 0)
    {
        m_error.emplace(Error{ParseError::InvalidPath, m_argv[2]});
        return false;
    }

    m_path = inputPath;

    // check if path argument is a directory for 'init' command
    if (m_commandType == Type::Init) { return checkIfDir(); }

    return checkIfFile();
}

// Helper function for special directory cases [. / ..]
static bool isSpecialDir(const fs::path& path)
{
    const auto& filename = path.filename();
    return filename == "." || filename == "..";
}

// Checks if provided path is a file
bool CommandParser::checkIfFile()
{
    if (!m_path.has_filename() || isSpecialDir(m_path))
    {
        m_error.emplace(Error{ParseError::NotAFile, m_argv[2]});
        return false;
    }

    return true;
}

// Checks if provided path is a directory
bool CommandParser::checkIfDir()
{
    if (m_argc > 3)
    {
        m_error.emplace(Error{ParseError::UnexpectedArgs, m_argv[3]});
        return false;
    }

    if (m_path.has_filename() && !isSpecialDir(m_path))
    {
        m_error.emplace(Error{ParseError::NotADirectory, m_argv[2]});
        return false;
    }

    return true;
}

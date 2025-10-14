#include "CommandParser.h"
#include "CommandInfo.h"
#include <algorithm>
#include <string_view>

using CommandInfo::commandNames;
using CommandInfo::optionNames;
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

    if (!std::string_view{m_argv[1]}.compare("--help"))
    {
        m_isHelp = true;
        return;
    }

    if (!parseCmd()) { return; }     // return on failure to parse main command
    if (!parsePath()) { return; }    // return on failure to parse path
    if (!parseOptions()) { return; } // return on failure to parse options
}

/**
 * ================================================================
 *   [Private parsing helper member functions]
 * ================================================================
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
    if (inputPath.compare(0, 2, "--") == 0)
    {
        m_error.emplace(Error{ParseError::InvalidPath, m_argv[2]});
        return false;
    }

    m_path = inputPath;

    // check if path is a directory for 'init' command, else check for file
    return (m_commandType == Type::Init) ? checkIfDir() : checkIfFile();
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

// Helper function checks if provided option is valid
static bool isOptValid(std::string_view inputOption, std::optional<CommandParser::Error>& outError)
{
    auto it = std::find(optionNames.begin(), optionNames.end(), inputOption);
    if (it == optionNames.end()) // option not found
    {
        if (inputOption.compare(0, 2, "--") == 0) // case: [--invalid] option passed
        {
            outError.emplace(Error{ParseError::InvalidOption, inputOption});
            return false;
        }
        // case: non option string passed
        outError.emplace(Error{ParseError::UnexpectedArgs, inputOption});
        return false;
    }
    return true;
}

// Parses user provided options (--author "Martin")
bool CommandParser::parseOptions()
{
    // Options shall start from [m_argv[3]]
    // case: no options to extract [valid] - e.g: bookit addbook ./file.pdf
    constexpr int kOptStartIndex = 3;
    if (m_argc == kOptStartIndex) { return true; }

    int optsArgc{m_argc - kOptStartIndex};
    char** optsArgv{m_argv + kOptStartIndex};

    for (int i = 0; i < optsArgc;)
    {
        std::string_view inputOption{optsArgv[i++]};

        // check if option is valid
        if (!isOptValid(inputOption, m_error)) { return false; }

        // no value provided for option OR unexpected option given instead of value
        if (i >= optsArgc || (std::string_view{optsArgv[i]}.compare(0, 2, "--") == 0))
        {
            m_error.emplace(Error{ParseError::NoOptValue, inputOption});
            return false;
        }

        std::string_view inputOptValue{optsArgv[i++]};
        m_option[inputOption] = inputOptValue;
    }

    return true; // all possible options extracted successfully
}

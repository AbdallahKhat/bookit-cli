#include "CommandInfo.h"
#include "CommandParser.h"
#include "iostream"
#include "string_view"
namespace CommandInfo
{

// convert command type to string
const std::string_view toString(CommandInfo::Type type) noexcept
{
    return CommandInfo::commandNames[static_cast<std::size_t>(type)];
}

// print a help message for usage and available commands.
void printHelp() noexcept
{
    std::cout << "bookit: --help: WIP\n";
}

static void tryHelpStr() noexcept
{
    std::cerr << "Try 'bookit --help' for more infomration.\n";
}

static bool looksLikeOption(std::string_view arg) noexcept
{
    return (arg.compare(0, 2, "--") == 0);
}

// print an error message corresponding to a specific ParseError.
void printError(ParseError errorType, const std::string_view arg) noexcept
{
    switch (errorType)
    {
    case MissingCommand:
        std::cerr << "bookit: missing command\n";
        break;
    case UnknownCommand:
        looksLikeOption(arg)
            ? std::cerr << "bookit: unexpected option '" << arg << "'; expected a command\n"
            : std::cerr << "bookit: invalid command " << '\'' << arg << "\'\n";
        break;
    case MissingArgument:
        std::cerr << "bookit: Missing file operand\n";
        break;
    case InvalidPath:
        std::cerr << "bookit: unexpected option '" << arg << "'\n";
        break;
    case NotAFile:
        looksLikeOption(arg)
            ? std::cerr << "bookit: unexpected option '" << arg << "'; expected a file\n"
            : std::cerr << "bookit: target '" << arg << "' is not a file\n";
        break;
    case NotADirectory:
        looksLikeOption(arg)
            ? std::cerr << "bookit: unexpected option '" << arg << "'; expected a directory\n"
            : std::cerr << "bookit: target '" << arg << "' is not a directory\n";
        break;
    case UnexpectedArgs:
        std::cerr << "bookit: unexpected argument '" << arg << "\'\n";
        break;
    case InvalidOption:
        std::cerr << "error: no such option '" << arg << "'\n";
        break;
    case NoOptValue:
        std::cerr << "error: option '" << arg << "' requires a value\n";
        break;
    default:
        std::cerr << "bookit: an unexpected error has occurred\n";
    }
    tryHelpStr();
}

} // namespace CommandInfo

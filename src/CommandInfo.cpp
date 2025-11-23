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
    using std::cout;

    cout << "bookit - PDF workspace manager\n\n";
    cout << "Usage:\n";
    cout << "  bookit <command> [arguments] [options]\n\n";

    cout << "Workspace commands:\n";
    cout << "  init <directory>         Initialize a workspace at <directory> (creates .bookit).\n";
    cout << "  listws                   List all tracked workspaces and show the current one.\n";
    cout << "  switchws <directory>     Switch the current workspace to an existing entry.\n";
    cout << "  removews <directory>     Unregister a workspace from the tracked list (keeps files).\n\n";

    cout << "Library commands (operate on the current workspace):\n";
    cout << "  addbook <path/to/book.pdf> [options]\n";
    cout << "                           Copy a PDF into the workspace and add metadata.\n";
    cout << "  removebook <book.pdf>    Delete the PDF and its metadata entry.\n";
    cout << "  updatebook <book.pdf> [options]\n";
    cout << "                           Update metadata fields for an existing book.\n";
    cout << "  listbooks                List all books stored in the workspace.\n";
    cout << "  openbook <book.pdf>      Open a book using the system PDF viewer.\n\n";

    cout << "Metadata options (usable with addbook/updatebook):\n";
    cout << "  --author <name>          Author of the book.\n";
    cout << "  --year <year>            Publication year.\n";
    cout << "  --isbn <value>           ISBN identifier.\n";
    cout << "  --category <label>       Arbitrary category or shelf label.\n\n";

    cout << "Global options:\n";
    cout << "  --help                   Show this help message.\n";
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
        std::cerr << "bookit: unexpected option '" << arg
                  << "'; expected a path\n";
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

#include "CommandInfo.h"
#include "CommandParser.h"
#include <iostream>

void tryHelpStr()
{
    std::cerr << "Try 'bookit --help' for more infomration.\n";
}

// prints error message.
void printError(CommandParser::Error error)
{
    using namespace CommandInfo;
    
    const auto& arg = error.invalidArg;
    switch (error.type)
    {
    case MissingCommand:
        std::cerr << "bookit: missing command\n";
        break;
    case UnknownCommand:
        std::cerr << "bookit: invalid command " << '\'' << arg << "\'\n";
        break;
    case MissingArgument:
        std::cerr << "bookit: Missing file operand\n";
        break;
    case InvalidPath:
        std::cerr << "bookit: invalid path argument '" << arg << "'\n";
        break;
    case NotAFile:
        std::cerr << "bookit: target '" << arg << "' is not a file\n";
        break;
    case NotADirectory:
        std::cerr << "bookit: target '" << arg << "' is not a directory\n";
        break;
    case UnexpectedArgs:
        std::cerr << "bookit: unexpected argument '" << arg << "\'\n";
        break;
    case InvalidOption:
    default:
        std::cerr << "bookit: an unexpected error has occurred\n";
    }
    tryHelpStr();
}

int main(int argc, char* argv[])
{
    // std::cout << "Hello, BookIt Project!" << '\n';

    CommandParser parsedCmd{argc, argv};

    if (!parsedCmd.isValid())
    {
        CommandParser::Error error = parsedCmd.error(); // get error code

        printError(error);

        return error.type;
    }

    std::cout << "The command given is: " << CommandInfo::toString(parsedCmd.commandType()) << '\n';
    if (parsedCmd.commandType() == CommandInfo::Init)
    {
        std::cout << "The Directory given is: " << parsedCmd.path() << '\n';
    }
    else { std::cout << "The path to file given is: " << parsedCmd.path() << '\n'; }

    return 0;
}

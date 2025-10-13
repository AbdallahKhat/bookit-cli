#include "CommandInfo.h"
#include "CommandParser.h"
#include <iostream>

void tryHelpStr()
{
    std::cerr << "Try 'bookit --help' for more infomration.\n";
}

// prints error message.
void printError(CommandInfo::ParseError error, char* argv[])
{
    using namespace CommandInfo;

    switch (error)
    {
    case MissingCommand:
        std::cerr << "bookit: missing command\n";
        tryHelpStr();
        return;

    case UnknownCommand:
        std::cerr << "bookit: invalid command: " << '\'' << argv[1] << "\'\n";
        tryHelpStr();
        return;

    case MissingArgument:
        std::cerr << "bookit: Missing file operand\n";
        tryHelpStr();
        return;

    case InvalidPath:
        std::cerr << "bookit: invalid path argument\n";
        tryHelpStr();
        return;

    case NotAFile:
        std::cerr << "bookit: target must be a file\n";
        tryHelpStr();
        return;

    case NotADirectory:
        std::cerr << "bookit: target must be a directory\n";
        tryHelpStr();
        return;

    case UnexpectedArgs:
        std::cerr << "bookit: too many arguments\n";
        tryHelpStr();
        return;

    case InvalidOption:
    default:
        std::cerr << "bookit: an unexpected error has occurre\n";
    }
}

int main(int argc, char* argv[])
{
    // std::cout << "Hello, BookIt Project!" << '\n';

    CommandParser parsedCmd{argc, argv};

    if (!parsedCmd.isValid())
    {
        CommandInfo::ParseError err = parsedCmd.getError(); // get error code

        printError(err, argv);

        return err;
    }

    std::cout << "The command given is: " << CommandInfo::toString(parsedCmd.commandType()) << '\n';
    if (parsedCmd.commandType() == CommandInfo::Init)
    {
        std::cout << "The Directory given is: " << parsedCmd.path() << '\n';
    }
    else { std::cout << "The path to file given is: " << parsedCmd.path() << '\n'; }

    return 0;
}

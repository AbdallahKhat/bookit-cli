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
        std::cerr << "bookit: Missing command. \n";
        tryHelpStr();
        return;

    case UnknownCommand:
        std::cerr << "bookit: Invalid command: " << '\'' << argv[1] << "\'\n";
        tryHelpStr();
        return;
    case MissingArgument:
    case InvalidOption:
    case InvalidPath:
    default:
        std::cerr << "An unexpected error has occurred.\n";
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

    std::cout << "The command given is: " << parsedCmd.getCmdStr() << '\n';

    return 0;
}

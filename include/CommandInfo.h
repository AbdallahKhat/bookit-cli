#ifndef COMMANDINFO_H
#define COMMANDINFO_H

#include <array>
#include <string_view>

namespace CommandInfo
{
enum Type // all available commands
{
    Init,           // bookit init ./path/dir/
    AddBook,        // bookit addbook path/file.pdf [adds file to library]
    MaxCommands,
};

enum ParseError
{
    None,            // success
    MissingCommand,  // bookit
    UnknownCommand,  // bookit somethingInvalid
    MissingArgument, // bookit addbook [missing file/option/etc]
    InvalidOption,   // bookit addbook --weirdFlag
    InvalidPath,     // bookit addbook invalid/path
};

constexpr CommandInfo::Type Invalid{MaxCommands}; // used as default init

using namespace std::string_view_literals; // for sv suffix
constexpr std::array commandNames{"init"sv, "addbook"sv};

static_assert(std::size(commandNames) == CommandInfo::MaxCommands, "mismatched number of commands");
} // namespace CommandInfo

#endif // COMMANDINFO_H

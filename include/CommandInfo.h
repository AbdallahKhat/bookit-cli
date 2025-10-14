#ifndef COMMANDINFO_H
#define COMMANDINFO_H

#include <array>
#include <string_view>

namespace CommandInfo
{
enum Type // all available commands
{
    Init,    // bookit init ./path/dir/
    AddBook, // bookit addbook path/file.pdf [adds file to library]
    MaxCommands,
};

enum ParseError
{
    None,            // success
    MissingCommand,  // bookit
    UnknownCommand,  // bookit somethingInvalid
    MissingArgument, // bookit addbook [missing file/option/etc]
    InvalidPath,     // bookit addbook invalid/path
    NotAFile,        // bookit addbook ./dir/
    NotADirectory,   // bookit init ./file.pdf
    UnexpectedArgs,  // unexpected arguments
    InvalidOption,   // bookit addbook --weirdFlag
    NoOptValue,      // bookit addbook --author [empty]
};

constexpr CommandInfo::Type Invalid{MaxCommands}; // used as default init

using namespace std::string_view_literals; // for sv suffix
constexpr std::array commandNames{"init"sv, "addbook"sv};
constexpr std::array optionNames{"--author"sv, "--year"sv};

static_assert(std::size(commandNames) == CommandInfo::MaxCommands, "mismatched number of commands");

inline const std::string_view toString(CommandInfo::Type type) noexcept
{
    return CommandInfo::commandNames[static_cast<std::size_t>(type)];
}

} // namespace CommandInfo

#endif // COMMANDINFO_H

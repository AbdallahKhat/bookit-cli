#ifndef COMMANDINFO_H
#define COMMANDINFO_H

#include <array>
#include <string_view>

namespace CommandInfo
{
enum Type // all available commands
{
    Init,       // bookit init ./path/dir/
    AddBook,    // bookit addbook path/file.pdf [adds file to library]
    RemoveBook, // bookit removebook book.pdf
    UpdateBook, // bookit updatebook book.pdf [updates metadata tags]
    ListBooks,  // bookit listbooks [prints workspace books]
    OpenBook,   // bookit openbook book.pdf
    ListWs,     // bookit listws [lists known workspaces]
    SwitchWs,   // bookit switchws /path/to/ws
    RemoveWs,   // bookit removews /path/to/ws
    MaxCommands,
};

enum ParseError
{
    None,            // success
    MissingCommand,  // bookit
    UnknownCommand,  // bookit somethingInvalid
    MissingArgument, // bookit addbook [missing file/option/etc]
    InvalidPath,     // bookit addbook invalid/path
    UnexpectedArgs,  // unexpected arguments
    InvalidOption,   // bookit addbook --weirdFlag
    NoOptValue,      // bookit addbook --author [empty]
};

inline constexpr CommandInfo::Type Invalid{MaxCommands}; // used as default init

using namespace std::string_view_literals; // for sv suffix
inline constexpr std::array commandNames{"init"sv,       "addbook"sv,   "removebook"sv,
                                         "updatebook"sv, "listbooks"sv, "openbook"sv,
                                         "listws"sv,     "switchws"sv,  "removews"sv};
inline constexpr std::array optionNames{"--author"sv, "--year"sv, "--isbn"sv, "--category"sv};

static_assert(std::size(commandNames) == CommandInfo::MaxCommands, "mismatched number of commands");

// ---------------------------
// Public printing interface
// ---------------------------

// convert command type to string
const std::string_view toString(CommandInfo::Type type) noexcept;

// print a help message showing usage and available commands.
void printHelp() noexcept;

// print an error message corresponding to a specific ParseError.
void printError(ParseError errorType, const std::string_view arg) noexcept;

} // namespace CommandInfo

#endif // COMMANDINFO_H

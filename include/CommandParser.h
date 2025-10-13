#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include "CommandInfo.h"
#include <filesystem>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace fs = std::filesystem;

// CommandParser class: Parses commands and sets them to (m_commandType, m_file ,m_option)
class CommandParser
{
public:
    using Options = std::unordered_map<std::string_view, std::string_view>;

    // Constructors/Destructors
    CommandParser(int argc, char* argv[]);
    ~CommandParser() = default;

    CommandParser(const CommandParser&) = delete;
    CommandParser& operator=(const CommandParser&) = delete;
    CommandParser(CommandParser&&) = default;
    CommandParser& operator=(CommandParser&&) = default;

    // Accessors
    std::string_view getCmdStr(); // gets command string from std::array CommandInfo::commandNames

    bool isValid() const { return !m_error.has_value(); }
    CommandInfo::ParseError getError() const { return *m_error; }

private:
    // Member variables
    CommandInfo::Type m_commandType{CommandInfo::Invalid};
    fs::path m_Path{};
    Options m_option{};

    std::optional<CommandInfo::ParseError> m_error{};
};

#endif // COMMANDPARSER_H

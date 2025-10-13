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
    CommandInfo::Type commandType() const noexcept { return m_commandType; }
    const fs::path& path() const noexcept { return m_path; }

    bool isValid() const { return !m_error.has_value(); }
    CommandInfo::ParseError getError() const { return *m_error; }

private:
    // Parsing helper functions
    bool parseCmd();
    bool parsePath();
    bool checkIfFile();
    bool checkIfDir();

    // Member variables
    CommandInfo::Type m_commandType{CommandInfo::Invalid};
    fs::path m_path{};
    Options m_option{};

    std::optional<CommandInfo::ParseError> m_error{};

    const int m_argc{};
    char** const m_argv{};
};

#endif // COMMANDPARSER_H

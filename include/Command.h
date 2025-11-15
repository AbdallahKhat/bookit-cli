#ifndef COMMAND_H
#define COMMAND_H

#include <filesystem>
#include <string>
#include <unordered_map>

class Command
{
public:
    virtual ~Command() {};

    // Pre-execution validation
    virtual bool validate() const = 0;

    // Core command execution
    virtual void execute() const = 0;

protected:
    // Shared validation helpers

    bool validateDirectory(const std::filesystem::path& path) const;

    bool validateFile(const std::filesystem::path& path) const;
};

// All concrete command classes:
class InitCommand : public Command
{
public:
    InitCommand(const std::filesystem::path& path) : m_path{path} {}
    bool validate() const override;
    void execute() const override;

private:
    const std::filesystem::path& m_path;
};

class AddBookCommand : public Command
{
public:
    using Options = std::unordered_map<std::string_view, std::string_view>;

    AddBookCommand(const std::filesystem::path& filePath, const Options& options,
                   const std::filesystem::path& wsDir)
        : m_filePath{filePath}, m_options{options}, m_wsDir{wsDir}
    {
    }

    bool validate() const override;
    void execute() const override;

private:
    std::filesystem::path m_filePath;
    Options m_options;
    std::filesystem::path m_wsDir;
};

#endif // COMMAND_H

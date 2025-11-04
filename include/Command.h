#ifndef COMMAND_H
#define COMMAND_H

#include <filesystem>
#include <string>

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

#endif // COMMAND_H

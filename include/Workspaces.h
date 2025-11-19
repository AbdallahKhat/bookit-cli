#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include <filesystem>
#include <nlohmann/json.hpp>

namespace Bookit
{

class Workspaces
{
public:
    Workspaces();

    void addWorkspace(const std::filesystem::path&);
    void listWorkspaces();
    void switchWorkspace(const std::filesystem::path& targetPath);
    void removeWorkspace(const std::filesystem::path& targetPath);
    const std::filesystem::path& getCurrentWorkspace() const noexcept { return m_currentWorkspace; }

private:
    std::filesystem::path m_currentWorkspace{};
    std::filesystem::path m_configFile{};

    std::filesystem::path getConfigFilePath() const;
    nlohmann::json createDefaultConfig() const;
    nlohmann::json loadWorkspaces();
    void setCurrent(const std::string& absolutePath);
    void saveConfig(const nlohmann::json& config);
};

} // namespace Bookit

#endif // WORKSPACEMANAGER_H

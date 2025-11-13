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
    // std::string getCurrent();

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

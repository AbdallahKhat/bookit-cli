#include <Workspaces.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string_view>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace
{
void unlockWorkspaceDirectory(const fs::path& dir)
{
    if (fs::exists(dir))
        fs::permissions(dir, static_cast<fs::perms>(0755), fs::perm_options::replace);
}

std::string ensureTrailingSlash(const fs::path& absolutePath)
{
    std::string str = absolutePath.string();

    // If it’s a directory and doesn’t end with '/', add it
    if (!str.empty() && std::filesystem::is_directory(absolutePath) && str.back() != '/')
        str.push_back('/');

    return str;
}

bool ensurePathProvided(const fs::path& path, std::string_view action)
{
    if (!path.empty()) { return true; }
    std::cerr << "Error: Missing workspace path to " << action << '\n';
    return false;
}

json* requireWorkspaceArray(json& config)
{
    if (!config.contains("workspaces") || !config["workspaces"].is_array())
    {
        std::cerr << "Error: Workspace configuration is corrupted; no workspace list found\n";
        return nullptr;
    }
    return &config["workspaces"];
}

std::string normalizedWorkspacePath(const fs::path& path)
{
    return ensureTrailingSlash(fs::absolute(path).lexically_normal());
}

json::iterator findWorkspaceEntry(json& workspaces, const std::string& pathStr)
{
    return std::find_if(workspaces.begin(), workspaces.end(), [&pathStr](const json& ws) {
        return ws.is_string() && ws.get<std::string>() == pathStr;
    });
}
} // namespace

namespace Bookit
{

Workspaces::Workspaces() : m_configFile{getConfigFilePath()}
{
    // Check if workspaces.json exists
    if (fs::exists(m_configFile))
    {
        // File exists, load the current workspace from it
        auto config = loadWorkspaces();
        if (config.contains("current") && !config["current"].is_null())
        {
            m_currentWorkspace = config["current"].get<std::string>();
        }
    }
    else
    {
        // File doesn't exist, create it with default structure
        auto defaultConfig = createDefaultConfig();

        // Ensure the directory exists
        auto configDir = m_configFile.parent_path();
        if (!fs::exists(configDir)) { fs::create_directories(configDir); }

        // Write default config to file
        std::ofstream configStream{m_configFile};
        configStream << defaultConfig.dump(4);
        configStream.close();
    }
}

// Get the configuration file path (same directory as executable)
std::filesystem::path Workspaces::getConfigFilePath() const
{
    std::error_code ec;
    auto exePath = fs::read_symlink("/proc/self/exe", ec);
    if (ec)
    {
        std::cerr << "Error: could not resolve /proc/self/exe: " << ec.message() << '\n';
        return {};
    }
    return exePath.parent_path() / "workspaces.json";
}

// Create default configuration structure
nlohmann::json Workspaces::createDefaultConfig() const
{
    json defaultConfig;
    defaultConfig["current"] = "";               // Empty current workspace path
    defaultConfig["workspaces"] = json::array(); // Empty array of initialized workspaces
    return defaultConfig;
}

// Load JSON from the file or return an empty default.
json Workspaces::loadWorkspaces()
{
    if (!fs::exists(m_configFile)) { return createDefaultConfig(); }

    std::ifstream configStream{m_configFile};
    json config;
    configStream >> config;
    configStream.close();

    return config;
}

// Add a workspace to the list if it doesn't already exist and sets it to current workspace
void Workspaces::addWorkspace(const std::filesystem::path& path)
{
    auto config = loadWorkspaces();
    auto* workspaces = requireWorkspaceArray(config);
    if (!workspaces) { return; }

    const std::string pathStr = normalizedWorkspacePath(path);

    auto it = findWorkspaceEntry(*workspaces, pathStr);

    // If workspace doesn't exist, add it
    if (it == workspaces->end())
    {
        workspaces->push_back(pathStr);
        saveConfig(config); // Save the updated config with new workspace
    }

    // Set as current workspace
    setCurrent(pathStr);
}

// Set the current workspace and save to file
void Workspaces::setCurrent(const std::string& absolutePath)
{
    auto config = loadWorkspaces();

    // Update current workspace
    m_currentWorkspace = absolutePath;
    config["current"] = absolutePath;

    // Save the updated config to file
    saveConfig(config);
}

// Helper function to save configuration to file
void Workspaces::saveConfig(const nlohmann::json& config)
{
    std::ofstream configStream{m_configFile};
    configStream << config.dump(4);
    configStream.close();
}

void Workspaces::listWorkspaces()
{
    auto config = loadWorkspaces();
    auto* workspaces = requireWorkspaceArray(config);
    if (!workspaces) { return; }

    std::string current;
    if (config.contains("current") && config["current"].is_string())
    {
        current = config["current"].get<std::string>();
    }

    std::cout << "CURRENT: " << (current.empty() ? std::string{"(none)"} : current) << '\n';
    std::cout << "Available:\n";

    if (workspaces->empty())
    {
        std::cout << "(none)\n";
        return;
    }

    for (const auto& wsEntry : *workspaces)
    {
        if (wsEntry.is_string()) { std::cout << wsEntry.get<std::string>() << '\n'; }
    }
}

void Workspaces::switchWorkspace(const std::filesystem::path& targetPath)
{
    if (!ensurePathProvided(targetPath, "switch to")) { return; }

    auto config = loadWorkspaces();
    auto* workspaces = requireWorkspaceArray(config);
    if (!workspaces) { return; }

    const std::string pathStr = normalizedWorkspacePath(targetPath);
    auto it = findWorkspaceEntry(*workspaces, pathStr);

    if (it == workspaces->end())
    {
        std::cerr << "Error: Workspace '" << pathStr
                  << "' has not been initialized. Use 'bookit init <dir>' first.\n";
        return;
    }

    setCurrent(pathStr);
    std::cout << "Switched current workspace to: " << pathStr << '\n';
}

void Workspaces::removeWorkspace(const std::filesystem::path& targetPath)
{
    if (!ensurePathProvided(targetPath, "remove")) { return; }

    auto config = loadWorkspaces();
    auto* workspaces = requireWorkspaceArray(config);
    if (!workspaces) { return; }

    const std::string pathStr = normalizedWorkspacePath(targetPath);
    auto it = findWorkspaceEntry(*workspaces, pathStr);

    if (it == workspaces->end())
    {
        std::cerr << "Error: Workspace '" << pathStr
                  << "' is not tracked. Use 'bookit init <dir>' first.\n";
        return;
    }

    workspaces->erase(it);

    if (config.contains("current") && config["current"].is_string() &&
        config["current"].get<std::string>() == pathStr)
    {
        m_currentWorkspace.clear();
        config["current"] = "";
    }

    saveConfig(config);
    unlockWorkspaceDirectory(fs::path{pathStr});
    std::cout << "Removed workspace: " << pathStr << '\n';
    std::cout << "Note: the directory was left on disk for safety "
                 "(in case you need to back up any book).\n";
    std::cout << "To remove it manually (Linux):  rm -r \"" << pathStr << "\"\n";
    std::cout << "This permanently deletes files — make sure important PDFs are backed up.\n";
}

}; // namespace Bookit

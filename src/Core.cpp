#include "Core.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

namespace
{
// set directory as writable to modify directory content
void setDirOwnerRWX(fs::path path)
{
    fs::permissions(path, static_cast<fs::perms>(0755), fs::perm_options::replace);
}

// lock directory as read-only to prevent accidental deletions
void setDirR_X(fs::path path)
{
    fs::permissions(path, static_cast<fs::perms>(0555), fs::perm_options::replace);
}

} // namespace

namespace BookitCore
{

void initializeWorkspace(const fs::path& wsDir)
{
    // TODO: Make command more robust (permissions, errors, wsDir ownership checks)

    setDirOwnerRWX(wsDir);

    fs::path bookitDir = wsDir / ".bookit";
    std::error_code ec;
    if (!fs::create_directory(bookitDir, ec))
    {
        if (ec)
        {
            std::cerr << "Error: failed creating .bookit/ directory: " << ec.message() << '\n';
            return;
        }
        std::cerr << "Error: .bookit directory already exists\n";
    }

    ordered_json bookitMetadata = {{"version", 1}, {"books", ordered_json::array()}};
    std::ofstream metadataFile{bookitDir / "metadata.json"};

    metadataFile << bookitMetadata.dump(4);
    metadataFile.close();

    setDirR_X(wsDir);
}

} // namespace BookitCore

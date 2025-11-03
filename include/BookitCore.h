#ifndef BOOKITCORE_H
#define BOOKITCORE_H

#include <filesystem>

namespace BookitCore
{
void initializeWorkspace(const std::filesystem::path& wsDir);
} // namespace BookitCore

#endif // BOOKITCORE_H

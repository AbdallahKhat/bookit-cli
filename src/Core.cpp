#include "Core.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <string_view>
#include <vector>

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

bool validateWorkspaceAndMetadata(const fs::path& wsDir)
{
    if (!fs::exists(wsDir) || !fs::is_directory(wsDir))
    {
        std::cerr << "Error: Workspace directory '" << wsDir.string() << "' does not exist\n";
        return false;
    }

    auto metadataPath = wsDir / ".bookit" / "metadata.json";
    if (!fs::exists(metadataPath))
    {
        std::cerr << "Error: Workspace metadata file not found. Workspace may be corrupted\n";
        return false;
    }

    return true;
}

bool isPdfFile(const fs::path& filePath)
{
    std::ifstream file{filePath, std::ios::binary};
    if (!file)
    {
        std::cerr << "Error: Unable to open file '" << filePath.string() << "'\n";
        return false;
    }

    char signature[4]{};
    file.read(signature, sizeof(signature));
    if (!file)
    {
        std::cerr << "Error: Failed reading file header for '" << filePath.string() << "'\n";
        return false;
    }

    return signature[0] == '%' && signature[1] == 'P' && signature[2] == 'D' && signature[3] == 'F';
}

std::vector<Bookit::Book> loadBooksFromMetadata(const ordered_json& metadata)
{
    std::vector<Bookit::Book> books;

    for (const auto& entry : metadata["books"])
    {
        Bookit::Book loadedBook;
        loadedBook.name = entry.value("name", "");
        loadedBook.author = entry.value("author", "");
        loadedBook.year = entry.value("year", "");
        loadedBook.isbn = entry.value("isbn", "");
        loadedBook.category = entry.value("category", "");
        books.push_back(std::move(loadedBook));
    }

    return books;
}

Bookit::Book createBookFromFileAndOptions(const fs::path& filePath,
                                          const CommandParser::Options& options)
{
    Bookit::Book book;
    book.name = filePath.filename().string();
    book.author = options.count("--author") ? std::string(options.at("--author")) : "";
    book.year = options.count("--year") ? std::string(options.at("--year")) : "";
    book.isbn = options.count("--isbn") ? std::string(options.at("--isbn")) : "";
    book.category = options.count("--category") ? std::string(options.at("--category")) : "";
    return book;
}

bool copyFileToWorkspace(const fs::path& filePath, const fs::path& wsDir)
{
    fs::path destPath = wsDir / filePath.filename();

    if (fs::exists(destPath))
    {
        std::cerr << "Error: A book named '" << filePath.filename().string()
                  << "' already exists in the workspace. Please rename the file and try again\n";
        return false;
    }

    std::error_code ec;
    fs::copy_file(filePath, destPath, fs::copy_options::none, ec);

    if (ec)
    {
        std::cerr << "Error: failed to copy book file: " << ec.message() << '\n';
        return false;
    }

    return true;
}

ordered_json createBookEntry(const Bookit::Book& book)
{
    ordered_json bookEntry;
    bookEntry["name"] = book.name;
    bookEntry["author"] = book.author;
    bookEntry["year"] = book.year;
    bookEntry["isbn"] = book.isbn;
    bookEntry["category"] = book.category;
    return bookEntry;
}

bool isBookDuplicate(const ordered_json& metadata, const Bookit::Book& book)
{
    auto existingBooks = loadBooksFromMetadata(metadata);
    auto duplicateIt =
        std::find_if(existingBooks.begin(), existingBooks.end(),
                     [&book](const Bookit::Book& existing) { return existing.name == book.name; });

    if (duplicateIt != existingBooks.end())
    {
        std::cerr << "Error: A book named '" << book.name
                  << "' already exists in metadata. Please rename the file before adding again\n";
        return true;
    }
    return false;
}

struct BookEntryLookup
{
    ordered_json* books;
    ordered_json::iterator entry;
};

ordered_json* getBooksArray(ordered_json& metadata)
{
    auto booksIt = metadata.find("books");
    if (booksIt == metadata.end() || !booksIt->is_array())
    {
        std::cerr << "Error: Metadata is corrupted; missing books array\n";
        return nullptr;
    }

    return &(*booksIt);
}

std::optional<ordered_json> readMetadata(const fs::path& metadataPath)
{
    std::ifstream metadataStream{metadataPath};
    if (!metadataStream)
    {
        std::cerr << "Error: Unable to open metadata file\n";
        return std::nullopt;
    }

    ordered_json metadata;
    metadataStream >> metadata;
    metadataStream.close();
    return metadata;
}

std::optional<BookEntryLookup> findBookEntry(ordered_json& metadata,
                                             const std::string_view bookName)
{
    auto* books = getBooksArray(metadata);
    if (!books) { return std::nullopt; }

    auto entryIt = std::find_if(books->begin(), books->end(), [&bookName](const ordered_json& entry)
                                { return entry.value("name", "") == bookName; });

    if (entryIt == books->end())
    {
        std::cerr << "Error: Book '" << bookName
                  << "' was not found in metadata. Nothing to remove.\n";
        return std::nullopt;
    }

    return BookEntryLookup{books, entryIt};
}

bool writeMetadata(const fs::path& metadataPath, const ordered_json& metadata)
{
    std::ofstream metadataOut{metadataPath};
    if (!metadataOut)
    {
        std::cerr << "Error: Unable to save metadata\n";
        return false;
    }

    metadataOut << metadata.dump(4);
    return true;
}

bool removeWorkspaceBookFile(const fs::path& wsDir, const std::string_view bookName)
{
    const auto bookPath = wsDir / bookName;
    if (!fs::exists(bookPath))
    {
        std::cerr << "Error: Book file '" << bookPath.string() << "' does not exist in workspace\n";
        return false;
    }

    std::error_code ec;
    if (!fs::remove(bookPath, ec) || ec)
    {
        std::cerr << "Error: Failed to delete book file '" << bookPath.string()
                  << "': " << ec.message() << '\n';
        return false;
    }

    return true;
}

std::string formatBookLine(const Bookit::Book& book)
{
    constexpr int fieldWidth = 20;
    std::ostringstream stream;
    stream << std::left << std::setfill(' ');

    auto appendField = [&](std::string_view label, const std::string& value, bool isLast = false) {
        stream << label << ": " << std::setw(fieldWidth) << (value.empty() ? "" : value);
        if (!isLast) { stream << " | "; }
    };

    appendField("Name", book.name);
    appendField("Author", book.author);
    appendField("Year", book.year);
    appendField("ISBN", book.isbn);
    appendField("Category", book.category, true);

    return stream.str();
}

} // namespace

void Bookit::Core::initializeWorkspace(const fs::path& wsDir)
{
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
        setDirR_X(wsDir);
        return;
    }

    ordered_json bookitMetadata = {{"version", 1}, {"books", ordered_json::array()}};
    std::ofstream metadataFile{bookitDir / "metadata.json"};

    metadataFile << bookitMetadata.dump(4);
    metadataFile.close();

    setDirR_X(wsDir);
}

void Bookit::Core::addBook(const fs::path& wsDir, const fs::path& filePath,
                           const CommandParser::Options& options)
{
    if (!validateWorkspaceAndMetadata(wsDir)) { return; }

    Book book = createBookFromFileAndOptions(filePath, options);

    if (!isPdfFile(filePath))
    {
        std::cerr << "Error: '" << filePath.string() << "' is not a valid PDF file\n";
        return;
    }

    setDirOwnerRWX(wsDir);

    if (!copyFileToWorkspace(filePath, wsDir))
    {
        setDirR_X(wsDir);
        return;
    }

    saveBookToMetadata(wsDir, book);
    setDirR_X(wsDir);
}

void Bookit::Core::removeBook(const fs::path& wsDir, const std::string_view bookFileName)
{
    if (!validateWorkspaceAndMetadata(wsDir)) { return; }

    const fs::path metadataPath = wsDir / ".bookit" / "metadata.json";

    auto metadata = readMetadata(metadataPath);
    if (!metadata) { return; }

    auto bookLookup = findBookEntry(*metadata, bookFileName);
    if (!bookLookup) { return; }

    setDirOwnerRWX(wsDir);

    if (!removeWorkspaceBookFile(wsDir, bookFileName))
    {
        setDirR_X(wsDir);
        return;
    }

    bookLookup->books->erase(bookLookup->entry);

    if (!writeMetadata(metadataPath, *metadata))
    {
        setDirR_X(wsDir);
        return;
    }

    setDirR_X(wsDir);
}

void Bookit::Core::listBooks(const fs::path& wsDir)
{
    if (!validateWorkspaceAndMetadata(wsDir)) { return; }

    const fs::path metadataPath = wsDir / ".bookit" / "metadata.json";

    auto metadata = readMetadata(metadataPath);
    if (!metadata) { return; }

    auto* booksArray = getBooksArray(*metadata);
    if (!booksArray) { return; }

    auto books = loadBooksFromMetadata(*metadata);
    if (books.empty())
    {
        std::cout << "No books found in workspace.\n";
        return;
    }

    std::sort(books.begin(), books.end(),
              [](const Bookit::Book& lhs, const Bookit::Book& rhs)
              { return lhs.name < rhs.name; });

    for (const auto& book : books)
    {
        std::cout << formatBookLine(book) << '\n';
    }
}

void Bookit::Core::saveBookToMetadata(const fs::path& wsDir, const Book& book)
{
    const fs::path metadataPath = wsDir / ".bookit" / "metadata.json";

    auto metadata = readMetadata(metadataPath);
    if (!metadata) { return; }

    auto* books = getBooksArray(*metadata);
    if (!books) { return; }

    if (isBookDuplicate(*metadata, book)) { return; }

    books->push_back(createBookEntry(book));
    (void)writeMetadata(metadataPath, *metadata);
}

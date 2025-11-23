## Bookit CLI

Bookit is a small command-line tool for managing personal PDF "workspaces" on your filesystem. It lets you initialize library directories, register and switch between them, and add, list, open, remove, and update PDF books with associated metadata.

**Highlights:** C++17 CLI, filesystem-heavy operations, JSON-based metadata, robust error handling.

---

## Technology Stack

- **Language:** C++17
- **Build system:** CMake (minimum 3.16)
- **Standard library:** `std::filesystem`, `<algorithm>`, `<optional>`, etc.
- **JSON library:** [nlohmann/json](https://github.com/nlohmann/json) (header-only, vendored in `third_party/`)

---

## Project Architecture

High-level components:

- `src/main.cpp`: Entry point; parses CLI arguments via `CommandParser`, then delegates to `LibraryManager`.
- `CommandParser`: Parses `argc/argv` into a command type (`CommandInfo::Type`), a path argument, and an option map.
- `CommandInfo`: Defines command and option enums/names and prints help and error messages.
- `LibraryManager`: Orchestrates execution of commands using `Core` and `Workspaces`.
- `Core`: Implements book-level operations inside a workspace (add, remove, list, open, update metadata).
- `Workspaces`: Manages the `workspaces.json` configuration next to the executable and tracks known workspaces and the current one.

Data layout:

- Each workspace directory contains a hidden `.bookit/metadata.json` file describing the workspace version and the list of books (name, author, year, isbn, category).
- A global `workspaces.json` next to the `bookit` binary tracks:
	- `current`: the current workspace path (string or empty).
	- `workspaces`: an array of workspace paths.

---

## Getting Started

### Prerequisites

- A C++17-capable compiler (e.g. `g++ >= 9`, `clang++ >= 10`).
- CMake **3.16+**.
- A POSIX-like environment (Linux is the primary target. uses `/proc/self/exe` to locate `workspaces.json` and `xdg-open` utility to open PDFs using the user's preferred application).

### Configure and Build

From the project root:

```bash
mkdir -p build
cd build
cmake -S .. -B .
cmake --build .
```

The resulting executable will be placed under:

- `build/bin/bookit`

You can then either add `build/bin` to your `PATH` or invoke the tool directly:

```bash
./bin/bookit --help
```

---

## Project Structure

```text
bookit-cli/
├── CMakeLists.txt          # CMake build configuration
├── include/                # Public headers
│   ├── Books.h
│   ├── Command.h
│   ├── CommandInfo.h
│   ├── CommandParser.h
│   ├── Core.h
│   ├── LibraryManager.h
│   └── Workspaces.h
├── src/                    # Implementation files
│   ├── Command.cpp
│   ├── CommandInfo.cpp
│   ├── CommandParser.cpp
│   ├── Core.cpp
│   ├── LibraryManager.cpp
│   ├── Workspaces.cpp
│   └── main.cpp
├── third_party/
│   └── nlohmann/json.hpp   # Header-only JSON library
└── README.md               # This file
```

Within a workspace directory (chosen by you):

```text
<workspace-root>/
├── .bookit/
│   └── metadata.json       # Workspace metadata and book list
├── some-book.pdf
└── another-book.pdf
```

---

## Key Features

- **Workspace management**
	- `init <directory>`: Initialize a new workspace (creates `.bookit/metadata.json`).
	- `listws`: List all tracked workspaces and show the current workspace.
	- `switchws <directory>`: Switch the current workspace to an existing entry.
	- `removews <directory>`: Unregister a workspace (leaves the actual directory and PDFs on disk; prints a manual `rm -r` hint).

- **Book management** (operate on the *current* workspace):
	- `addbook <path/to/book.pdf>`: Copies the PDF into the workspace and adds metadata.
	- `removebook <book.pdf>`: Deletes the PDF from the workspace and removes its metadata entry.
	- `updatebook <book.pdf>`: Updates existing metadata fields for a book.
	- `listbooks`: Prints all books in the workspace in a tabular line format.
	- `openbook <book.pdf>`: Opens a book using the system viewer (e.g. `xdg-open`).

- **Metadata options** for `addbook` and `updatebook`:
	- `--author <name>`
	- `--year <year>`
	- `--isbn <value>`
	- `--category <label>`

Robust error handling:

- Validates that workspaces and metadata files exist and are well-formed.
- Checks that book files are valid PDFs (via magic `%PDF` header) before import.
- Ensures consistency between metadata and actual files in the workspace.

---

## Using the CLI

Display help:

```bash
bookit --help
```

Initialize and use a workspace:

```bash
# Initialize a workspace directory
bookit init /path/to/my-workspace

# Switch to it (if not already current)
bookit switchws /path/to/my-workspace

# Add a book with metadata
bookit addbook /path/to/some-book.pdf \
	--author "Some Author" \
	--year "2024" \
	--isbn "978-3-16-148410-0" \
	--category "Programming"

# List all books in the current workspace
bookit listbooks

# Open a book using the system viewer
bookit openbook some-book.pdf

# Update metadata for an existing book
bookit updatebook some-book.pdf --author "New Author"

# Remove a book
bookit removebook some-book.pdf
```

Managing workspaces:

```bash
# List tracked workspaces
bookit listws

# Remove a tracked workspace (does NOT delete files)
bookit removews /path/to/my-workspace
```

---

## Development Workflow

During development, you typically:

- Edit code under `src/` and `include/`.
- Reconfigure if needed with CMake.
- Rebuild the `bookit` binary:

```bash
cd build
cmake --build .
```

---

## Coding Standards

- C++17, with warnings enabled: `-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -pedantic-errors`.
- Consistent use of `std::filesystem` for path handling.
- Error handling via `std::error_code` where applicable (e.g. filesystem operations) and clear `std::cerr` messages.
- JSON structures represented with `nlohmann::json` and validated before use (e.g. checking arrays and keys).
- Helper functions placed in anonymous namespaces in `.cpp` files to avoid leaking internal symbols.

---

## Contributing

Contributions are welcome but not expected. Feel free to open issues or pull requests. A lightweight flow is enough:

1. Fork the repository and create a feature branch.
2. Make changes in `src/` and `include/` following the existing style (warning-free build, modern C++17).
3. Rebuild and sanity-check core commands (e.g. `bookit --help`, `init`, `addbook`, `listbooks`).
4. Open a pull request with a short description of what changed and why.

---

## License

This project is licensed under the [MIT License](LICENSE). Please refer to that file for the full license text and terms under which Bookit is distributed.

---
**Status:** Maintained as a personal project. Stable for everyday use, with room for future enhancements (e.g. search/filtering, more metadata fields).

